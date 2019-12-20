/*
 * Microcurrent measuring device.
 */

#include <libe/libe.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <esp_timer.h>

#ifndef TARGET_ESP32
#error "This app only works in ESP32"
#endif

#define MULTIMETER_UDP_PORT     17001
#define MULTIMETER_TCP_PORT     17002
#define MULTIMETER_CONFIG \
		"device:name:Microcurrent\r\n" \
		"A:name:Current\r\n" \
		"A:type:current\r\n" \
		"A:mode:source\r\n" \
		"A:method:push\r\n" \
		"A:multiplier:0.001\r\n" \
		"A:base:10\r\n" \
		"A:resolution:0.000001\r\n" \
		"B:name:Current Avg.\r\n" \
		"B:type:current\r\n" \
		"B:mode:source\r\n" \
		"B:method:push\r\n" \
		"B:multiplier:0.001\r\n" \
		"B:resolution:0.000001\r\n" \
		"B:base:10\r\n" \
		"C:name:Current Peak\r\n" \
		"C:type:current\r\n" \
		"C:mode:source\r\n" \
		"C:method:push\r\n" \
		"C:multiplier:0.001\r\n" \
		"C:resolution:0.000001\r\n" \
		"C:base:10\r\n" \
		"H:name:mA raw\r\n" \
		"H:type:plain\r\n" \
		"H:mode:source\r\n" \
		"H:method:push\r\n" \
		"I:name:uA raw\r\n" \
		"I:type:plain\r\n" \
		"I:mode:source\r\n" \
		"I:method:push\r\n"

struct i2c_master i2c;
struct i2c_device dev1, dev2;
pthread_t adcs_thread;
float ma = 0, ua = 0, I = 0;
float avg_I = 0, peak_I = 0;
int32_t v1, v2;

int tcp_fd = -1;
int udp_fd = -1;
fd_set r_fds, w_fds;
int max_fd = -1;
int last_client = -1;

static void *adcs_thread_func(void *data)
{
	int64_t t = esp_timer_get_time() + 500L;
	int64_t s = t + 1000000L - 500L;
	double avg_s = 0.0;
	uint32_t avg_c = 0.0;

	/* let power etc stabilize before first measurement */
	os_sleepf(0.1);

	while (1) {
		v1 = mcp3221_read(&dev1, 0);
		v2 = mcp3221_read(&dev2, 0);
		ma = ((float)v1 - 2054.0) / 37.7646;
		ua = ((float)v2 - 2072.0) / 1.77466;

		if (ua > 750.0) {
			I = ma / 1000.0;
		} else {
			I = ua / 1000000.0;
		}

		if (peak_I < fabs(I) && t > 5000000L) {
			peak_I = fabs(I);
		}

		avg_s += fabs(I);
		avg_c++;
		avg_I = avg_s / (double)avg_c;

		while (t > esp_timer_get_time());
		t += 500L;
	}

	return NULL;
}


int net_init(void)
{
	int opt;
	struct sockaddr_in addr;

	/* wifi base init */
	ERROR_IF_R(wifi_init(), 1, "wifi initialization failed");

	/* setup for select */
	FD_ZERO(&r_fds);
	FD_ZERO(&w_fds);

	/* create and setup tcp socket */
	tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (tcp_fd < 0) {
		fprintf(stderr, "socket() failed\n");
		return -1;
	}
	FD_SET(tcp_fd, &r_fds);
	max_fd = tcp_fd > max_fd ? tcp_fd : max_fd;
	/* enable reuse */
	opt = 1;
	if (setsockopt(tcp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		fprintf(stderr, "setsockopt() failed\n");
		return -1;
	}

	/* bind */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(MULTIMETER_TCP_PORT);
	if (bind(tcp_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "bind() failed\n");
		return -1;
	}

	/* start to listen */
	if (listen(tcp_fd, 5) < 0) {
		fprintf(stderr, "listen() failed\n");
		return -1;
	}

	/* create and setup udp socket */
	udp_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (udp_fd < 0) {
		fprintf(stderr, "socket() failed\n");
		return -1;
	}
	FD_SET(udp_fd, &r_fds);
	max_fd = udp_fd > max_fd ? udp_fd : max_fd;
	/* set broadcast mode */
	opt = 1;
	if (setsockopt(udp_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt)) < 0) {
		fprintf(stderr, "setsockopt() failed\n");
		return -1;
	}
	/* enable reuse */
	opt = 1;
	if (setsockopt(udp_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		fprintf(stderr, "setsockopt() failed\n");
		return -1;
	}

	/* bind to specific port */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(MULTIMETER_UDP_PORT);
	if (bind(udp_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		fprintf(stderr, "bind() failed\n");
		return -1;
	}

	return 0;
}

int net_recv(int fd)
{
	int n;
	char line[1500];
	struct sockaddr_in addr;
	socklen_t l = sizeof(addr);

	/* if new connection */
	if (fd == tcp_fd) {
		n = accept(tcp_fd, (struct sockaddr *)&addr, &l);
		if (n >= 0) {
			FD_SET(n, &r_fds);
			max_fd = n > max_fd ? n : max_fd;
			return 0;
		}
		fprintf(stderr, "accept() failed\n");
		return -1;
	}

	/* if scanner query */
	if (fd == udp_fd) {
		n = recvfrom(udp_fd, line, sizeof(line), 0, (struct sockaddr *)&addr, &l);
		if (n < 1) {
			/* don't care */
			return 0;
		}
		if (strncmp("scan", line, n) == 0) {
			memset(&addr, 0, sizeof(addr));
			addr.sin_family = AF_INET;
			addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);
			addr.sin_port = htons(MULTIMETER_UDP_PORT);
			sprintf(line, "tcp:%d", MULTIMETER_TCP_PORT);
			sendto(udp_fd, line, strlen(line), 0, (struct sockaddr *)&addr, sizeof(addr));
			printf("discovery request replied\n");
		}
		return 0;
	}

	/* receive data */
	memset(line, 0, sizeof(line));
	while (1) {
		n = recv(fd, line + strlen(line), 1, 0);
		if (n <= 0) {
			close(fd);
			FD_CLR(fd, &r_fds);
			return 0;
		}
		if ((strlen(line) + 1) >= sizeof(line)) {
			break;
		}
		if (line[strlen(line) - 1] == '\n') {
			break;
		}
	}
	n = strlen(line);
	
	/* we got some data from a client, first trim spaces from end */
	for (char *p = line + n - 1; p >= line && (*p == ' ' || *p == '\r' || *p == '\n'); p--) {
		*p = '\0';
	}
	/* check action */
	if (strncmp("get config", line, n) == 0) {
		dprintf(fd, MULTIMETER_CONFIG);
		last_client = fd;
	} else if (strncmp("quit", line, n) == 0) {
		close(fd);
		FD_CLR(fd, &r_fds);
	} else if (n > 2) {
		if (line[1] != '=') {
			/* skip invalid sets */
		}
		// else if (line[0] == 'A') {
		// }
	}

	return 0;
}

int net_run(void)
{
	struct timeval tv;
	fd_set r = r_fds;
	fd_set w = w_fds;

	/* break out of select about 100 mS */
	tv.tv_sec = 0;
	tv.tv_usec = 200000;

	int err = select(max_fd + 1, &r, &w, NULL, &tv);
	if (err < 0) {
		return -1;
	} else if (err == 0) {
		/* send to client */
		if (last_client > 0) {
			dprintf(last_client, "A=%0.3f\nB=%0.3f\nC=%0.3f\nH=%x\nI=%x\n", I * 1000.0, avg_I * 1000.0, peak_I * 1000.0, v1, v2);
		}
	} else {
		for (int i = 0; i <= max_fd; i++) {
			/* check if read fd is set */
			if (FD_ISSET(i, &r)) {
				net_recv(i);
			}
			/* check if write fd is set */
			if (FD_ISSET(i, &w)) {
			}
		}
	}

	return 0;
}

int app_main(int argc, char *argv[])
{
	/* base init */
	os_init();
	log_init();
	nvm_init(NULL, 0);
	net_init();

	/* open i2c */
	ERROR_IF_R(i2c_master_open(&i2c, I2C_NUM_0, 400000, I2C_SCL, I2C_SDA), 1, "unable to open i2c device");
	/* enable internal pullups */
	i2c_set_pin(I2C_NUM_0, I2C_SDA, I2C_SCL, GPIO_PULLUP_ENABLE, GPIO_PULLUP_ENABLE, I2C_MODE_MASTER);

	/* open mcp3221 devices */
	ERROR_IF_R(mcp3221_open(&dev1, &i2c, MCP3221_ADDR_A2), 1, "unable to find mcp3221-a2");
	ERROR_IF_R(mcp3221_open(&dev2, &i2c, MCP3221_ADDR_A5), 1, "unable to find mcp3221-a5");

	/* create adc reader thread */
	pthread_create(&adcs_thread, NULL, adcs_thread_func, NULL);

	/* read mcp3221 devices */
	while (1) {
		// INFO_MSG("A2: %.3fV (%04d), A5: %.3fV (%04d)", (float)v1 * vref / 4096.0, (int)v1, (float)v2 * vref / 4096.0, (int)v2);
		// INFO_MSG("%2.3f mA, average %0.3f mA, peak %0.3f mA (%2.2f mA, %3.1f uA, %04u, %04u)", I * 1000.0, avg_I * 1000.0, peak_I * 1000.0, ma, ua, v1, v2);
		net_run();
		// os_sleepf(0.2);
	}

	/* quit */
	mcp3221_close(&dev1);
	mcp3221_close(&dev2);
	i2c_master_close(&i2c);
	// wifi_quit();
	nvm_quit();
	log_quit();
	os_quit();
	return 0;
}
