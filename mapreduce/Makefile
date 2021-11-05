CC = g++
CFLAGS  = -std=c++17 -I/home/hmei/rpclib/include -L/home/hmei/rpclib/build -lrpc -lpthread

all: 
	$(CC) cl.cc mrlib.cc $(CFLAGS) -o cl.bin
	$(CC) sv.cc mrlib.cc $(CFLAGS) -o sv.bin
	docker build -t haroldmei/mapreduce ./
	docker push haroldmei/mapreduce