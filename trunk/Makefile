CC=gcc
#CFLAGS=-O3

INCLUDES=-I./src

CFLAGS=$(INCLUDES) -g -Wall
LDFLAGS=-lpthread -lbluetooth

TEST_OBJECTS=test/capture_test.o
APP=src/bdremoteng.o src/lirc_callback.o
GENERIC_OBJECTS=src/bdrcfg.o src/captureif.o src/lirc_srv.o
LINUX_OBJECTS=src/capture/capture_bluez.o
OBJECTS=$(TEST_OBJECTS) $(GENERIC_OBJECTS) $(LINUX_OBJECTS) $(APP)

all: $(OBJECTS) bdremoteng capturetest

bdremoteng: $(GENERIC_OBJECTS) $(LINUX_OBJECTS)
	$(CC) $(LDFLAGS) $(LINUX_OBJECTS) $(GENERIC_OBJECTS) $(APP) -o bdremoteng

capturetest: $(TEST_OBJECTS) $(GENERIC_OBJECTS) $(LINUX_OBJECTS)
	$(CC) $(LDFLAGS) $(GENERIC_OBJECTS) $(LINUX_OBJECTS) $(TEST_OBJECTS) -o capturetest

test/capture_test.o: test/capture_test.c
	$(CC) $(CFLAGS) -c test/capture_test.c -o test/capture_test.o

src/bdrcfg.o: src/bdrcfg.h src/bdrcfg.c
	$(CC) $(CFLAGS) -c src/bdrcfg.c -o src/bdrcfg.o

src/lirc_callback.o: src/lirc_srv.h src/lirc_callback.c 
	$(CC) $(CFLAGS) -c src/lirc_callback.c -o src/lirc_callback.o

src/lirc_srv.o: src/lirc_srv.h src/lirc_srv.c
	$(CC) $(CFLAGS) -c src/lirc_srv.c -o src/lirc_srv.o

src/captureif.o: src/captureif.h src/captureif.h
	$(CC) $(CFLAGS) -c src/captureif.c -o src/captureif.o

src/bdremoteng.o: src/bdremoteng.h src/bdremoteng.c src/globaldefs.h
	$(CC) $(CFLAGS) -c src/bdremoteng.c -o src/bdremoteng.o

src/capture/capture_bluez.o: src/capture/capture_bluez.h src/capture/capture_bluez.c
	$(CC) $(CFLAGS) -c src/capture/capture_bluez.c -o src/capture/capture_bluez.o

clean: 
	rm -f bdremoteng capturetest *.o
	rm -f src/*.o
	rm -f src/capture/*.o
	rm -f test/*.o
