/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:41:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/23 17:41:40 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include "Client.hpp"

class Client;

class Channel {
private:
	std::string _name;
	std::map<int, Client *> _user;
	std::vector<int> _operator;
public:
	Channel(const std::string &name);
	~Channel();

	const std::string &getName() const;

	void addUser(Client *client);
	void removeUser(int fd);
	
	void addOperator(int fd);
	void removeOperator(int fd);

	bool isOperator(int fd) const;
	bool hasUser(int fd) const;
	
	void broadcast(const std::string &message, int senderFd = -1);
};

#endif