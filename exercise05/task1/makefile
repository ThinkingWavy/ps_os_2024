CFLAGS = -Wall -Wextra -std=c11 -O3 -lrt -D_DEFAULT_SOURCE -D_POSIX_SOURCE -g

.PHONY:  all clean

all: scheduler service

scheduler: scheduler.c

service: service.c

clean:
	$(RM) *.o scheduler service