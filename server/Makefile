CC ?= $(CROSS_COMPILE)gcc

#CFLAGS ?= -g -Wall -Werror

#TARGET ?= aesdsocket

LDFLAGS ?= -pthread -lrt



all:
	$(CC) aesdsocket.c -o aesdsocket $(LDFLAGS)
	
default:
	$(CC) aesdsocket.c -o aesdsocket $(LDFLAGS)
	
clean:
	rm -f aesdsocket
	

