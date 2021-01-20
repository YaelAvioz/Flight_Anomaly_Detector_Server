/*
 * Server.cpp
 *
 * Author: Yael Avioz,207237421
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <ostream>
#include "Server.h"

#define SOCKET_ERROR "Couldn't open a new socket"
#define BIND_ERROR "Call to bind() failed"
#define LISTEN_ERROR "listen failure"
#define CONNECTION_ERROR "Connection Error"
#define CLIENTS_NUM 3

Server::Server(int port) throw(const char *)
{
	// Create a new TCP socket
	socket_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socket_fd < 0)
	{
		throw SOCKET_ERROR;
	}

	// Set sockaddr_in struct
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(port);

	// Assigns the socket to an IP address and a port
	if (bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
	{
		throw BIND_ERROR;
	}

	// Listen for new connections
	if (listen(socket_fd, CLIENTS_NUM) < 0)
	{
		throw LISTEN_ERROR;
	}
}

void Server::start(ClientHandler &ch) throw(const char *)
{
	// creat new thread
	alarm(5);
	t = new thread([&ch, this]() 
	{
		while (!stop_server)
		{
			socklen_t client_address_size = sizeof(client_address);
			client_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_size);
			
			// Check the connection
			if (client_fd < 0)
			{
				throw CONNECTION_ERROR;
			}
			ch.handle(client_fd);
			close(client_fd);
		}
		close(socket_fd);
	});
}

// Close the socket
void Server::stop()
{
	stop_server = true;
	t->join(); // do not delete this!
	// Free the memory
	delete t;
}

Server::~Server()
{
}
