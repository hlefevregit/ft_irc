/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   leaveChannel.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/02 14:27:23 by ldalmass          #+#    #+#             */

/*   Updated: 2025/04/03 19:21:07 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void	Server::leaveChannel(Client &sender, std::string &params)
{
	AUTO_LOG

	// Extract all channels refered by the sender
	std::vector<std::string>			parameters = splitByDelimiter(params, ' ');
	std::vector<std::string>			channelsNames = splitByDelimiter(parameters[0], ',');
	std::vector<std::string>::iterator	channelsNamesStart = channelsNames.begin();
	std::vector<std::string>::iterator	channelsNamesEnd = channelsNames.end();

	// Check if the params are empty
	if (params.empty())
	{
		LOG(ERROR "Need more params !")
		std::string	numerical = ERR_NEEDMOREPARAMS(std::string("PART"));
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return;
	}

	// Get all channels the user want to leave in channelsNames
	std::map<std::string, Channel *>			channels = getChannels();
	std::map<std::string, Channel *>::iterator	channelsEnd = channels.end();
	std::vector<std::string>					selectedChannelsNames;

	while (channelsNamesStart != channelsNamesEnd)
	{
		std::map<std::string, Channel *>::iterator	channelsStart = channels.begin();
		while (channelsStart != channelsEnd)
		{
			if (*channelsNamesStart == channelsStart->second->getName())
			{
				LOG(INFO "Found and added " << *channelsNamesStart << " channel in channelsNamesStart")
				selectedChannelsNames.push_back(*channelsNamesStart);
			}
			++channelsStart;
		}
		++channelsNamesStart;
	}

	// Check if the sender is not part of one of the selected channels
	std::vector<std::string>::iterator	isMemberOfChannelStart = selectedChannelsNames.begin();
	std::vector<std::string>::iterator	isMemberOfChannelEnd = selectedChannelsNames.end();

	while (isMemberOfChannelStart != isMemberOfChannelEnd)
	{
		Channel *	channel = getChannel(*isMemberOfChannelStart);
	
		if (!channel->hasMember(sender.getFd()))
		{
			LOG(ERROR "User " << sender.getNickname() << " is not part of " << channel->getName() << " channel !")
			std::string	numerical = ERR_NOTONCHANNEL(channel->getName());
			send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
			return ;
		}
		++isMemberOfChannelStart;
	}

	// Get the optional leave message to broadcast to channels
	std::string	leaveBroadcast = "";

	if (parameters.size() == 2)
	{
		leaveBroadcast = parameters[1];
		if (parameters[1][0] == ':')	// Remove the ':' at the start
			leaveBroadcast.erase(leaveBroadcast.begin());
		LOG(INFO "Leave message : " << parameters[1])
	}

	// Remove sender from selected channels
	std::vector<std::string>::iterator	selectedChannelsStart = selectedChannelsNames.begin();
	std::vector<std::string>::iterator	selectedChannelsEnd = selectedChannelsNames.end();

	while (selectedChannelsStart != selectedChannelsEnd)
	{
		LOG(DEBUG "Leaving channel : " << *selectedChannelsStart)
		Channel	*selectedChannel = getChannel(*selectedChannelsStart);
		if (leaveBroadcast.empty())	// Case there is no leave message
		{
			LOG(INFO "CASE default")
			std::string	numerical = ":irc.ircserv.fr " + sender.getPrefix() + " PART " + selectedChannel->getName() + " :" + sender.getNickname() + " has left the channel\n";
			selectedChannel->broadcast(numerical);	// Use default leave message
		}
		else						// Case there is a leave message
		{
			LOG(INFO "CASE custom leaving message")
			std::string	numerical = sender.getPrefix() + " PART " + selectedChannel->getName() + " :" + leaveBroadcast + "\n";
			selectedChannel->broadcast(numerical);	// Use optional leave message
		}
		LOG(INFO "User " << sender.getNickname() << " leaved " << selectedChannel->getName())
		selectedChannel->removeMember(sender.getFd());
		++selectedChannelsStart;
	}

	return ;
}