/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:46:11 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/23 17:40:12 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
#include <string>


/*********************************************************************/
/*********************************************************************/
/****************************SERV HANDLER*****************************/
/*********************************************************************/
/*********************************************************************/


Server::Server(int port, const std::string &password) : _password(password) {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw std::runtime_error("Error: Failed to create socket");

	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Error: Failed to bind socket");
	if (listen(_serverSocket, 5) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	
	pollfd pfd;
	pfd.fd = _serverSocket;
	pfd.events = POLLIN;
	_pollfds.push_back(pfd);

	std::cout << "Server started on port " << port << std::endl;
}

Server::~Server() {
	shutDownServer();
}

void Server::shutDownServer() {
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		close(it->fd);
	}
	close(_serverSocket);
	std::cout << "Server shut down" << std::endl;
}

void Server::_acceptNewClient(int clientSocket) {
	
	std::cout << "\033[32m[INFO]\033[0m New client connected: " << clientSocket << std::endl;
	
	clientState state;
	state.authenticated = false;
	state.attempts = 3;
	_clientStates[clientSocket] = state;
	
	pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN;
	_pollfds.push_back(pfd);

	_clients[clientSocket] = new Client(clientSocket);
	send(clientSocket, "Welcome ! Please enter the password\n", 36, 0);
	std::cout << "New client connected: " << clientSocket << std::endl;
}

void Server::authenticateClient(int fd, const std::string &message) {
	if (_clientStates.find(fd) == _clientStates.end()) {
		std::cerr << "\033[31m[ERROR]\033[0m Client (" << fd << ") not found in clientStates" << std::endl;
		return ;
	}

	if (message == _password) {
		_clientStates[fd].authenticated = true;
		_clients[fd]->authenticate();
		send(fd, "Authenticated\n", 14, 0);
		std::cout << "\033[31m[INFO]\033[0m Client (" << fd << ") authenticated" << std::endl;
	} else {
		_clientStates[fd].attempts--;
		if (_clientStates[fd].attempts <= 0) {
			send(fd, "Too many attempts. Disconecting ...\n", 35, 0);
			std::cout << "\033[31m[INFO]\033[0m Client (" << fd << ") disconnected: Failed to authentificate" << std::endl;
			_removeClient(fd);
		} else {
			std::ostringstream oss;
			oss << "Invalid password. " << _clientStates[fd].attempts << " attempts left.\n";
			std::string errorMsg = oss.str();
			send(fd, errorMsg.c_str(), errorMsg.size(), 0);
		}
	}
}

bool Server::_readFromClient(int fd) {
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(fd, buffer, sizeof(buffer) - 1, 0);

	if (_clientStates.find(fd) == _clientStates.end()) {
		std::cerr << "\033[31m[ERROR]\033[0m Client (" << fd << ") not found in clientStates" << std::endl;
		return false;
	}

	if (bytesRead <= 0) {
		if (bytesRead < 0)
			std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client " << fd << std::endl;
		else
			std::cerr << "\033[31m[INFO]\033[0m Client (" << fd << ") disconnected" << std::endl;
		_removeClient(fd);
		return false;
	}
	

	std::string message(buffer);
	std::string cleanMessage(message);

	std::cout << "\033[31m[DATA]\033[0m Received from client (" << fd << "): " << cleanMessage << std::endl;
	
	while (!cleanMessage.empty() && (cleanMessage[cleanMessage.size() - 1] == '\n' || cleanMessage[cleanMessage.size() - 1] == '\r'))
		cleanMessage.erase(cleanMessage.size() - 1);
	
	std::cout << "\033[31m[DEBUG]\033[0m _clientds[fd]->isAuthenticated = " << _clients[fd]->isAuthenticated() << std::endl;
	if (!_clients[fd]->isAuthenticated()) {
		authenticateClient(fd, cleanMessage);
	} else {
		std::cout << "\033[34m[CLIENT " << fd << "]\033[0m authenticated and sent: " << buffer << std::endl;
	}
	return true;
}

void Server::_removeClient(int fd) {
	close(fd);
	_pollfds.erase(std::remove_if(_pollfds.begin(), _pollfds.end(), FdMatcher(fd)), _pollfds.end());
	_clientStates.erase(fd);
	// delete _clients[fd];
	std::cout << "\033[31m[INFO]\033[0m Client (" << fd << ") removed from the server" << std::endl;
}

void Server::run() {
	std::cout << "\033[32m[INFO]\033[0m Server is running" << std::endl;
	while (1) {
		int pollCount = poll(&_pollfds[0], _pollfds.size(), -1);
		if (pollCount < 0) {
			std::cerr << "\033[31m[ERROR]\033[0m Failed to poll" << std::endl;
			continue;
		}
		for (size_t i = 0; i < _pollfds.size(); ++i) {
			if (_pollfds[i].revents & POLLIN) {
				if (_pollfds[i].fd == _serverSocket) {
					int clientFd = accept(_serverSocket, NULL, NULL);
					if (clientFd < 0) {
						std::cerr << "\033[31m[ERROR]\033[0m Failed to accept client" << std::endl;
					} else {
						_acceptNewClient(clientFd);
					}
				}
				else if (_pollfds[i].fd >= 0) {
					if (!_readFromClient(_pollfds[i].fd)) {
						_removeClient(_pollfds[i].fd);
						i--;
					}
				}
			}
			std::string buf;
			std::getline(std::cin, buf);
			if (buf == "exit") {
				break;
			}
		}
	}
}

/*********************************************************************/
/*********************************************************************/
/*****************************CHANNEL HANDLER*************************/
/*********************************************************************/
/*********************************************************************/

void Server::createChannel(const std::string &name) {
	if (_channels.find(name) == _channels.end()) {
		_channels[name] = new Channel(name);
		std::cout << "\033[32m[INFO]\033[0m Channel " << name << " created" << std::endl;
	} else {
		std::cout << "\033[31m[ERROR]\033[0m Channel " << name << " already exists" << std::endl;
	}
}

void Server::deleteChannel(const std::string &name) {
	std::map <std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end()) {
		delete it->second;
		_channels.erase(it);
		std::cout << "\033[32m[INFO]\033[0m Channel " << name << " deleted" << std::endl;
	} else {
		std::cout << "\033[31m[ERROR]\033[0m Channel " << name << " not found" << std::endl;
	}
}

Channel *Server::getChannel(const std::string &name) {
	std::map <std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end()) {
		return it->second;
	}
	return NULL;
}

Client *Server::getClient(int fd) {
	std::map <int, Client*>::iterator it = _clients.find(fd);
	if (it != _clients.end()) {
		return it->second;
	}
	return NULL;
}

void Server::joinChannel(int fd, const std::string &name) {
	Channel *channel = getChannel(name);
	if (channel == NULL) {
		std::cout << "\033[31m[ERROR]\033[0m Channel " << name << " not found" << std::endl;
		return ;
	}
	Client *client = getClient(fd);
	if (client == NULL) {
		std::cout << "\033[31m[ERROR]\033[0m Client " << fd << " not found" << std::endl;
		return ;
	}

	if (client && channel) {
		channel->addUser(client);
		std::cout << "\033[32m[INFO]\033[0m Client " << fd << " joined channel " << name << std::endl;
		channel->broadcast("Client " + client->getNickname() + " joined the channel\n", fd);
	}
}

void Server::leaveChannel(int fd, const std::string &name) {
	Channel *channel = getChannel(name);
	if (channel == NULL) {
		std::cout << "\033[31m[ERROR]\033[0m Channel " << name << " not found" << std::endl;
		return ;
	}
	Client *client = getClient(fd);
	if (client == NULL) {
		std::cout << "\033[31m[ERROR]\033[0m Client " << fd << " not found" << std::endl;
		return ;
	}

	if (client && channel) {
		channel->removeUser(fd);
		std::cout << "\033[32m[INFO]\033[0m Client " << fd << " left channel " << name << std::endl;
		channel->broadcast("Client " + client->getNickname() + " left the channel\n", fd);
		
		if (!channel->hasUser(fd)) {
			deleteChannel(name);
		}
	}
}

void Server::displayChannels() {
	std::cout << "\033[32m[INFO]\033[0m Channels:" << std::endl;
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it) {
		std::cout << " - " << it->first << std::endl;
	}
}