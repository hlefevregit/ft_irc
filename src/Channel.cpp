/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:45:49 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/04 13:42:39 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <algorithm>
#include <iostream>
#include <sys/socket.h>
#include "../includes/Server.hpp"

Channel::Channel() {}
Channel::Channel(const std::string& name) : _name(name), _topic("") {}
Channel::~Channel() {
	std::cout << CYAN << "[INFO] " << RESET << "Channel " << _name << " destroyed" << std::endl;
}

Channel::Channel(const std::string& name, const Client& creator) : _name(name) {
	_members[creator.getFd()] = const_cast<Client*>(&creator);
	_operators.insert(creator.getFd());
}

bool Channel::hasMember(const Client& client) const
{
	return _members.find(client.getFd()) != _members.end();
}

bool Channel::hasMember(const int &fd) const
{
	return _members.find(fd) != _members.end();
}

void Channel::addMember(Client *client)
{
	_members[client->getFd()] = client;
}

void Channel::removeMember(int fd)
{
	_members.erase(fd);
}
std::vector<std::string> Channel::getMemberNames() const
{
	std::vector<std::string> names;
	std::map<int, Client*>::const_iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		names.push_back(it->second->getNickname());
	}
	return names;
}

std::vector<Client*> Channel::getMembers() const
{
	std::vector<Client*> members;
	std::map<int, Client*>::const_iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		members.push_back(it->second);
	}
	return members;
}

void Channel::broadcast(const std::string& message, int exceptFd)
{
	AUTO_LOG
	std::map<int, Client*>::const_iterator it;
	for (it = _members.begin(); it != _members.end(); ++it)
	{
		if (it->second->getFd() != exceptFd)
		{
			LOG(DEBUG "Sending message to client " << it->second->getFd())
			send(it->second->getFd(), message.c_str(), message.size(), 0);
		}
	}
}

void Channel::addOperator(int fd)
{
	_operators.insert(fd);
}

bool Channel::isOperator(int fd) const
{
	return _operators.find(fd) != _operators.end();
}

bool Channel::hasMode(char c) const 
{
	return _modes.find(c) != _modes.end();
}

const std::string& Channel::getKey() const 
{
	return _key;
}

bool Channel::isOperator(const Client& client) const 
{
	return _operators.find(client.getFd()) != _operators.end();
}

bool Channel::isOperator(const std::set<int>::iterator &it) const 
{
	return _operators.find(*it) != _operators.end();
}

int	Channel::getMemberFd(std::vector<std::string>::iterator it) const
{
	std::map<int, Client*>::const_iterator memberIt = _members.begin();
	while (memberIt != _members.end())
	{
		if (memberIt->second->getNickname() == *it)
			return memberIt->second->getFd();
		memberIt++;
	}
	return -1;
}

void Channel::sendNumericReply(int fd, int code, const std::string& message) const 
{
	std::stringstream ss;
	ss << code;
	std::string reply = ":" + _name + " " + ss.str() + " " + message + "\r\n";
	send(fd, reply.c_str(), reply.size(), 0);
}

// Envoie :353 & 366
void Channel::sendNamesReply(Client &client) {
	std::string names;
	for (std::map<int, Client*>::const_iterator it = _members.begin(); it != _members.end(); ++it) {
		if (!names.empty())
			names += " ";
		names += it->second->getNickname();
	}
	std::string chan = getName();
	std::string msg = "ircserv: 353 " + client.getNickname() + " = " + chan + " :" + names + "\r\n";
	send(client.getFd(), msg.c_str(), msg.size(), 0);
	msg = "ircserv: 366 " + client.getNickname() + " " + chan + " :End of NAMES list\r\n";
	send(client.getFd(), msg.c_str(), msg.size(), 0);
	
}

std::string Channel::getOpList() const {
    std::string opList;
    const std::vector<Client*>& members = this->getMembers();

    for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        Client* member = *it;
        if (this->isOperator(member->getFd())) {
            if (!opList.empty())
                opList += " ";
            opList += "@" + member->getNickname();  // Convention IRC : @ = operator
        }
    }

    return opList;
}

bool Channel::isInvited(const Client &client) const {
	return _invitedNicknames.find(client.getNickname()) != _invitedNicknames.end();
}

void Channel::inviteClient(const Client &client) {
	_invitedNicknames.insert(client.getNickname());
}

// Channel.cpp



/*********************************************************************/
/*********************************************************************/
/*****************************CHANNEL HANDLER*************************/
/*********************************************************************/
/*********************************************************************/


Channel* Server::getChannel(const std::string &name)
{
	std::map<std::string, Channel*>::iterator it = _channels.find(name);
	if (it != _channels.end())
		return it->second;
	return NULL;
}