/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:41:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/21 19:47:18 by ldalmass         ###   ########.fr       */
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
	// std::string _topic;
	std::map<int, Client > _user;
	std::vector<int> _operator;
public:
	Channel(const std::string &name);
	~Channel();

	const std::string &getName() const { return _name; };
	const std::map<int, Client > &getUser() const { return _user; };
	const std::vector<int> &getOperator() const { return _operator; };

	void addUserToChannel(Client &client);
	// void invinteUserToChannel(Client &client);
};

#endif