/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   joinChannel.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 10:52:09 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/29 13:37:08 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/Channel.hpp"

bool checkChannelName(std::string &channelName)
{
	if (channelName[0] != '#' && channelName[0] != '&' && channelName[0] != '!' && channelName[0] != '+')
		return false;
	for (size_t i = 1; i < channelName.size(); i++)
	{
		if (channelName[i] == ' ')
			return false;
	}
	return true;
}

void	Server::joinCommand(Client client, std::string &channelName)
{
	if (_channels.find(channelName) == _channels.end())
	{
		if (checkChannelName(channelName) == false)
		{
			sendToUserErr403(client.getFd(), channelName.c_str());
			return ;
		}
		Channel channel(channelName);
		_channels.insert(std::pair<std::string, Channel>(channel.getName(), channel));
		std::cout << "\033[32m[INFO]\033[0m Channel " << channelName << " created" << std::endl;
	}
	addClientToChannel(client, channelName);
	
}
