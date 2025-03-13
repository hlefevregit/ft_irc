/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:46:11 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/13 17:16:01 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
#include <string>
#include <map>
#include <fstream>

void	sendChillguy(int clientSocket)
{
	std::ifstream file;
	char		filePath[22] = "./src/chillGuy.config";
	
	file.open(filePath);
	if (!file.is_open())
	{
		std::cerr << "\033[31m[ERROR]\033[0m Failed to open file" << std::endl;
		return ;
	}
	std::string line;
	while (std::getline(file, line))
	{
		send(clientSocket, line.c_str(), line.size(), 0);
		send(clientSocket, "\n", 1, 0);
	}
	file.close();
}

/*********************************************************************/
/*********************************************************************/
/****************************SERV HANDLER*****************************/
/*********************************************************************/
/*********************************************************************/


Server::Server(int port, const std::string &password, struct tm *timeinfo) : _password(password) {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket == -1)
		throw std::runtime_error("Error: Failed to create socket");

	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);
	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Error: Failed to bind socket");
	if (listen(_serverSocket, MAX_CLIENT) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	
	pollfd pfd;
	pfd.fd = _serverSocket;
	pfd.events = POLLIN;
	_pollfds.push_back(pfd);

	setDatetime(timeinfo);

	std::cout << "Server started on port " << port << std::endl;
}

Server::~Server() {
	shutDownServer();
}

void	Server::setDatetime(struct tm *timeinfo)
{
	char buffer[80];

	strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
  	std::string str(buffer);

	_datetime = str;
}

void Server::shutDownServer() {
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		close(it->fd);
	}
	shutdown(_serverSocket, SHUT_RDWR);
	close(_serverSocket);
	std::cout << "Server shut down" << std::endl;
}

void Server::addClient(int clientSocket, std::vector<pollfd> &pollfds) {
	pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN;
	Client newClient(clientSocket);
	
	_clients.insert(std::pair<int, Client>(clientSocket, newClient));
	std::cout << "\033[32m[INFO]\033[0m New client added #" << clientSocket << std::endl;

	std::string msg = "Welcome to the server, please enter the password: ";
	send(clientSocket, msg.c_str(), msg.size(), 0);
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
	if (bytesRead < 0) {
		std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
	} else if (bytesRead == 0) {
		std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
	} else {
		std::string cleanPassword = messageCleaner(buffer);
		if (cleanPassword != _password) {
			std::string msg = "Wrong password\n";
			send(clientSocket, msg.c_str(), msg.size(), 0);
			std::cout << "\033[32m[INFO]\033[0m Wrong password, client disconnected" << std::endl;
			close(clientSocket);
		} else {
			std::string msg = "Password accepted\n";
			send(clientSocket, msg.c_str(), msg.size(), 0);
			sendChillguy(clientSocket);
			pollfds.push_back(pfd);
			// _pollfds.push_back(pfd);
		}
	}
}

void Server::deleteClient(std::vector<pollfd> &pollfds, const std::vector<pollfd>::iterator &it, int fd) {
	std::cout << "\033[32m[INFO]\033[0m Deconnecting client #" << fd << std::endl;
	close(fd);
	_clients.erase(fd);
	pollfds.erase(it);
	std::cout << "\033[32m[INFO]\033[0m Total client: " << _clients.size() << std::endl;
}

int Server::acceptNewClient(std::vector<pollfd> &pollfds, std::vector<pollfd> &newPollfds) {
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket < 0) {
		if (errno == EINTR) {
			std::cout << GREEN << "[INFO]" << RESET << " Signal received" << std::endl;
			return 1;
		}
		std::cerr << "\033[31m[ERROR]\033[0m Failed to accept client" << std::endl;
		return 2;
	}
	
	if (pollfds.size() >= MAX_CLIENT) {
		std::cerr << "\033[31m[ERROR]\033[0m Server is full" << std::endl;
		std::string msg = "Server is full\n";
		send(clientSocket, msg.c_str(), msg.size(), 0);
		close(clientSocket);
	} else {
		addClient(clientSocket, newPollfds);
		std::cout << "\033[32m[INFO]\033[0m New client connected" << std::endl;
	}
	return 0;
	
}

std::map<const int, Client> &Server::getClients() { return (_clients); }

Client *getClient(Server *server, int fd) {
	std::map<const int, Client> &clients = server->getClients();
	std::map<const int, Client>::iterator it = clients.find(fd);
	
	if (it == clients.end()) {
		std::cerr << "\033[31m[ERROR]\033[0m Client not found" << std::endl;
		return NULL;
	}
	return &it->second;
}

int	Server::readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it) {
	
	Client *client;
	client = getClient(this, it->fd);
	
	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(it->fd, buffer, sizeof(buffer), 0);

	if (bytesRead < 0) {
		std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
		deleteClient(pollfds, it, it->fd);
		return 3;
	} else if (bytesRead == 0) {
		std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
		deleteClient(pollfds, it, it->fd);
		return 3;
	} else {

		std::cout << "\033[32m[INFO]\033[0m Received " << bytesRead << " bytes from client" << std::endl;
		client->setReadBuffer(buffer);
		if (bytesRead == 1)
			client->setReadBuffer("");
		else {
			while (!client->getReadBuffer().empty() && 
				(client->getReadBuffer()[client->getReadBuffer().size() - 1] == '\n' || 
				client->getReadBuffer()[client->getReadBuffer().size() - 1] == '\r')) 
			{
				client->setReadBuffer(client->getReadBuffer().erase(client->getReadBuffer().size() - 1, 1));
			}
		}
		std::cout << "\033[32m[INFO]\033[0m Message: " << buffer << std::endl;
		try {
			this->parseMessage(client, client->getReadBuffer());
			if (client->getReadBuffer().find("\r\n"))
				client->resetBuffer();
		} catch (const std::exception &e) {
			std::cerr << "\033[31m[ERROR]\033[0m " << e.what() << std::endl;
			return 3;
		}
		std::cout << YELLOW << "[DEBUG]" << RESET << " Message parsed" << std::endl;
	}
		
	
	return 0;
}


void Server::run() {
	
	std::vector<pollfd> pollfds;
	pollfd serverPfd;
	serverPfd.fd = _serverSocket;
	serverPfd.events = POLLIN;

	pollfds.push_back(serverPfd);


	while (serverRunning == 1) {
		std::cout << RED << "[INFO]" << RESET << " Server running = " << serverRunning << std::endl;
		std::vector<pollfd> newPollfds;
		
		if (poll((pollfd *)&pollfds[0], (unsigned int)pollfds.size(), -1) < 0) {
			if (errno == EINTR) {
				std::cout << GREEN << "[INFO]" << RESET << " Signal received" << std::endl;
				continue ;
			}
			std::cerr << "\033[31m[ERROR]\033[0m Failed to poll" << std::endl;
			break;
		}
		
		std::vector<pollfd>::iterator it = pollfds.begin();
		while (it != pollfds.end()) {
			if (it->revents & POLLIN)
			{
				if (it->fd == _serverSocket)
				{
					if (this->acceptNewClient(pollfds, newPollfds) == 2)
						continue ;
				}
				else
				{
					if (this->readFromClient(pollfds, it) == 3)
						break ;
				}
			}
			it++;
		}
		pollfds.insert(pollfds.end(), newPollfds.begin(), newPollfds.end());
	}
	return ;
}

/*********************************************************************/
/*********************************************************************/
/*****************************CHANNEL HANDLER*************************/
/*********************************************************************/
/*********************************************************************/


void Server::addClientToChannel(Client client, const std::string &channelName)
{
	std::map<std::string, Channel>::iterator it = _channels.find(channelName);
	std::string clientNick = client.getNickname();
	if (it != _channels.end())
	{
		it->second.addUserToChannel(client);
		std::cout << "\033[32m[INFO]\033[0m " << clientNick << " joined channel " << channelName << std::endl;
		std::string msg = "You have successfully joined channel " + channelName + "\n";
		send(client.getFd(), msg.c_str(), msg.size(), 0);
	}
	

}

void	Server::sendAllUsers(const std::string &msg, const std::string &nickname)
{
	std::map<const int, Client>::iterator it = _clients.begin();
	while (it != _clients.end())
	{
		std::string message = nickname + ": " + msg + "\n";
		send(it->first, message.c_str(), message.size(), 0);
		it++;
	}
}

// std::map<const int, Client>::iterator	Server::getClientByNickname(const std::string &nickname)
// {
// 	std::map<const int, Client>::iterator	start = _client.begin();
// 	std::map<const int, Client>::iterator	end = _client.end();

// 	// Check if nickname correspond to a Client's nickname
// 	while (start != end)
// 	{
// 		if (start->second.getNickname() == nickname)
// 			return (start);
// 		++start;
// 	}
// 	std::cerr << ERROR << "getClientByNickname: nickname not found amongs clients !" << RESET << std::endl;
// 	return (start);
// }

// void	Server::sendMessageUser(const std::string &msg, const std::string &nickname)
// {
// 	std::map<const int, Client>::iterator	reciever = getClientByNickname(nickname);
// 	std::map<const int, Client>::iterator	end = _client.end();

// 	if (reciever->first == end->first)
// 	{
// 		std::cerr << ERROR << "sendMessageUser: client does not exist !" << RESET << std::endl;
// 		return ;
// 	}

// 	// Sends message to user
// 	std::string message = "PM: " + "Sender" + ": " + msg + "\n";
// 	send(reciever->first, message.c_str(), message.size(), 0);

// 	return ;
// }