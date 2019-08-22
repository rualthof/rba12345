install:
	apt-get install libczmq-dev

clean:
	rm -rf logger & rm -rf client & rm -rf server
	rm -rf log.txt

compile:
	gcc server.c -lczmq -lzmq -lpthread -o server
	gcc logger.c -lczmq -lzmq -o logger
	gcc client.c -lczmq -lzmq -lpthread -o client
