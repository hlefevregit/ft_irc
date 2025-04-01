/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Topic.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 17:19:42 by ldalmass          #+#    #+#             */
/*   Updated: 2025/04/01 19:55:19 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void	Server::changeTopic(Client &sender, std::string &params)
{
	AUTO_LOG

	// Extract the channel's name correctly
	std::string				extractedChannelName;
	std::string::iterator	paramsStart = params.begin();
	std::string::iterator	paramsEnd = params.end();

	// Skips spaces if any
	while (paramsStart != paramsEnd && *paramsStart == ' ')
		++paramsStart;
	// Add letter to extractedChannelName
	while (paramsStart != paramsEnd && *paramsStart != ' ')
	{
		extractedChannelName += *paramsStart;
		++paramsStart;
	}
	LOG(INFO "extractedChannelName : " << extractedChannelName)
	// Checks for extractedChannelName
	if (extractedChannelName.empty())
	{
		LOG(ERROR "No channels we're provided with the TOPIC command ❌")
		return ;
	}

	// Iterate through all channels to find the one we want
	Channel *									channel;
	std::map<std::string, Channel *>			channels = getChannels();
	std::map<std::string, Channel *>::iterator	channelsStart = channels.begin();
	std::map<std::string, Channel *>::iterator	channelsEnd = channels.end();

	while (channelsStart != channelsEnd)
	{
		if (channelsStart->second->getName() == extractedChannelName) // We've found the channel
		{
			channel = channelsStart->second;
			break;
		}
		++channelsStart;
	}
	// Checks if the channel exists
	if (channelsStart == channelsEnd)
	{
		LOG(ERROR "Channel not found ❌")
		return ;
	}
	LOG(DEBUG "Channel found ✅")

	// Now we want to know if the sender is in the said channel
	std::string							senderNickname = sender.getNickname();
	std::vector<std::string>			channelNicknames = channel->getMemberNames();
	std::vector<std::string>::iterator	channelNicknamesStart = channelNicknames.begin();
	std::vector<std::string>::iterator	channelNicknamesEnd = channelNicknames.end();

	while (channelNicknamesStart != channelNicknamesEnd)
	{
		if (*channelNicknamesStart == senderNickname)
			break;
		++channelNicknamesStart;
	}
	// Checks if the sender client is present in the said channel
	if (channelNicknamesStart == channelNicknamesEnd)
	{
		LOG(ERROR "User is not in the said channel ❌")
		return ;
	}

	// If there is not left over after the channel'name
	// we display the current topic to the user
	if (paramsStart == paramsEnd)
	{
		LOG(INFO "Displaying current topic to User")
		std::string	message = RPL_TOPIC(sender.getNickname(), channel->getName(), channel->getTopic());
		send(sender.getFd(), message.c_str(), message.size(), 0);
		return ;
	}

	// Get the new Topic formatted
	std::string	newTopic;

	while (paramsStart != paramsEnd && *paramsStart == ' ')	// Skips spaces if any
		++paramsStart;
	if (paramsStart != paramsEnd && *paramsStart == ':')	// Skips ':' if needed
		++paramsStart;
	while (paramsStart != paramsEnd)						// Add all chars to newTopic
	{
		newTopic += *paramsStart;
		++paramsStart;
	}

	// Broadcast the change to all clients in the channel
	LOG(INFO "Broadcasting new topic in " << channel->getName() << "...")
	std::string	broadcast = RPL_TOPIC_BROADCAST(sender.getNickname(), channel->getName(), newTopic);
	channel->broadcast(broadcast);

	// Now we can finally change the topic of the selected channel
	LOG(INFO "Sucessfully changed topic from \'" << channel->getTopic() << "\' to \'" << newTopic << "\' ✅")
	channel->setTopic(newTopic);
	std::string	numerical = RPL_TOPIC(sender.getNickname(), channel->getName(), newTopic);
	channel->broadcast(numerical);
	return ;
}