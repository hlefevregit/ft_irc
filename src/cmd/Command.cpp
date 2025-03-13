/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 11:05:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/13 17:23:15 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
// #include "../../includes/Server.hpp"

int	parseCommand(std::string const &message, t_cmd &cmd)
{
	std::istringstream iss(message);
	std::string word;
	
	if (!message.empty() && message[0] == ':') {
        iss >> cmd.prefix;
        cmd.prefix = cmd.prefix.substr(1);
    }

    if (!(iss >> cmd.command))
        return 1;

    for (size_t i = 0; i < cmd.command.length(); ++i) {
        if (cmd.command[i] < 'A' || cmd.command[i] > 'Z') {
            return 1; 
        }
    }

    std::getline(iss, cmd.params);
    if (!cmd.params.empty() && cmd.params[0] == ' ') {
        cmd.params.erase(0, 1);
    }
    
	std::cout << YELLOW << "[DEBUG]" << RESET << " Prefix: " << cmd.prefix << std::endl;
	std::cout << YELLOW << "[DEBUG]" << RESET << " Command: " << cmd.command << std::endl;
	std::cout << YELLOW << "[DEBUG]" << RESET << " Params: " << cmd.params << std::endl;
    return 0; 
}

// int    sendMessage(Client sender, std::string &params)
// {
//     (void)sender;

// 	std::string				channel_prefix = "&#+!";
// 	std::string				first_word;
// 	std::string::iterator	start = params.begin();
// 	std::string::iterator	end = params.end();

// 	std::cout << DEBUG << "┌─ IN  sendMessage ───────────────────┐" << std::endl;

//     // Check if msg is empty
//     if (params.empty())
//     {
// 		std::cerr << ERROR << "│  sendMessage: messsage is empty !" << RESET << std::endl;
// 	    std::cout << DEBUG << "└─ OUT sendMessage ───────────────────┘" << std::endl;
//         // std::cerr << ERROR << ERR_NEEDMOREPARAMS << std::endl;   // TODO : use numerical correctly
//         return 1;
//     }
// 	// Skip leading spaces
// 	while (start != end && *start == ' ')
// 		++start;
// 	// Get first word
// 	while (start != end && *start != ' ')
// 	{
// 		first_word += *start;
// 		++start;
// 	}

// 	std::cout << DEBUG << "│  first word : " << first_word << std::endl;

// 	// Checks first word's prefix
// 	if (first_word.size() > 1)
// 	{
// 		if (channel_prefix.find(first_word[0]) != std::string::npos)
// 			std::cout << DEBUG << "│  is Channel" << RESET << std::endl;
// 			// sendMessageChannel();    // TODO : dguerin implementation
// 		else
// 			std::cout << DEBUG << "│  is User" << RESET << std::endl;
// 			// sendMessageUser();   // TODO : ldalmass implementation
// 	}

// 	std::cout << DEBUG << "└─ OUT sendMessage ───────────────────┘" << std::endl;
// 	return 0;
// }