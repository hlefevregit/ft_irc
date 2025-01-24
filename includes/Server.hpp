/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:41:34 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/23 17:32:59 by hulefevr         ###   ########.fr       */
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

#include "Client.hpp"
#include "Channel.hpp"

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
	std::map<int, Client*>	_clients;
	std::string				_password;
	std::map<int, clientState> _clientStates;

	std::map<std::string, Channel*>	_channels;

	void _acceptNewClient(int clientSocket);
	bool _readFromClient(int fd);
	void _removeClient(int fd);
	void authenticateClient(int fd, const std::string &message);

public:
	Server(int port, const std::string &password);
	~Server();

	void run();
	void shutDownServer();

	void createChannel(const std::string &name);
	void deleteChannel(const std::string &name);
	Channel *getChannel(const std::string &name);
	Client *getClient(int fd);

	void	joinChannel(int fd, const std::string &name);
	void	leaveChannel(int fd, const std::string &name);

	void	displayChannels();
	// void addUserToChannel(int fd, const std::string &name);
	// void removeUserFromChannel(int fd, const std::string &name);
	// void sendMessageToChannel(const std::string &name, const std::string &message);
};

#endif