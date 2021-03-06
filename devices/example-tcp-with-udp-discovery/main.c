/*
 * Example network device.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#ifdef TARGET_ESP32
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/event_groups.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_wps.h>
#include <esp_event_loop.h>
#include <esp_log.h>
#define WIFI_MAXIMUM_RETRY      10
#define WIFI_CONNECTED_BIT      BIT0
int wifi_init(void);
#endif

#define MULTIMETER_TEST_CONFIG \
		"device:name:example-tcp (%d)\n" \
		"A:name:RGB\n" \
		"A:type:switch\n" \
		"A:mode:sink\n" \
		"A:method:push\n" \
		"B:name:Slider Red\n" \
		"B:type:slider\n" \
		"B:mode:sink\n" \
		"B:method:push\n" \
		"B:color:#000000,#ff0000\n" \
		"B:parent:A\n" \
		"C:name:Slider Green\n" \
		"C:type:slider\n" \
		"C:mode:sink\n" \
		"C:method:push\n" \
		"C:color:#000000,#00ff00\n" \
		"C:parent:A\n" \
		"D:name:Slider Blue\n" \
		"D:type:slider\n" \
		"D:mode:sink\n" \
		"D:method:push\n" \
		"D:color:#000000,#0000ff\n" \
		"D:parent:A\n" \
		"G:name:Wattage\n" \
		"G:type:wattage\n" \
		"G:mode:source\n" \
		"G:method:push\n" \
		"G:multiplier:0.001\n" \
		"G:resolution:0.05\n" \
		"G:parent:M\n" \
		"E:name:Voltage (mV)\n" \
		"E:type:voltage\n" \
		"E:mode:source\n" \
		"E:method:push\n" \
		"E:multiplier:0.001\n" \
		"E:resolution:0.001\n" \
		"E:parent:G\n" \
		"E:divider:0.001\n" \
		"F:name:Current\n" \
		"F:type:current\n" \
		"F:mode:source\n" \
		"F:method:push\n" \
		"F:multiplier:0.001\n" \
		"F:resolution:0.001\n" \
		"F:parent:G\n" \
		"H:name:Resistance\n" \
		"H:type:resistance\n" \
		"H:mode:source\n" \
		"H:method:push\n" \
		"H:base:10\n" \
		"H:resolution:5\n" \
		"H:parent:M\n" \
		"I:name:Temperature\n" \
		"I:type:temperature\n" \
		"I:mode:source\n" \
		"I:method:push\n" \
		"I:base:10\n" \
		"I:resolution:0.05\n" \
		"I:parent:L\n" \
		"J:name:Humidity\n" \
		"J:type:humidity\n" \
		"J:mode:source\n" \
		"J:method:push\n" \
		"J:base:10\n" \
		"J:resolution:0.05\n" \
		"J:parent:L\n" \
		"K:name:Random Frequency\n" \
		"K:type:frequency\n" \
		"K:mode:source\n" \
		"K:method:push\n" \
		"K:base:10\n" \
		"L:name:Environment\n" \
		"L:type:group\n" \
		"M:name:Power\n" \
		"M:type:group\n" \
		"N:name:1 MHz\n" \
		"N:type:frequency\n" \
		"N:mode:source\n" \
		"N:method:push\n" \
		"N:base:10\n" \
		"O:name:Datetime\n" \
		"O:type:datetime\n" \
		"O:mode:source\n" \
		"O:method:push\n" \
		"A=%u\nB=%x\nC=%x\nD=%x\n"

// "E=0\n"
// "F=0\n"
// "G=0\n"
// "H=0\n"
// "I=22.1\n"
// "J=65.2\n"
// "K=0\n"

#define MULTIMETER_UDP_PORT     17001

static int tcp_fd = -1;
static int tcp_port = 0;
static int udp_fd = -1;
static fd_set r_fds, w_fds;
static int max_fd = -1;

static double U = 3.3;
static double I = 0.150;

static int last_client = -1;

unsigned int sw = 0, R = 25, G = 50, B = 75;

int p_init(void)
{
	int opt;
	struct sockaddr_in addr;
	socklen_t size;

#ifdef TARGET_ESP32
	/* initialize networking = wifi when using esp32 */
	wifi_init();
#endif

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

	/* bind to random port */
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(tcp_port);
	if (bind(tcp_fd, &addr, sizeof(addr)) < 0) {
		fprintf(stderr, "bind() failed\n");
		return -1;
	}

	/* start to listen */
	if (listen(tcp_fd, 5) < 0) {
		fprintf(stderr, "listen() failed\n");
		return -1;
	}

	/* get port which was selected randomly, needed later for clients */
	size = sizeof(addr);
	getsockname(tcp_fd, &addr, &size);
	tcp_port = ntohs(addr.sin_port);
	printf("using random port: %d\n", tcp_port);

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
	if (bind(udp_fd, &addr, sizeof(addr)) < 0) {
		fprintf(stderr, "bind() failed\n");
		return -1;
	}

	return 0;
}

void p_quit(int retval)
{
	close(tcp_fd);
	exit(retval);
}

int p_recv(int fd)
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
			sprintf(line, "tcp:%d", tcp_port);
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
		dprintf(fd, MULTIMETER_TEST_CONFIG, tcp_port, sw, R, G, B);
		last_client = fd;
	} else if (strncmp("quit", line, n) == 0) {
		close(fd);
		FD_CLR(fd, &r_fds);
	} else if (n > 2) {
		if (line[1] != '=') {
			/* skip invalid sets */
		} else if (line[0] == 'A') {
			sw = line[2] != '0';
			printf("Switch is now %s\n", sw ? "on" :  "off");
		} else if (line[0] == 'B') {
			R = strtol(line + 2, NULL, 16);
			printf("Red slider to %u (%s)\n", R, line + 2);
		} else if (line[0] == 'C') {
			G = strtol(line + 2, NULL, 16);
			printf("Green slider to %u (%s)\n", R, line + 2);
		} else if (line[0] == 'D') {
			B = strtol(line + 2, NULL, 16);
			printf("Blue slider to %u (%s)\n", B, line + 2);
		}
	}

	return 0;
}

void p_run(void)
{
	int err, i;

	while (1) {
		struct timeval tv;
		fd_set r = r_fds;
		fd_set w = w_fds;

		/* break out of select about every 1 seconds if nothing has happened */
		tv.tv_sec = 0;
		tv.tv_usec = 333000;

		err = select(max_fd + 1, &r, &w, NULL, &tv);
		if (err < 0) {
			fprintf(stderr, "select() failed, reason: %s", strerror(errno));
			break;
		} else if (err == 0) {
			/* temperature and humidity */
			static float temperature = -15, humidity = 30;
			temperature = temperature > 15 ? temperature - 32 : temperature + 0.7512;
			humidity = humidity > 80 ? humidity - 52 : humidity + 1.498;

			/* voltage and current */
			U += ((double)(rand() % 100) - 50.0) / 10000.0;
			I += ((double)(rand() % 100) - 50.0) / 10000.0;
			U = U < 0.0 ? 0.0 : U;
			I = I < 0.0 ? 0.0 : I;

			/* datetime */
			time_t now;
			char datetime[128];
			time(&now);
			strftime(datetime, sizeof(datetime), "%FT%T", localtime(&now));

			/* send to client */
			if (last_client > 0) {
				dprintf(last_client, "E=%04x\nF=%04x\nG=%04x\nH=%lf\nI=%f\nJ=%f\nK=%u\nN=%u\nO=%s\n",
				        (unsigned int)(U * 1000.0), /* E */
				        (unsigned int)(I * 1000.0), /* F */
				        (unsigned int)(U * I * 1000.0), /* G */
				        U / I, /* H */
				        temperature, /* I */
				        humidity, /* J */
				        rand() % 1000000000, /* K */
				        1000000, /* N */
						datetime /* O */
				       );
			}

			printf("U: %.3f, I: %.3f\n", U, I);
			continue;
		}

		for (i = 0; i <= max_fd; i++) {
			/* check if read fd is set */
			if (FD_ISSET(i, &r)) {
				p_recv(i);
			}
			/* check if write fd is set */
			if (FD_ISSET(i, &w)) {
			}
		}
	}
}

#ifdef TARGET_ESP32
int app_main(int argc, char*argv[])
#else
int main(int argc, char*argv[])
#endif
{
	if (p_init()) {
		return EXIT_FAILURE;
	}

	while (1) {
		p_run();
	}

	p_quit(EXIT_SUCCESS);
	return EXIT_SUCCESS;
}


/* esp32 wifi with wps */
#ifdef TARGET_ESP32

static EventGroupHandle_t wifi_event_group;
static esp_wps_config_t wifi_wps_config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);
#define ERROR_IF_R(condition, ret, msg) do { if (condition) { ESP_LOGE("wifi", "%s", msg); return ret; } } while(0)

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{
	static int wifi_retries = 0;

	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		if (esp_wifi_connect()) {
			ERROR_IF_R(esp_wifi_wps_enable(&wifi_wps_config), ESP_FAIL, "wifi wps enable failed");
			ERROR_IF_R(esp_wifi_wps_start(0), ESP_FAIL, "wifi wps start failed");
			ESP_LOGI("wifi", "wifi wps setup started");
		} else {
			ESP_LOGI("wifi", "wifi is connecting");
		}
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		ESP_LOGI("wifi", "wifi got ip: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
		ERROR_IF_R(esp_wifi_wps_disable(), ESP_FAIL, "wifi wps disable failed");
		wifi_retries = 0;
		xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		if (wifi_retries < WIFI_MAXIMUM_RETRY) {
			esp_wifi_connect();
			xEventGroupClearBits(wifi_event_group, WIFI_CONNECTED_BIT);
			wifi_retries++;
			ESP_LOGW("wifi", "wifi retry connect to the AP");
		} else {
			ERROR_IF_R(esp_wifi_wps_disable(), ESP_FAIL, "wifi wps disable after disconnect failed");
			ERROR_IF_R(esp_wifi_wps_enable(&wifi_wps_config), ESP_FAIL, "wifi wps enable after disconnect failed");
			ERROR_IF_R(esp_wifi_wps_start(0), ESP_FAIL, "wifi wps start after disconnect failed");
			wifi_retries = 0;
		}
		ESP_LOGW("wifi", "wifi connect to the AP fail");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
		/*point: the function esp_wifi_wps_start() only get ssid & password
		 * so call the function esp_wifi_connect() here
		 * */
		ESP_LOGI("wifi", "SYSTEM_EVENT_STA_WPS_ER_SUCCESS");
		ERROR_IF_R(esp_wifi_wps_disable(), ESP_FAIL, "wifi wps disable failed");
		ERROR_IF_R(esp_wifi_connect(), ESP_FAIL, "wifi connect after wps setup failed");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_FAILED:
		ESP_LOGI("wifi", "SYSTEM_EVENT_STA_WPS_ER_FAILED");
		ERROR_IF_R(esp_wifi_wps_disable(), ESP_FAIL, "wifi wps disable after failed wps attempt failed");
		ERROR_IF_R(esp_wifi_wps_enable(&wifi_wps_config), ESP_FAIL, "wifi wps enable after failed wps attempt failed");
		ERROR_IF_R(esp_wifi_wps_start(0), ESP_FAIL, "wifi wps start after failed wps attempt failed");
		break;
	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
		ESP_LOGI("wifi", "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT");
		ERROR_IF_R(esp_wifi_wps_disable(), ESP_FAIL, "wifi wps disable after wps timeout failed");
		ERROR_IF_R(esp_wifi_wps_enable(&wifi_wps_config), ESP_FAIL, "wifi wps enable after wps timeout failed");
		ERROR_IF_R(esp_wifi_wps_start(0), ESP_FAIL, "wifi wps start after wps timeout failed");
		break;
	default:
		break;
	}

	return ESP_OK;
}

int wifi_init(void)
{
	wifi_event_group = xEventGroupCreate();

	tcpip_adapter_init();
	ERROR_IF_R(esp_event_loop_init(wifi_event_handler, NULL), -1, "wifi event loop setup failed");

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	ERROR_IF_R(esp_wifi_init(&cfg), -1, "wifi config has errors");

	ERROR_IF_R(esp_wifi_set_mode(WIFI_MODE_STA), -1, "unable to set wifi mode to WIFI_MODE_STA");
	ERROR_IF_R(esp_wifi_start(), -1, "unable to start wifi");

	return 0;
}

#endif

