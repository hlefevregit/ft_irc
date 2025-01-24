/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:45:49 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/23 17:51:40 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <algorithm>
#include <iostream>
#include <sys/socket.h>

Channel::Channel(const std::string &name) : _name(name) {}

Channel::~Channel() {}

std::string const &Channel::getName() const {
	return _name;
}

void Channel::addUser(Client *client) {
	int fd = client->getFd();
	if (_user.find(fd) == _user.end()) {
		_user[fd] = client;
	}
}

void Channel::removeUser(int fd) {
	_user.erase(std::remove(_user.begin(), _user.end(), fd), _user.end());
}

void Channel::addOperator(int fd) {
	if (!isOperator(fd)) {
		_operator.push_back(fd);
	}
}

void Channel::removeOperator(int fd) {
	_operator.erase(std::remove(_operator.begin(), _operator.end(), fd), _operator.end());
}

bool Channel::isOperator(int fd) const {
	return std::find(_operator.begin(), _operator.end(), fd) != _operator.end();
}

bool Channel::hasUser(int fd) const {
	return std::find(_user.begin(), _user.end(), fd) != _user.end();
}

void Channel::broadcast(const std::string &message, int senderFd) {
	for (std::map<int, Client*>::iterator it = _user.begin(); it != _user.end(); ++it) {
		if (it->first != senderFd) {
			send(it->first, message.c_str(), message.size(), 0);
		}
	}
}