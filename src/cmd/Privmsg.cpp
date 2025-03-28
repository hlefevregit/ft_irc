/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Privmsg.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 10:50:02 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/28 19:13:20 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"


/*********************************************************************/
/*********************************************************************/
/*****************************    PRIVMSG    *************************/
/*********************************************************************/
/*********************************************************************/

std::map<const int, Client>::iterator	Server::getClientByNickname(const std::string &nickname)
{
	AUTO_LOG
	std::map<const int, Client>::iterator	start = _clients.begin();
	std::map<const int, Client>::iterator	end = _clients.end();

	// Check if nickname correspond to a Client's nickname
	while (start != end)
	{
		if (start->second.getNickname() == nickname)
			return (start);
		++start;
	}
	// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
	// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
	LOG(ERROR "getClientByNickname: nickname not found amongs clients !")
	return (start);
}

void	Server::sendMessageUser(std::string &msg, const std::string &nickname, Client &sender)
{
	AUTO_LOG
	std::map<const int, Client>::iterator	reciever = getClientByNickname(nickname);
	std::map<const int, Client>::iterator	client_end = _clients.end();

	/****************/
	/* Basic checks */
	/****************/

	// Check if the user is found in the server to send a message to
	if (reciever->first == client_end->first)
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		LOG(ERROR "sendMessageUser: client does not exist !")
		return ;
	}

	// Check if the sender is sending a PRIVMSG to itself
	if (sender.getNickname() == reciever->second.getNickname())
	{
		LOG(ERROR "sendMessageUser: cannot send a PRIVMSG to itself !")
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		return ;
	}

	/***********************************/
	/* Remove the targeted user in msg */
	/***********************************/
	std::string::iterator	start = msg.begin();
	std::string::iterator	end = msg.end();

	// Skips spaces
	while (start != end && *start == ' ')
		++start;
	// Skips first word
	while (start != end && *start != ' ')
		++start;
	// Skips spaces
	while (start != end && *start == ' ')
		++start;
	// Skips the semicolon ':' if needed
	if (start != end && *start == ':')
		++start;
	// Reform message
	std::string	cleaned_message(start, end);
	if (cleaned_message.empty())
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		std::cerr << ERROR << "sendMessageUser: cleaned_message is empty !" << RESET << std::endl;
		return ;
	}

	/************************/
	/* Send message to user */
	/************************/
	std::string	message = CYAN "Private message from [" + sender.getUsername() + "]: " + RESET + cleaned_message + "\n";
	message = AnsiToMircConverter().convertAnsiToMirc(message);
	send(reciever->first, message.c_str(), message.size(), 0);

	return ;
}

int    Server::sendMessage(Client sender, std::string &params)
{
	AUTO_LOG;
	std::string				channel_prefix = "&#+!";
	std::string				first_word;
	std::string::iterator	start = params.begin();
	std::string::iterator	end = params.end();


	// Check if msg is empty
	if (params.empty())
	{
		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
		LOG(ERROR "sendMessage: messsage is empty !")
		return 1;
	}
	// Skip leading spaces
	while (start != end && *start == ' ')
		++start;
	// Get first word
	while (start != end && *start != ' ')
	{
		first_word += *start;
		++start;
	}

	LOG(DEBUG "first word : " << first_word)

	// Checks first word's prefix
	if (first_word.size() > 1)
	{
		if (channel_prefix.find(first_word[0]) != std::string::npos)
		{
			LOG(DEBUG "  is Channel")
			// sendMessageChannel();    // TODO : dguerin implementation
		}
		else
		{
			LOG(DEBUG "  is User")
			sendMessageUser(params, first_word, sender);   // TODO : ldalmass implementation
		}
	}

	return 0;
}
