/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:41:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/02 15:10:20 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP


#include <string>
#include <map>
#include <vector>
#include <set>
#include "Client.hpp"

class Client;

class Channel {
private:
	std::string 			_name;
	std::string 			_topic;
	std::map<int, Client*> 	_members; // clé: fd du client
	std::set<int>			_operators;
	std::string				_key;
	std::set<char>			_modes;

public:
	Channel();
	Channel(const std::string& name);
	Channel(const std::string& name, const Client& creator);
	~Channel();

	// Accesseurs
	const std::string&			getName() const { return _name; };
	const std::string&			getTopic() const { return _topic; };
	void 						setTopic(const std::string& topic) { _topic = topic; };
	void 						setName(const std::string& name) { _name = name; };

	// Membres
	bool 						hasMember(const Client& client) const;
	bool 						hasMember(const int &fd) const;
	void 						addMember(Client *client);
	void 						removeMember(int fd);
	std::vector<std::string>	getMemberNames() const;


	// Broadcast
	void						broadcast(const std::string& message, int exceptFd = -1);

	// Operators
	void						addOperator(int fd);
	bool						isOperator(int fd) const;

	bool						hasMode(char c) const;
	const						std::string& getKey() const;
	void						sendNamesReply(Client &client);
};

#endif