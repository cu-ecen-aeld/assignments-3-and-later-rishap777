CC ?= $(CROSS_COMPILE)gcc

#CFLAGS ?= -g -Wall -Werror

TARGET ?= aesdsocket

LDFLAGS ?= -lpthread -lrt

all:aesdsocket
default:aesdsocket
aesdsocket:aesdsocket.c
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
	
clean:
	rm -f aesdsocket
	

