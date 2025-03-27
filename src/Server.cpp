/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:46:11 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/27 18:44:21 by hulefevr         ###   ########.fr       */
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

// void Server::addClient(int clientSocket, std::vector<pollfd> &pollfds) {
// 	pollfd pfd;
// 	pfd.fd = clientSocket;
// 	pfd.events = POLLIN;
// 	Client newClient(clientSocket);
	
// 	_clients.insert(std::pair<int, Client>(clientSocket, newClient));
// 	std::cout << "\033[32m[INFO]\033[0m New client added #" << clientSocket << std::endl;

// 	std::string msg = "Welcome to the server, please enter the password: ";
// 	send(clientSocket, msg.c_str(), msg.size(), 0);
// 	char buffer[1024];
// 	std::memset(buffer, 0, sizeof(buffer));
// 	int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
// 	if (bytesRead < 0) {
// 		std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
// 	} else if (bytesRead == 0) {
// 		std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
// 	} else {
// 		std::string cleanPassword = messageCleaner(buffer);
// 		if (cleanPassword != _password) {
// 			std::string msg = "Wrong password\n";
// 			send(clientSocket, msg.c_str(), msg.size(), 0);
// 			std::cout << "\033[32m[INFO]\033[0m Wrong password, client disconnected" << std::endl;
// 			close(clientSocket);
// 		} else {
// 			std::string msg = "Password accepted\n";
// 			send(clientSocket, msg.c_str(), msg.size(), 0);
// 			sendChillguy(clientSocket);
// 			pollfds.push_back(pfd);
// 			// _pollfds.push_back(pfd);
// 		}
// 	}
// }

void Server::addClient(int clientSocket, std::vector<pollfd> &pollfds)
{
	Client		newClient(clientSocket);
	std::string	message;

	pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN;

	// /* Handling CAP LS */
	// // Recieve message
	// char buffer[1024];
	// std::memset(buffer, 0, sizeof(buffer));
	// int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

	// if (bytesRead < 0)
	// {
	// 	std::cerr << ERROR << "Failed to read from client" << std::endl;
	// 	return ;
	// }
	// else if (bytesRead == 0)
	// {
	// 	std::cout << INFO << "Client disconnected" << std::endl;
	// 	return ;
	// }
	// message = messageCleaner(buffer);

	// // Return to HexChat that we do not have capabilities
	// if (message.find("CAP LS 302") != std::string::npos)
	// {
	// 	std::string msg = ":" SERVER_NAME " CAP * LS :\r\n";
	// 	send(clientSocket, msg.c_str(), msg.size(), 0);
	// 	// send(clientSocket, msg.c_str(), msg.size(), 0);
	// }

	/* Add new client to std::map<const int, Client> */
	std::cout << INFO << "New unregistered client added #" << clientSocket << std::endl;
	_clients.insert(std::pair<int, Client>(clientSocket, newClient));
	pollfds.push_back(pfd);

	return ;
}

void Server::deleteClient(std::vector<pollfd> &pollfds, int fd) {
	std::cout << "\033[32m[INFO]\033[0m Deconnecting client #" << fd << std::endl;

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
		std::cerr << "\033[33m[WARN]\033[0m pollfd for fd " << fd << " not found." << std::endl;

	std::cout << "\033[32m[INFO]\033[0m Total clients: " << _clients.size() << std::endl;
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

// int	Server::readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it) {
	
// 	Client *client;
// 	client = getClient(this, it->fd);
	
// 	char buffer[4096];
// 	std::memset(buffer, 0, sizeof(buffer));
// 	int bytesRead = recv(it->fd, buffer, sizeof(buffer), 0);

// 	if (bytesRead < 0) {
// 		std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
// 		deleteClient(pollfds, it, it->fd);
// 		return 3;
// 	} else if (bytesRead == 0) {
// 		std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
// 		deleteClient(pollfds, it, it->fd);
// 		return 3;
// 	} else {

// 		std::cout << "\033[32m[INFO]\033[0m Received " << bytesRead << " bytes from client" << std::endl;
// 		client->setReadBuffer(buffer);
// 		if (bytesRead == 1)
// 			client->setReadBuffer("");
// 		else {
// 			while (!client->getReadBuffer().empty() && 
// 				(client->getReadBuffer()[client->getReadBuffer().size() - 1] == '\n' || 
// 				client->getReadBuffer()[client->getReadBuffer().size() - 1] == '\r')) 
// 			{
// 				client->setReadBuffer(client->getReadBuffer().erase(client->getReadBuffer().size() - 1, 1));
// 			}
// 		}
// 		std::cout << "\033[32m[INFO]\033[0m Message: " << buffer << std::endl;
// 		try {
// 			this->parseMessage(client, client->getReadBuffer());
// 			if (client->getReadBuffer().find("\r\n"))
// 				client->resetBuffer();
// 		} catch (const std::exception &e) {
// 			std::cerr << "\033[31m[ERROR]\033[0m " << e.what() << std::endl;
// 			return 3;
// 		}
// 		std::cout << YELLOW << "[DEBUG]" << RESET << " Message parsed" << std::endl;
// 	}
		
	
// 	return 0;
// }
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
	client->setReadBuffer(buffer);

	std::string temp_buffer = client->getReadBuffer();
	std::string::size_type pos;

	while ((pos = temp_buffer.find('\n')) != std::string::npos) {
		std::string command = temp_buffer.substr(0, pos);
		temp_buffer.erase(0, pos + 1);

		// ⚠️ parseMessage peut déclencher un QUIT → potentielle suppression !
		this->parseMessage(client, command, pollfds);

		// 🔒 On vérifie que le client existe toujours avant de continuer
		client = getClient(this, fd);
		if (!client) return 3;
	}

	client->setReadBuffer(temp_buffer);
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

void	Server::authenticateClient(Client &sender)
{
	std::cout << DEBUG << "│  ┌─ IN  authenticateClient ────────┐" << std::endl;
	if (sender.hasNickname() && sender.hasUsername() && sender.hasPassword())
		sender.authenticate();
	if (sender.isAuthenticated() == true)
		std::cout << DEBUG << "│  │  client #" << sender.getFd() << GREEN " is fully authentificated" RESET << std::endl;
	else
		std::cout << DEBUG << "│  │  client #" << sender.getFd() << YELLOW " is not fully authentificated yet" RESET << std::endl;
	std::cout << DEBUG << "│  └─ OUT authenticateClient ────────┘" << std::endl;
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

/*********************************************************************/
/*********************************************************************/
/******************************    PASS    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::connectToServerWithPass(Client &sender, std::string &params)
{
	std::cout << DEBUG << "┌─ IN  connectToServerWithPass ────────┐" << std::endl;
	if (!sender.hasNickname() && !sender.hasUsername())
	{
		// Get PASS
		// std::string::iterator	start = (params.begin() + params.find("PASS") + 5);
		std::string::iterator	start = params.begin();
		std::string				pass = std::string(start, params.end());

		// Get position of the end of the pass
		size_t	endPos = 0;
		while (start != params.end())
		{
			if (*start == ' ' || *start == '\n' || *start == '\r')	// Break to separator
				break;
			++endPos;
			++start;
		}

		// Truncate pass until separator
		pass = pass.substr(0, endPos);

		// Last checks
		if (pass.empty())
		{
			std::cerr << ERROR << "pass empty !" << std::endl;
			std::cout << DEBUG << "└─ OUT connectToServerWithPass ────────┘" << std::endl;
			return ;
		}
		std::cout << DEBUG << "│  extracted pass : " << pass << std::endl;

		// Authenticate user
		if (pass == _password)
		{
			std::cout << DEBUG << "│  Client #" << sender.getFd() << " successfully authentificated" << std::endl;
			sender.setPassword(pass);
			authenticateClient(sender);
		}
		else
			std::cout << ERROR << "│  Client #" << sender.getFd() << " failed authentificate : Wrong password !" << std::endl;
	}
	std::cout << DEBUG << "└─ OUT connectToServerWithPass ────────┘" << std::endl;
}

/*********************************************************************/
/*********************************************************************/
/******************************    NICK    ***************************/
/*********************************************************************/
/*********************************************************************/

bool	Server::isNicknameAvailable(std::string &nickname)
{
	std::map<const int, Client>::iterator	start = _clients.begin();
	std::map<const int, Client>::iterator	end = _clients.end();

	// Check if nickname correspond to a any Client's nickname
	while (start != end)
	{
		if (start->second.getNickname() == nickname)
			return false;
		++start;
	}
	return true;
}

void	Server::changeNickname(Client &sender, std::string &params)
{
	std::cout << DEBUG << "┌─ IN  changeNickname ────────┐" << std::endl;

	// Get nickname
	std::string::iterator	start = params.begin();
	std::string				nickname = std::string(start, params.end());

	// Get position of the end of the nickname
	size_t	endPos = 0;
	while (start != params.end())
	{
		if (*start == ' ' || *start == '\n' || *start == '\r')	// No more a word we can stop counting
			break;
		++endPos;
		++start;
	}
	endPos = endPos > 9 ? 9 : endPos;

	// Truncate nickname to fit 9 chars
	nickname = nickname.length() > 9 ? nickname.substr(0, endPos) : nickname;

	// Trim spaces
	start = nickname.begin();
	std::string::iterator	end = nickname.end();

	while (start != end && (*start == ' ' || *start == '\n' ||  *start == '\r'))
		++start;

	nickname = std::string(start, end);

	// Last checks
	if (nickname.empty())
	{
	std::cerr << ERROR << "│  Nickname empty !" << std::endl;
	std::cout << DEBUG << "└─ OUT changeNickname ────────┘" << std::endl;
		return ;
	}
	if (nickname == sender.getNickname())
	{
		std::cerr << INFO << "│  User is already using this nickname !" << std::endl;
		std::cout << DEBUG << "└─ OUT changeNickname ────────┘" << std::endl;
		return ;
	}
	if (!isNicknameAvailable(nickname))
	{
		std::cerr << INFO << "│  nickname already taken !" << std::endl;
		std::cout << DEBUG << "└─ OUT changeNickname ────────┘" << std::endl;
		return ;
	}
	std::cout << DEBUG << "│  User #" << sender.getFd() << std::endl;
	std::cout << DEBUG << "│  Changed Nickname from " << sender.getNickname() << " to " << nickname << std::endl;
	sender.setNickname(nickname);
	authenticateClient(sender);

	std::cout << DEBUG << "└─ OUT changeNickname ────────┘" << std::endl;
	return ;
}

/*********************************************************************/
/*********************************************************************/
/******************************    USER    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::changeUsername(Client &sender, std::string &params)
{
	std::cout << DEBUG << "┌─ IN  changeUsername ────────┐" << std::endl;

	// Get username
	std::string::iterator	start = params.begin();
	std::string				username = std::string(start, params.end());

	// Get position of the end of the username
	size_t	endPos = 0;
	while (start != params.end())
	{
		if (*start == ' ' || *start == '\n' || *start == '\r')	// No more a word we can stop counting
			break;
		++endPos;
		++start;
	}
	endPos = endPos > 9 ? 9 : endPos;

	// Truncate username to fit 9 chars
	username = username.length() > 9 ? username.substr(0, endPos) : username;

	// Last checks
	if (username.empty())
	{
		std::cerr << ERROR << "│  Nickname empty !" << std::endl;
		std::cout << DEBUG << "└─ OUT changeUsername ────────┘" << std::endl;
		return ;
	}
	if (username == sender.getUsername())
	{
		std::cerr << INFO << "│  User is already using this username" << std::endl;
		std::cout << DEBUG << "└─ OUT changeUsername ────────┘" << std::endl;
		return ;
	}

	std::cout << DEBUG << "│  User #" << sender.getFd() << std::endl;
	std::cout << DEBUG << "│  Changed Nickname from " << sender.getUsername() << " to " << username << std::endl;
	sender.setUsername(username);
	authenticateClient(sender);

	std::cout << DEBUG << "└─ OUT changeUsername ────────┘" << std::endl;
	return ;
}

/*********************************************************************/
/*********************************************************************/
/******************************    QUIT    ***************************/
/*********************************************************************/
/*********************************************************************/

void Server::quitServer(Client &sender, std::vector<pollfd> &pollfds)
{
	std::cout << DEBUG << "┌─ IN  quitServer ────────────┐" << std::endl;

	int fd = sender.getFd();

	// Vérifier si le client est présent dans _clients
	if (_clients.find(fd) == _clients.end())
	{
		std::cerr << ERROR << "│  The quitting client's fd is not present in our std::map!" << std::endl;
		std::cout << DEBUG << "└─ OUT quitServer ────────────┘" << std::endl;
		return;
	}

	// Vérifier si le pollfd correspondant existe
	std::vector<pollfd>::iterator it = findPollfdIterator(fd, pollfds);
	if (it == pollfds.end())
	{
		std::cerr << ERROR << "│  Could not find the matching fd in pollfds!" << std::endl;
		std::cout << DEBUG << "└─ OUT quitServer ────────────┘" << std::endl;
		return;
	}

	// Fermer proprement le client
	deleteClient(pollfds, fd);
	std::cout << DEBUG << "└─ OUT quitServer ────────────┘" << std::endl;
}


/*********************************************************************/
/*********************************************************************/
/******************************    TEST    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::testCommand(Client &sender)
{
	std::cout << DEBUG << "┌─ IN  testCommand ────────────┐" << std::endl;

	std::string	msg(10000000, 'A');
	send(sender.getSocket(), msg.c_str(), msg.size(), 0);

	send(sender.getSocket(), "B", 1, 0);
	std::cout << DEBUG << "└─ OUT testCommand ────────────┘" << std::endl;

	return ;
}


/*********************************************************************/
/*********************************************************************/
/*****************************    CAP LS    **************************/
/*********************************************************************/
/*********************************************************************/

void	Server::sendCapabilities(Client &sender)
{
	std::cout << DEBUG << "┌─ IN  sendCapabilities ────────────┐" << std::endl;

	// Return to HexChat that we do not have capabilities
	std::string msg = ":" SERVER_NAME " CAP * LS :\r\n";
	send(sender.getSocket(), msg.c_str(), msg.size(), 0);
	std::cerr << DEBUG << "│  CAP LS 302 re triggered" << std::endl;

	std::cout << DEBUG << "└─ OUT sendCapabilities ────────────┘" << std::endl;
	return ;
}

/*********************************************************************/
/*********************************************************************/
/*****************************    PRIVMSG    *************************/
/*********************************************************************/
/*********************************************************************/

std::map<const int, Client>::iterator	Server::getClientByNickname(const std::string &nickname)
{
	std::map<const int, Client>::iterator	start = _clients.begin();
	std::map<const int, Client>::iterator	end = _clients.end();

	// Check if nickname correspond to a Client's nickname
	while (start != end)
	{
		if (start->second.getNickname() == nickname)
			return (start);
		++start;
	}
	// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
	// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
	std::cerr << ERROR << "getClientByNickname: nickname not found amongs clients !" << RESET << std::endl;
	return (start);
}

void	Server::sendMessageUser(std::string &msg, const std::string &nickname, Client &sender)
{
	std::map<const int, Client>::iterator	reciever = getClientByNickname(nickname);
	std::map<const int, Client>::iterator	client_end = _clients.end();

	/****************/
	/* Basic checks */
	/****************/

	// Check if the user is found in the server to send a message to
	if (reciever->first == client_end->first)
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		std::cerr << ERROR << "sendMessageUser: client does not exist !" << RESET << std::endl;
		return ;
	}

	// Check if the sender is sending a PRIVMSG to itself
	if (sender.getNickname() == reciever->second.getNickname())
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		std::cerr << ERROR << "sendMessageUser: cannot send a PRIVMSG to itself !" << RESET << std::endl;
		return ;
	}

	/***********************************/
	/* Remove the targeted user in msg */
	/***********************************/
	std::string::iterator	start = msg.begin();
	std::string::iterator	end = msg.end();

	// Skips spaces
	while (start != end && *start == ' ')
		++start;
	// Skips first word
	while (start != end && *start != ' ')
		++start;
	// Skips spaces
	while (start != end && *start == ' ')
		++start;
	// Skips the semicolon ':' if needed
	if (start != end && *start == ':')
		++start;
	// Reform message
	std::string	cleaned_message(start, end);
	if (cleaned_message.empty())
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		std::cerr << ERROR << "sendMessageUser: cleaned_message is empty !" << RESET << std::endl;
		return ;
	}

	/************************/
	/* Send message to user */
	/************************/
	std::string	message = CYAN "Private message from [" + sender.getUsername() + "]: " + RESET + cleaned_message + "\n";
	send(reciever->first, message.c_str(), message.size(), 0);

	return ;
}

int    Server::sendMessage(Client sender, std::string &params)
{
	std::string				channel_prefix = "&#+!";
	std::string				first_word;
	std::string::iterator	start = params.begin();
	std::string::iterator	end = params.end();

	std::cout << DEBUG << "┌─ IN  sendMessage ───────────────────┐" << std::endl;

	// Check if msg is empty
	if (params.empty())
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		std::cerr << ERROR << "│  sendMessage: messsage is empty !   │" << RESET << std::endl;
		std::cout << DEBUG << "└─ OUT sendMessage ───────────────────┘" << std::endl;
		return 1;
	}
	// Skip leading spaces
	while (start != end && *start == ' ')
		++start;
	// Get first word
	while (start != end && *start != ' ')
	{
		first_word += *start;
		++start;
	}

	std::cout << DEBUG << "│  first word : " << first_word << std::endl;

	// Checks first word's prefix
	if (first_word.size() > 1)
	{
		if (channel_prefix.find(first_word[0]) != std::string::npos)
			std::cout << DEBUG << "│  is Channel" << RESET << std::endl;
			// sendMessageChannel();    // TODO : dguerin implementation
		else
		{
			std::cout << DEBUG << "│  is User" << RESET << std::endl;
			sendMessageUser(params, first_word, sender);   // TODO : ldalmass implementation
		}
	}

	std::cout << DEBUG << "└─ OUT sendMessage ───────────────────┘" << std::endl;
	return 0;
}

/*********************************************************************/
/*********************************************************************/
/*****************************      BOT      *************************/
/*********************************************************************/
/*********************************************************************/

// void	Server::botHelp(Client sender)
// {
// 	std::string	message = GREEN "Hey " + sender.getUsername() + " I'm joe !\n"
// 	RESET "How to use :\n"
// 	"PRIVMSG joe stats" + YELLOW + " - Display various server statistics.\n" + RESET +
// 	"PRIVMSG joe user [nickname]" + YELLOW + " - Display informations about an user.\n" + RESET +
// 	"PRIVMSG joe channel [channel's name]" + YELLOW + " - Display informations about a channel.\n" + RESET
// 	;
// 	send(sender.getFd(), message.c_str(), message.size(), 0);
// 	return ;
// }

// void	Server::botParse(Client sender, std::string &params)
// {
// 	std::string				word;
// 	std::string::iterator	start = params.begin();
// 	std::string::iterator	end = params.end();

// 	std::cout << DEBUG << "┌─ IN  botParse ──────────────────────┐" << std::endl;

// 	// Check if msg is empty
// 	if (params.empty())
// 	{
// 		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
// 		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
// 		std::cerr << ERROR << "│  botParse: messsage is empty !      │" << RESET << std::endl;
// 		std::cout << DEBUG << "└─ OUT botParse ──────────────────────┘" << std::endl;
// 		return ;
// 	}
// 	// Skip leading spaces
// 	while (start != end && *start == ' ')
// 		++start;
// 	// Skip first word spaces
// 	while (start != end && *start != ' ')
// 		++start;
// 	// Skip spaces
// 	while (start != end && *start == ' ')
// 		++start;
// 	// Get second word
// 	while (start != end && *start != ' ')
// 	{
// 		word += *start;
// 		++start;
// 	}

// 	std::cout << DEBUG << "│  magic word : " << word << std::endl;

// 	// Check if command
// 	if (!word.empty())
// 	{
// 		if (word == "stats")
// 		{
// 			std::cout << DEBUG << "│  is stats command" << RESET << std::endl;
// 		}
// 		else if (word == "user")
// 		{
// 			std::cout << DEBUG << "│  is user command" << RESET << std::endl;
// 		}
// 		else if (word == "channel")
// 		{
// 			std::cout << DEBUG << "│  is channel command" << RESET << std::endl;
// 		}
// 		else
// 		{
// 			std::cout << DEBUG << "└─ OUT botParse ──────────────────────┘" << std::endl;
// 			return ;
// 		}
// 	}
// 	else
// 		botHelp(sender);

// 	std::cout << DEBUG << "└─ OUT botParse ──────────────────────┘" << std::endl;
// 	return ;
// }