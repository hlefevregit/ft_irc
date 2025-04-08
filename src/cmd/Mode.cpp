/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 19:00:37 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/08 17:48:00 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/numericalReplies.hpp"

void Server::modeCommand(Client &client, const std::string &params)
{
	AUTO_LOG
	std::istringstream iss(params);
	std::string channelName, modeString, arg;
	iss >> channelName >> modeString;

	LOG(DEBUG "modeCommand: channelName = " << channelName)
	LOG(DEBUG "modeCommand: modeString = " << modeString)
	LOG(DEBUG "iss = " << iss.str())

	if (channelName.empty()) {
		LOG(INFO "Client " << client.getNickname() << " failed to set mode (no channel name)")
		sendNumericReply(client.getFd(), 461, "MODE :Not enough parameters");
		return;
	}

	Channel *channel = getChannel(channelName);
	if (!channel) {
		LOG(INFO "Client " << client.getNickname() << " failed to set mode on channel " << channelName << " (no such channel)")
		sendNumericReply(client.getFd(), 403, channelName); // No such channel
		return;
	}

	if (modeString.empty()) {
		// Just want to view current modes
		LOG(INFO "Client " << client.getNickname() << " requested modes for channel " << channelName)
		std::string modes = channel->getModes();
		std::string msg = ":ircserv 324 " + client.getNickname() + " " + channelName + " " + modes + "\r\n";
		send(client.getFd(), msg.c_str(), msg.length(), 0);
		return;
	}

	// Check if client is operator
	if (!channel->isOperator(client.getFd())) {
		std::string msg = ":ircserv 482 " + client.getNickname() + " " + channelName + " :You're not channel operator\r\n";
		send(client.getFd(), msg.c_str(), msg.length(), 0);
		return;
	}

	bool adding = true;
	for (std::string::size_type i = 0; i < modeString.length(); ++i) {
		char c = modeString[i];
		LOG(DEBUG "Processing mode: " << c)
		if (c == '+')
			adding = true;
		else if (c == '-')
			adding = false;
		else {
			LOG(DEBUG "Setting mode: " << c)
			if (c == 'k' || c == 'l' || c == 'o'|| c == 'i' || c == 't') {
				if (((adding == true) && (c == 'l' || c == 'k' || c == 'o')) || (adding == false && c == 'o'))
					iss >> arg;
				else
					arg = " ";
				if (arg.empty()) {
					LOG(INFO "Client " << client.getNickname() << " failed to set mode " << c << " on channel " << channelName << " (missing argument)")
					sendNumericReply(client.getFd(), 461, "MODE :Not enough parameters");
					continue;
				}
			}
			channel->setMode(c, adding, client, arg);
		}
	}
}

std::string Channel::getModes() const
{
	std::string result = "+";

	std::set<char>::const_iterator it = _modes.begin();
	while (it != _modes.end()) {
		result += *it;
		++it;
	}
	if (hasMode('k'))
		result += " " + _key;
	if (hasMode('l')) {
		std::ostringstream oss;
		oss << " " << _userLimit;
		result += oss.str();
	}
	return result;
}

void Channel::setMode(char mode, bool enable, const Client& client, const std::string& arg)
{
	AUTO_LOG
	LOG(INFO "Client " << client.getNickname() << " set mode " << (enable ? "+" : "-") << mode << " on channel " << _name)
	if (!isOperator(client.getFd())) {
		LOG(INFO "Client " << client.getNickname() << " failed to set mode " << mode << " on channel " << _name << " (not operator)")
		sendNumericReply(client.getFd(), 482, _name); // You're not channel operator
		return;
	}
	if (hasMember(client.getFd()) == false) {
		LOG(INFO "Client " << client.getNickname() << " failed to set mode " << mode << " on channel " << _name << " (not member)")
		sendNumericReply(client.getFd(), 442, _name); // You're not on that channel
		return;
	}
	switch (mode) {
		case 'i':
			if (enable)
				_modes.insert(mode);
			else
				_modes.erase(mode);
			LOG(DEBUG "Mode 'i' set to " << (enable ? "enabled" : "disabled"))
			break;
		case 't':
			if (enable)
				_modes.insert(mode);
			else
				_modes.erase(mode);
			LOG(DEBUG "Mode 't' set to " << (enable ? "enabled" : "disabled"))
			break;

		case 'k':
			if (isOperator(client.getFd()))
			{
				if (enable) {
					if (!arg.empty()) {
						_modes.insert('k');
						_key = arg;
						LOG(DEBUG "Key set to " << _key)
					}
				} else {
					_modes.erase('k');
					_key = "";
				}
				LOG(DEBUG "Mode 'k' set to " << (enable ? "enabled" : "disabled"))
				
			}
			else
			{
				LOG(INFO "Client " << client.getNickname() << " failed to set mode k on channel " << _name << " (not operator)")
				sendNumericReply(client.getFd(), 482, _name); // You're not channel operator
			}
			break;
		case 'l':
			if (enable) {
				if (!arg.empty()) {
					std::istringstream iss(arg);
					int limit;
					iss >> limit;
					if (!iss.fail() && limit > 0) {
						_modes.insert('l');
						_userLimit = limit;
					}
				}
			} else {
				_modes.erase('l');
				_userLimit = -1;
			}
			LOG(DEBUG "Mode 'l' set to " << (enable ? "enabled" : "disabled"))
			break;

		case 'o': {
			LOG(DEBUG "Mode 'o' set to " << (enable ? "enabled" : "disabled"))
			std::map<int, Client*>::iterator it = _members.begin();
			for (; it != _members.end(); ++it) {
				if (it->second->getNickname() == arg) {
					if (enable)
					{
						_operators.insert(it->first);
						LOG(DEBUG "Client " << it->second->getNickname() << " is now operator")
						std::string msg = ":" + client.getPrefix() + " MODE " + _name + " +" + mode + " " + arg + "\r\n";
						send(it->first, msg.c_str(), msg.length(), 0);
						this->broadcast(msg, it->first);
					}
					else
					{
						_operators.erase(it->first);
						LOG(DEBUG "Client " << it->second->getNickname() << " is no longer operator")
						std::string msg = ":" + client.getPrefix() + " MODE " + _name + " -" + mode + " " + arg + "\r\n";
						send(it->first, msg.c_str(), msg.length(), 0);
						this->broadcast(msg, it->first);
					}
					LOG(INFO "Operator list = " << getOpList())
					break;
				}
			}
			break;
		}

		default:
			sendNumericReply(client.getFd(), 472, std::string(1, mode)); // Unknown mode
			break;
	}
	this->broadcastModeChange(client, mode, enable, arg);
}


void Channel::broadcastModeChange(const Client& setter, char mode, bool enable, const std::string& arg) {
	std::string msg = ":" + setter.getPrefix() + " MODE " + _name + " ";
	msg += (enable ? "+" : "-");
	msg += mode;

	if (mode == 'k' || mode == 'l' || mode == 'o')
		msg += " " + arg;

	msg += "\r\n";

	if (mode != 'o')
	{
		std::map<int, Client*>::iterator it;
		for (it = _members.begin(); it != _members.end(); ++it) {
			send(it->first, msg.c_str(), msg.length(), 0);
		}
	}
}