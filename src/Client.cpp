/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:50 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/31 15:48:01 by hugolefevre      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

Client::Client(int socket) : _socket(socket), _authentificated(false), _hasUsername(false), _hasNickname(false), _nickname(""), _username(""), _readBuf("") {}

Client::Client(const Client &src) {
	*this = src;
}

Client &Client::operator=(const Client &src) {
	if (this != &src) {
		_socket = src._socket;
		_authentificated = src._authentificated;
		_hasUsername = src._hasUsername;
		_hasNickname = src._hasNickname;
		_nickname = src._nickname;
		_username = src._username;
		_readBuf = src._readBuf;
	}
	return *this;
}

Client::~Client() {}

int Client::getSocket() const {
	return _socket;
}

bool Client::isAuthenticated() const {
	return _authentificated;
}

void Client::authenticate() {
	_authentificated = true;
}

void Client::setReadBuffer(std::string const &buf) {
	_readBuf += buf;
}

std::string messageCleaner(char *buffer)
{
	std::string message(buffer);
	std::string cleanMessage(message);
	while (!cleanMessage.empty() && (cleanMessage[cleanMessage.size() - 1] == '\n' || cleanMessage[cleanMessage.size() - 1] == '\r'))
		cleanMessage = cleanMessage.erase(cleanMessage.size() - 1);
	return cleanMessage;
}

int Client::authentification(Client *client) {
	if (!_hasNickname) {
		std::string msg = "Please enter a nickname: ";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		char buffer[4096];
		std::memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(client->getSocket(), buffer, sizeof(buffer), 0);
		if (bytesRead < 0) {
			std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
			return 0;
		} else if (bytesRead == 0) {
			std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
			return 0;
		} else {
			std::string nickname = messageCleaner(buffer);
			if (nickname.length() > 9) {
				std::string msg = "Nickname too long, please enter a nickname that is less than 9 characters long: ";
				send(client->getSocket(), msg.c_str(), msg.size(), 0);
				this->authentification(client);
			}
			client->setNickname(nickname);
		}
	}
	if (!_hasUsername) {
		std::string msg = "Please enter a username: ";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		char buffer[4096];
		std::memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(client->getSocket(), buffer, sizeof(buffer), 0);
		if (bytesRead < 0) {
			std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
			return 0;
		} else if (bytesRead == 0) {
			std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
			return 0;
		} else {
			std::string username = messageCleaner(buffer);
			client->setUsername(username);
		}
	}
	if (!_hasPassword)
	{
		std::string msg = "Please enter a password: ";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		char buffer[4096];
		std::memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(client->getSocket(), buffer, sizeof(buffer), 0);
		if (bytesRead < 0) {
			std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
			return 0;
		} else if (bytesRead == 0) {
			std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
			return 0;
		} else {
			std::string password = messageCleaner(buffer);
			client->setPassword(password);
		}
	}
	if (_hasUsername && _hasNickname && _hasPassword) {
		authenticate();
		std::string msg = "Welcome " + _nickname + "!\n";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		sendChad(client->getSocket());
		return 1;
	}
	return 0;
}

#include <fstream>

void sendChad(int clientSocket)
{
	std::ifstream file;
	char		filePath[22] = "./src/chad.config";
	
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