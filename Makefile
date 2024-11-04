WARNING = -Wall -Wshadow --pedantic 
ERROR = -Wvla -Werror
GCC = gcc -std=c99 -g $(WARNING) $(ERROR)

SRCS = a6.c
OBJS = $(SRCS:%.c=%.o)

a6: $(OBJS)
		$(GCC) $(OBJS) -o a6

.c.o:
		$(GCC) -c $*.c 

clean:
	rm -f a6 *.o output? *~