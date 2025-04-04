/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 10:52:09 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/04 12:45:17 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"
#include "../../includes/numericalReplies.hpp"


void Server::joinCommand(Client &client, const std::string &params)
{
	AUTO_LOG
	std::vector<std::string> channels;
	std::vector<std::string> keys;

	// Séparer les channels et les keys
	std::string::size_type pos = params.find(' ');
	std::string chanStr = pos != std::string::npos ? params.substr(0, pos) : params;
	std::string keyStr = pos != std::string::npos ? params.substr(pos + 1) : "";

	std::istringstream chanStream(chanStr);
	std::istringstream keyStream(keyStr);

	std::string token;
	while (std::getline(chanStream, token, ','))
		channels.push_back(token);
	while (std::getline(keyStream, token, ','))
		keys.push_back(token);

	for (size_t i = 0; i < channels.size(); ++i) {
		std::string chanName = channels[i];
		std::string key = (i < keys.size()) ? keys[i] : "";

		// Validation nom de canal
		if (chanName.empty() || (chanName[0] != '#' && chanName[0] != '&')) {
			sendNumericReply(client.getFd(), 403, chanName);
			LOG(INFO "Client " << client.getNickname() << " failed to join channel " << chanName << " (invalid name)")
			continue;
		}

		Channel *channel = getChannel(chanName);
		if (!channel) {
			// Création canal si inexistant
			channel = new Channel(chanName, client);
			_channels[chanName] = channel;
			channel->addOperator(client.getFd());
			LOG(INFO "Channel " << chanName << " created with client " << client.getNickname() << " as operator")
		} else {
			// Vérif +k
			if (channel->hasMode('k') && channel->getKey() != key) {
				sendNumericReply(client.getFd(), 475, chanName);
				LOG(INFO "Client " << client.getNickname() << " failed to join channel " << chanName << " (wrong key)")
				continue;
			}
			if (channel->hasMode('i') && !channel->isInvited(client)) {
				sendNumericReply(client.getFd(), 473, chanName);  // ERR_INVITEONLYCHAN
				LOG(INFO "Client " << client.getNickname() << " cannot join invite-only channel " << chanName)
				continue;
			}
			if (channel->hasMode('l') && channel->getMembers().size() >= channel->getUserLimit()) {
				sendNumericReply(client.getFd(), 471, chanName);  // ERR_CHANNELISFULL
				LOG(INFO "Client " << client.getNickname() << " cannot join full channel " << chanName)
				continue;
			}
		}

		if (!channel->hasMember(client))
			client.joinedChannel(channel);
		// RPL_NAMREPLY + RPL_ENDOFNAMES
		sendJoinReplies(client, *channel);
		
		
		std::string modes = channel->getModes();
		std::string modeLine = ":ircserv 324 " + client.getNickname() + " " + chanName + " " + modes + "\r\n";
		send(client.getFd(), modeLine.c_str(), modeLine.length(), 0);

		LOG(INFO "Client " << client.getNickname() << " joined channel " << chanName)
	}
}

std::string Server::getOpList(Channel &channel) {
    std::string opList;
    const std::vector<Client*>& members = channel.getMembers();

    for (std::vector<Client*>::const_iterator it = members.begin(); it != members.end(); ++it) {
        Client* member = *it;
        if (channel.isOperator(member->getFd())) {
            if (!opList.empty())
                opList += " ";
            opList += "@" + member->getNickname();  // Convention IRC : @ = operator
        }
    }

    return opList;
}

void Server::sendJoinReplies(Client &client, Channel &channel)
{
	AUTO_LOG
	const std::string& channelName = channel.getName();
	std::string nick = client.getNickname();
	std::string prefix = client.getPrefix(); // ex: nick!user@host

	// 1. JOIN
	std::string joinMsg = prefix + " JOIN " + channelName + "\r\n";
	LOG(INFO "Sending JOIN message to client " << client.getFd())
	send(client.getFd(), joinMsg.c_str(), joinMsg.length(), 0);

	// 2. RPL_TOPIC (332)
	std::string topic = channel.getTopic();
	if (topic.empty())
		topic = "No topic is set";
	std::string rplTopic = ":" + _serverName + " 332 " + nick + " " + channelName + " :" + topic + "\r\n";
	send(client.getFd(), rplTopic.c_str(), rplTopic.length(), 0);

	// // 3. RPL_TOPICWHOTIME (333)
	// std::ostringstream oss;
	// oss << ":" << _serverName << " 333 " << nick << " " << channelName << " "
	// 	<< channel.getTopicSetter() << " " << channel.getTopicSetTime() << "\r\n";
	// std::string topicWhoTime = oss.str();
	// send(client.getFd(), topicWhoTime.c_str(), topicWhoTime.length(), 0);

	// 4. RPL_NAMREPLY (353)
	std::string nameList;
	std::vector<std::string> members = channel.getMemberNames();
	for (std::vector<std::string>::iterator it = members.begin(); it != members.end(); ++it) {
		if (!nameList.empty())
			nameList += " ";
		nameList += *it;
	}
	std::string rplNames = ":" + _serverName + " 353 " + nick + " = " + channelName + " :" + nameList + "\r\n";
	send(client.getFd(), rplNames.c_str(), rplNames.length(), 0);

	// 5. RPL_ENDOFNAMES (366)
	std::string rplEnd = ":" + _serverName + " 366 " + nick + " " + channelName + " :End of /NAMES list\r\n";
	send(client.getFd(), rplEnd.c_str(), rplEnd.length(), 0);
	
	std::string op_list = getOpList(channel);
	LOG(INFO "List des op du chan : " << op_list)
}
