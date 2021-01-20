/*
 * Server.h
 *
 * Author: Yael Avioz,207237421
 */

#ifndef SERVER_H_
#define SERVER_H_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <thread>
#include <unistd.h>

#include "commands.h"
#include "CLI.h"

using namespace std;

// edit your ClientHandler interface here:
class ClientHandler
{
public:
	virtual void handle(int clientID) = 0;
};

// edit your AnomalyDetectionHandler class here
class AnomalyDetectionHandler : public ClientHandler
{
protected:
	DefaultIO *dio;

public:
	virtual void handle(int clientID)
	{
		this->dio = new SocketIO(clientID);
		CLI cli(dio);
		cli.start();
	}
};

// implement on Server.cpp
class Server
{
protected:
	thread *t; // the thread to run the start() method in
	int socket_fd;
	int port;
	int client_fd;
	struct sockaddr_in server_address;
	struct sockaddr_in client_address;
	bool stop_server = false;

public:
	Server(int port) throw(const char *);
	virtual ~Server();
	void start(ClientHandler &ch) throw(const char *);
	void stop();
};

#endif /* SERVER_H_ */