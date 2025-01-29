/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:41:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/29 13:28:41 by hulefevr         ###   ########.fr       */
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
	std::map<int, Client > _user;
	std::vector<int> _operator;
public:
	Channel(const std::string &name);
	~Channel();

	const std::string &getName() const { return _name; };
	const std::map<int, Client > &getUser() const { return _user; };
	const std::vector<int> &getOperator() const { return _operator; };

	void addUserToChannel(Client &client);
};

#endif