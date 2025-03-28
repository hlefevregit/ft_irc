/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 10:49:14 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/28 10:49:40 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"


/*********************************************************************/
/*********************************************************************/
/*****************************      BOT      *************************/
/*********************************************************************/
/*********************************************************************/

// void	Server::botHelp(Client sender)
// {
// 	std::string	message = GREEN "Hey " + sender.getUsername() + " I'm joe !\n"
// 	RESET "How to use :\n"
// 	"PRIVMSG joe stats" + YELLOW + " - Display various server statistics.\n" + RESET +
// 	"PRIVMSG joe user [nickname]" + YELLOW + " - Display informations about an user.\n" + RESET +
// 	"PRIVMSG joe channel [channel's name]" + YELLOW + " - Display informations about a channel.\n" + RESET
// 	;
// 	send(sender.getFd(), message.c_str(), message.size(), 0);
// 	return ;
// }

// void	Server::botParse(Client sender, std::string &params)
// {
// 	std::string				word;
// 	std::string::iterator	start = params.begin();
// 	std::string::iterator	end = params.end();

// 	std::cout << DEBUG << "┌─ IN  botParse ──────────────────────┐" << std::endl;

// 	// Check if msg is empty
// 	if (params.empty())
// 	{
// 		// std::string numerical = ERR_NEEDMOREPARAMS(PRIVMSG);
// 		// send(sender, numerical, numerical.size(), 0);  // TODO : use numerical correctly
// 		std::cerr << ERROR << "│  botParse: messsage is empty !      │" << RESET << std::endl;
// 		std::cout << DEBUG << "└─ OUT botParse ──────────────────────┘" << std::endl;
// 		return ;
// 	}
// 	// Skip leading spaces
// 	while (start != end && *start == ' ')
// 		++start;
// 	// Skip first word spaces
// 	while (start != end && *start != ' ')
// 		++start;
// 	// Skip spaces
// 	while (start != end && *start == ' ')
// 		++start;
// 	// Get second word
// 	while (start != end && *start != ' ')
// 	{
// 		word += *start;
// 		++start;
// 	}

// 	std::cout << DEBUG << "│  magic word : " << word << std::endl;

// 	// Check if command
// 	if (!word.empty())
// 	{
// 		if (word == "stats")
// 		{
// 			std::cout << DEBUG << "│  is stats command" << RESET << std::endl;
// 		}
// 		else if (word == "user")
// 		{
// 			std::cout << DEBUG << "│  is user command" << RESET << std::endl;
// 		}
// 		else if (word == "channel")
// 		{
// 			std::cout << DEBUG << "│  is channel command" << RESET << std::endl;
// 		}
// 		else
// 		{
// 			std::cout << DEBUG << "└─ OUT botParse ──────────────────────┘" << std::endl;
// 			return ;
// 		}
// 	}
// 	else
// 		botHelp(sender);

// 	std::cout << DEBUG << "└─ OUT botParse ──────────────────────┘" << std::endl;
// 	return ;
// }