
TARGET = example-network
SRC = main.c

OBJ = $(SRC:.c=.o) $(ASRC:.S=.o) 

CC = gcc
RM = rm -f
CFLAGS = -Os -std=gnu99
LDFLAGS = 

all: build
build: $(TARGET)
clean:
	$(RM) *.o $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(OBJ) -o $@

%.o : %.c
	$(CC) -c $(CFLAGS) $< -o $@
