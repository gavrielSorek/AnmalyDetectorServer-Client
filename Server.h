/*
 * server.h
 *
 * Author: 318525185, Gavriel Sorek
 */

#ifndef SERVER_H_
#define SERVER_H_
#include <stdio.h>
#include <sys/socket.h>
#include <thread>
#include <pthread.h>
#include <netinet/in.h>
#include <mutex>
#include "CLI.h" //for the client handler
using namespace std;

// edit your ClientHandler interface here:
class ClientHandler{
    public:
    virtual void handle(int clientID)=0;
};


// you can add helper classes here and implement on the cpp file


// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler:public ClientHandler{
	public:
    virtual void handle(int clientID){
        SocketIO *socketIO = new SocketIO(clientID);
        CLI cli(socketIO);
        cli.start();
        delete socketIO;
    }
};


// implement on Server.cpp
class Server {
	thread* t; // the thread to run the start() method in
	int fd;
	sockaddr_in serverInfo;
	sockaddr_in clientInfo;
	volatile bool isStop;
	mutex m; //only one thread every time
	// you may add data members

public:
	Server(int port) throw (const char*);
	virtual ~Server();
	void start(ClientHandler& ch)throw(const char*);
	void stop();
};

#endif /* SERVER_H_ */
