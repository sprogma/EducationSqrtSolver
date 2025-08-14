CC = gcc
LD = gcc

CFLAGS += -Wall -Wpedantic -fpic
LDFLAGS += -shared
LDLIBS += -lm

all : libsqsv.so

libsqsv.so : main.o
	$(LD) $^ $(LDFLAGS) -o $@ $(LDLIBS)

main.o : main.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f *.o libsqsv.o
