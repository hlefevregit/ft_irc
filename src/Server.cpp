/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:46:11 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/28 19:15:48 by hulefevr         ###   ########.fr       */
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

void Server::addClient(int clientSocket, std::vector<pollfd> &pollfds)
{
	Client		newClient(clientSocket);
	std::string	message;

	pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN;

	/* Add new client to std::map<const int, Client> */
	std::cout << INFO << "New unregistered client added #" << clientSocket << std::endl;
	_clients.insert(std::pair<int, Client>(clientSocket, newClient));
	pollfds.push_back(pfd);

	return ;
}

void Server::deleteClient(std::vector<pollfd> &pollfds, int fd) {
	AUTO_LOG
	LOG(INFO "Deconnecting client #" << fd)

	// 1. Fermer le socket
	close(fd);

	// 2. Supprimer le client du map
	if (_clients.find(fd) != _clients.end())
		_clients.erase(fd);

	// 3. Trouver l'itérateur pollfd et le supprimer
	std::vector<pollfd>::iterator it = findPollfdIterator(fd, pollfds);
	if (it != pollfds.end())
		pollfds.erase(it);
	else
		{ LOG(ERROR "pollfd for fd " << fd << " not found.") }

	LOG(INFO "Total clients: " << _clients.size())
}


int Server::acceptNewClient(std::vector<pollfd> &pollfds, std::vector<pollfd> &newPollfds)
{
	AUTO_LOG
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket < 0) {
		if (errno == EINTR) {
			LOG(INFO "Signal received")
			return 1;
		}
		LOG(ERROR "Failed to accept client")
		return 2;
	}
	
	if (pollfds.size() >= MAX_CLIENT) {
		LOG(ERROR "Server is full")
		std::string msg = "Server is full\n";
		send(clientSocket, msg.c_str(), msg.size(), 0);
		close(clientSocket);
	} else {
		addClient(clientSocket, newPollfds);
		LOG(INFO "New client connected #" << clientSocket)
		sendChillguy(clientSocket);}
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

int Server::readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it) {
	int fd = it->fd;
	Client* client = getClient(this, fd);
	if (!client) {
		std::cerr << RED << "[ERROR] Client not found for fd " << fd << RESET << std::endl;
		deleteClient(pollfds, fd);
		return 3;
	}

	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesRead <= 0) {
		if (bytesRead == 0)
			std::cout << GREEN << "[INFO] Client disconnected: " << fd << RESET << std::endl;
		else
			std::cerr << RED << "[ERROR] Failed to read from client " << fd << RESET << std::endl;

		deleteClient(pollfds, fd);
		return 3;
	}

	std::cout << GREEN << "[INFO] Received " << bytesRead << " bytes from client " << fd << RESET << std::endl;

	// ✅ Append only new data to read buffer
	std::string newData(buffer, bytesRead);
	client->appendToReadBuffer(newData);
	std::string totalBuffer = client->getReadBuffer();
	std::cout << "[DEBUG] Buffer accumulated: " << totalBuffer << std::endl;

	std::string::size_type pos;
	// std::cout << "BEFORE : '" << totalBuffer << "'" << std::endl;

	while ((pos = totalBuffer.find('\n')) != std::string::npos)
	{
		std::string command = totalBuffer.substr(0, pos);
		totalBuffer.erase(0, pos + 1);

		this->parseMessage(client, command, pollfds);
		client = getClient(this, fd);
		if (!client)
			return 3;
	}

	if (totalBuffer.empty())
		client->resetBuffer();
	// std::cout << "AFTER : '" << client->getReadBuffer() << "'" << std::endl;

	return 0;
}

void Server::run() {
	
	std::vector<pollfd> pollfds;
	pollfd serverPfd;
	serverPfd.fd = _serverSocket;
	serverPfd.events = POLLIN;

	pollfds.push_back(serverPfd);


	while (serverRunning == 1) {
		std::cout << std::endl << RED << "[INFO]" << RESET << " Server running = " << serverRunning << std::endl;
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

void	Server::authenticateClient(Client &sender)
{
	AUTO_LOG
	if (sender.hasNickname() && sender.hasUsername() && sender.hasPassword())
	{
		std::string	msg = RPL_WELCOME(sender.getNickname());
		send(sender.getFd(), msg.c_str(), msg.size(), 0);
		sendChad(sender.getFd());
		// msg = RPL_YOURHOST(SERVER_NAME);
		// send(sender.getFd(), msg.c_str(), msg.size(), 0);
		// msg = RPL_CREATED();
		// send(sender.getFd(), msg.c_str(), msg.size(), 0);
		// msg = RPL_MYINFO();
		// send(sender.getFd(), msg.c_str(), msg.size(), 0);
		sender.authenticate();
	}
	if (sender.isAuthenticated() == true)
		{LOG(DEBUG "client #" << sender.getFd() << GREEN " is fully authentificated" RESET)}
	else
		{LOG(DEBUG "client #" << sender.getFd() << YELLOW " is not fully authentificated yet" RESET)}
	return ;
}


/*********************************************************************/
/*********************************************************************/
/******************************    QUIT    ***************************/
/*********************************************************************/
/*********************************************************************/

void Server::quitServer(Client &sender, std::vector<pollfd> &pollfds)
{
	AUTO_LOG

	int fd = sender.getFd();

	// Vérifier si le client est présent dans _clients
	if (_clients.find(fd) == _clients.end())
	{
		LOG(ERROR "The quitting client's fd is not present in our std::map!")
		return;
	}

	// Vérifier si le pollfd correspondant existe
	std::vector<pollfd>::iterator it = findPollfdIterator(fd, pollfds);
	if (it == pollfds.end())
	{
		LOG(ERROR "Could not find the matching fd in pollfds!")
		return;
	}

	// Fermer proprement le client
	deleteClient(pollfds, fd);
}


/*********************************************************************/
/*********************************************************************/
/******************************    TEST    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::testCommand(Client &sender)
{
	AUTO_LOG

	std::string	msg(10000000, 'A');
	send(sender.getSocket(), msg.c_str(), msg.size(), 0);

	send(sender.getSocket(), "B", 1, 0);
	return ;
}


/*********************************************************************/
/*********************************************************************/
/*****************************    CAP LS    **************************/
/*********************************************************************/
/*********************************************************************/

void	Server::sendCapabilities(Client &sender)
{
	AUTO_LOG
	LOG(DEBUG "CAP LS 302 triggered")

	// Return to HexChat that we do not have capabilities
	std::string msg = ":" SERVER_NAME " CAP * LS :\r\n";
	send(sender.getSocket(), msg.c_str(), msg.size(), 0);

	return ;

}
