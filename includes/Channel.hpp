/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 16:41:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/04 12:55:28 by hulefevr         ###   ########.fr       */
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
	std::set<std::string>	_invitedNicknames;
	
	size_t						_userLimit;

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
	size_t						getUserLimit() const { return _userLimit; };


	// Membres
	bool 						hasMember(const Client& client) const;
	bool 						hasMember(const int &fd) const;
	void 						addMember(Client *client);
	void 						removeMember(int fd);
	std::vector<std::string>	getMemberNames() const;
	std::vector<Client*>		getMembers() const;


	// Broadcast
	void						broadcastModeChange(const Client& setter, char mode, bool enable, const std::string& arg = "");
	void						broadcast(const std::string& message, int exceptFd = -1);
	void						sendNumericReply(int fd, int code, const std::string& message) const;
	// Operators
	void						addOperator(int fd);
	bool						isOperator(int fd) const;
	bool						isOperator(const Client &client) const;
	std::set<int>				getOperators() const { return _operators; }
	std::string					getOpList() const;

	bool						hasMode(char c) const;
	const						std::string& getKey() const;
	void						sendNamesReply(Client &client);

	// Invites
	bool						isInvited(const Client &client) const;
	void						inviteClient(const Client &client);
	void						setMode(char mode, bool emable, const Client &client, const std::string &arg = "");
	std::string					getModes() const;
};

#endif