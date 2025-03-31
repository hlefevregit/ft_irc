/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:50 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/31 22:13:36 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"
#include "../includes/Server.hpp"

Client::Client(int socket) : _socket(socket), _authentificated(false), _hasUsername(false), _hasNickname(false), _hasPassword(false), _nickname(""), _username(""), _readBuf(""), _password("") {}

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

std::string	Client::getDataSentByClientCleaned(int fd)
{
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesRead < 0)
	{
		std::cerr << ERROR << "Failed to read from client" << std::endl;
		return (std::string(""));
	}
	else if (bytesRead == 0)
	{
		std::cout << INFO << "Client disconnected" << std::endl;
		return (std::string(""));
	}

	std::string	cleanedMessage = messageCleaner(buffer);
	return (cleanedMessage);
}

void Client::joinedChannel(Channel* channel) {
	if (std::find(_joinedChannels.begin(), _joinedChannels.end(), channel) == _joinedChannels.end())
		_joinedChannels.push_back(channel);
	channel->addMember(this);
}

void Client::leaveChannel(const std::string& name) {
	for (std::vector<Channel*>::iterator it = _joinedChannels.begin(); it != _joinedChannels.end(); ++it) {
		if ((*it)->getName() == name) {
			_joinedChannels.erase(it);
			break;
		}
	}
}

const std::vector<Channel*>& Client::getJoinedChannels() const {
	return _joinedChannels;
}