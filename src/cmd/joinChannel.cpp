/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 10:52:09 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/31 16:12:15 by hulefevr         ###   ########.fr       */
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
			_channels[chanName] = Channel(chanName, client);
			channel = &_channels[chanName];
			LOG(INFO "Channel " << chanName << " created")
		} else {
			// Vérif +k
			if (channel->hasMode('k') && channel->getKey() != key) {
				sendNumericReply(client.getFd(), 475, chanName);
				LOG(INFO "Client " << client.getNickname() << " failed to join channel " << chanName << " (wrong key)")
				continue;
			}
		}

		if (!channel->hasMember(client))
			client.joinedChannel(channel);

		std::string joinMsg = ":" + client.getPrefix() + " JOIN " + chanName + "\r\n";
		channel->broadcast(joinMsg);
		LOG(INFO "Client " << client.getNickname() << " joined channel " << chanName)
		// RPL_TOPIC si topic existant
		if (!channel->getTopic().empty()) {
			sendNumericReply(client.getFd(), 332, chanName + " :" + channel->getTopic());
		}

		// RPL_NAMREPLY + RPL_ENDOFNAMES
		channel->sendNamesReply(client);
		LOG(INFO "Client " << client.getNickname() << " joined channel " << chanName)
	}
}