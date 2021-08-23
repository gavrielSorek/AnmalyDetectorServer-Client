/*
 * server.cpp
 *
 * Author: 318525185, Gavriel Sorek
 */
#include <unistd.h>
#include "Server.h"


Server::Server(int port) throw(const char *) {
    this->fd = socket(AF_INET, SOCK_STREAM, 0);//update the file descriptor
    if (fd < 0) //if error
        throw "error, cant open socket";
    serverInfo.sin_addr.s_addr = INADDR_ANY; //local host
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(port);

    if (bind(fd, (struct sockaddr *) &serverInfo, sizeof(serverInfo)) < 0)
        throw "bind error";
    if (listen(fd, 30) < 0)
        throw "listen failed";
    isStop = false;
}

void Server::start(ClientHandler &ch) throw(const char *) {

    t = new thread([&ch, this]() { //thread that handle customers
        socklen_t socketSize = sizeof(clientInfo);
        while (!isStop) { //handle clients
            //set time yo wain to client
            struct timeval tv;
            tv.tv_sec = 1;
            tv.tv_usec = 0;
            fd_set rfds;
            FD_ZERO(&rfds);
            FD_SET(fd, &rfds);
            int retVal = select(fd + 1, &rfds, NULL, NULL, &tv);
            if (retVal == 0) //if timeout
                continue;
            int clientFD = accept(fd, (struct sockaddr *) &clientInfo, &socketSize);
            m.lock(); //lock to be careful from changes in the same time
            if (clientFD < 0)
                throw "accept failed";
            ch.handle(clientFD);
            close(clientFD);
            m.unlock();
        }
    });

}

void Server::stop() {
    isStop = true;
    t->join(); // do not delete this!
}

Server::~Server() {
    close(fd);
    delete t; //delete the thread
}

