/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:41:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/24 14:29:05 by hugolefevre      ###   ########.fr       */
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
};

#endif