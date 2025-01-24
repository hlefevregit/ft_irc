/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:50 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/24 18:33:17 by hugolefevre      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(int socket) : _socket(socket), _authentificated(false), _hasUsername(false), _hasNickname(false), _nickname(""), _username(""), _readBuf("") {}

Client::Client(const Client &src) {
	*this = src;
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

int Client::authentification(Client *client) {
	char buffer[1024];
	memset(buffer, 0, sizeof(buffer));
	
	if (!hasUsername())
	{
		std::string alt = "Authentification ...\n";
		send(client->getSocket(), alt.c_str(), alt.size(), 0);
		std::string msg = "Enter your username: ";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		int bytesRead = recv(client->getSocket(), buffer, sizeof(buffer), 0);
		if (bytesRead < 0) {
			std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
			return 3;
		} else if (bytesRead == 0) {
			std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
			return 3;
		} else {
			client->setUsername(buffer);
			std::cout << "Username: " << client->getUsername() << std::endl;
		}
	}
	else if (!hasNickname())
	{
		std::string msg = "Enter your nickname: ";
		send(client->getSocket(), msg.c_str(), msg.size(), 0);
		int bytesRead = recv(client->getSocket(), buffer, sizeof(buffer), 0);
		if (bytesRead < 0) {
			std::cerr << "\033[31m[ERROR]\033[0m Failed to read from client" << std::endl;
			return 3;
		} else if (bytesRead == 0) {
			std::cout << "\033[32m[INFO]\033[0m Client disconnected" << std::endl;
			return 3;
		} else {
			client->setNickname(buffer);
			std::cout << "Nickname: " << client->getNickname() << std::endl;
		}
	}
	std::cout << "_hasUsername: " << client->hasUsername() << std::endl;
	std::cout << "_hasNickname: " << client->hasNickname() << std::endl;
	if (client->hasUsername() == true && client->hasNickname() == true)
	{
		client->authenticate();
	}
	return 0;
}