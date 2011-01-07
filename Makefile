CC = gcc -static
LIBS = -L/usr/lib -linotifytools

all: file-monitor

file-monitor::
	$(CC) $(INCLUDES) $(LIBS) fsmonitor.c -o fsmonitor /usr/lib/libinotifytools.a

clean:
	rm -f fsmonitor
