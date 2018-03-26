# define variables
CC:g++
CFLAGS:-std=c++11 -c -Wall
CPPFLAGS:-I/usr/include/ -I/usr/include/libxml2/ -lcurl -lcurlpp -lpqxx


# build
all: scooper

scooper: main.o barchartutils.o Grabber.o db.o time_support.o
	$(CC) main.o barchartutils.o Grabber.o db.o time_support.o -o scooper

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

barcharutils.o: barchartUtils.cpp
	$(CC) $(CFLAGS) barchartUtils.cpp

grabber.o: Grabber.cpp
	$(CC) $(CFLAGS) Grabber.cpp

db.o: db.cpp
	$(CC) $(CFLAGS) db.cpp

time_support.o: time_support.cpp
	$(CC) $(CFLAGS) time_support.cpp

clean:
	rm -rf *.o scooper
