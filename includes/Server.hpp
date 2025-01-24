/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:41:34 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/24 17:43:09 by hugolefevre      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <ctime>
#include <netdb.h>
#include <csignal>

#include "Client.hpp"
#include "Channel.hpp"

#define NICKNAME 0
#define USERNAME 1

#define MAX_CLIENT 15

extern bool serverRunning;

class Client;

struct FdMatcher {
	int fd;

	FdMatcher(int fd) : fd(fd) {}

	bool operator()(const pollfd &pfd) const {
		return pfd.fd == fd;
	}
};

struct clientState {
	bool authenticated;
	int attempts;
};

class Server
{
private:	
	int						_serverSocket;
	std::vector<pollfd> 	_pollfds;
	std::map<const int, Client>	_clients;
	std::string				_password;
	std::map<int, clientState> _clientStates;
	std::string		_datetime;

	// std::map<std::string, Channel*>	_channels;


public:
	Server(int port, const std::string &password, struct tm *timeinfo);
	~Server();

	void run();
	void shutDownServer();
	void setDatetime(struct tm *timeinfo);

	// Channel *getChannel(const std::string &name);
	std::map<const int, Client> &getClients();

	int	acceptNewClient(std::vector<pollfd> &pollfds, std::vector<pollfd> &newPollfds);
	int	readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it);

	void	addClient(int clientSocket, std::vector<pollfd> &pollfds);
	void	deleteClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it, int fd);

	// void	joinChannel(int fd, const std::string &name);
	// void	leaveChannel(int fd, const std::string &name);

	// void	displayChannels();
	// void addUserToChannel(int fd, const std::string &name);
	// void removeUserFromChannel(int fd, const std::string &name);
	// void sendMessageToChannel(const std::string &name, const std::string &message);
};

#endif