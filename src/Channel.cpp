/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:45:49 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/29 13:38:10 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Channel.hpp"
#include <algorithm>
#include <iostream>
#include <sys/socket.h>

Channel::Channel(const std::string &name) : _name(name) {}

Channel::~Channel() {}

void	Channel::addUserToChannel(Client &client)
{
	std::map<int, Client>::iterator it = _user.find(client.getFd());
	if (it == _user.end())
	{
		_user.insert(std::pair<int, Client>(client.getFd(), client));
		std::cout << "\033[32m[INFO]\033[0m " << client.getNickname() << " joined channel " << _name << std::endl;
	}
	else {
		std::cout << "\033[31m[ERROR]\033[0m " << client.getNickname() << " is already on channel " << _name << std::endl;
		std::string msg = ERR_USERONCHANNEL(client.getNickname(), _name);
		send(client.getFd(), msg.c_str(), msg.size(), 0);
	}
}