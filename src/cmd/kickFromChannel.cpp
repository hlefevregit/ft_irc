/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   kickFromChannel.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/03 16:31:20 by ldalmass          #+#    #+#             */
/*   Updated: 2025/04/03 19:12:59 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

void	Server::kickFromChannel(Client &sender, std::string &params)
{
	AUTO_LOG

	// Get all params
	std::vector<std::string>			parameters = splitByDelimiter(params, ' ');
	std::vector<std::string>::iterator	paramsStart = parameters.begin();
	std::vector<std::string>::iterator	paramsEnd = parameters.end();

	LOG(DEBUG "options.size() : " << parameters.size())
	while (paramsStart != paramsEnd)
		{ LOG(DEBUG "options : " << *paramsStart++) }

	// Checks for minimum parameters needed
	if (parameters.size() < 2)
	{
		LOG(ERROR "Missing parameters !")
		std::string	numerical = ERR_NEEDMOREPARAMS(std::string("KICK"));
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return ;
	}

	// Verify asked exists among all the channels
	Channel *									channel;
	std::map<std::string, Channel *>			channels = getChannels();
	std::map<std::string, Channel *>::iterator	channelsStart = channels.begin();
	std::map<std::string, Channel *>::iterator	channelsEnd = channels.end();

	while (channelsStart != channelsEnd)
	{
		if (channelsStart->second->getName() == parameters[0])
		{
			LOG(INFO "Channel " << parameters[0] << " found !")
			channel = getChannel(parameters[0]);
			break;
		}
		++channelsStart;
	}
	if (channelsStart == channelsEnd)
	{
		LOG(ERROR "There is no " << parameters[0] << " channel !")
		std::string	numerical = ERR_NOSUCHCHANNEL(
			std::string(SERVER_NAME),\
			sender.getNickname(),\
			parameters[0]);
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return ;
	}

	// Check that the sender is part of the said channel
	if (!channel->hasMember(sender.getFd()))
	{
		LOG(ERROR << sender.getNickname() << " is not member of " << channel->getName() << " !")
		std::string	numerical = ERR_NOTONCHANNEL(channel->getName());
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return ;
	}

	// Check that the sender is operator in the channel
	if (!channel->isOperator(sender.getFd()))
	{
		LOG(ERROR << sender.getNickname() << " is not an operator of " << channel->getName() << " !")
		std::string	numerical = ERR_CHANOPRIVSNEEDED(channel->getName());
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return ;
	}

	// Get the victim to KICK if it exist in the std::map
	std::map<const int, Client>&			clients = getClients();
	std::map<const int, Client>::iterator	clientEnd = clients.end();
	std::map<const int, Client>::iterator	victim = getClientByNickname(parameters[1]);

	if (victim == clientEnd)
	{
		LOG(ERROR "targeted victim " << parameters[1] << " does not exist !")
		std::string	numerical = ERR_NOSUCHNICK(parameters[1]);
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return ;
	}

	// Check if the victim is present in the said channel
	if (!channel->hasMember(victim->second.getFd()))
	{
		LOG(ERROR "targeted victim " << parameters[1] << " is not a member of " << channel->getName() << " !")
		std::string	numerical = ERR_USERNOTINCHANNEL(
			victim->second.getNickname(),\
			channel->getName());;
		send(sender.getFd(), numerical.c_str(), numerical.size(), 0);
		return ;
	}

	// Get the optionnal reason
	std::string							reason = "No reason provided";
	std::vector<std::string>::iterator	start = parameters.begin() + 2;	// To start at third argument
	std::vector<std::string>::iterator	end = parameters.end();
	if (parameters.size() > 2)
	{
		reason.clear();
		while (start != end)
		{
			std::string::iterator	reasonStart = start->begin();
			std::string::iterator	reasonEnd = start->end();

			while (reasonStart != reasonEnd)
			{
				reason += *reasonStart;
				++reasonStart;
			}
			if (reasonStart != reasonEnd)	// Append a space in between argument unless its the last argument
				reason += ' ';
			++start;
		}
	}
	if (reason[0] == ':')
		reason.erase(reason.begin());

	// KICK the victim
	std::string	numerical = RPL_KICK_SUCCESS(
		std::string(SERVER_NAME),\
		channel->getName(),\
		victim->second.getNickname(),\
		reason);
	channel->broadcast(numerical);
	LOG(INFO "targeted victim " << parameters[1] << " has been kicked from " << channel->getName() << " by " << sender.getNickname() << " !")
	channel->removeMember(victim->second.getFd());

	return ;
}