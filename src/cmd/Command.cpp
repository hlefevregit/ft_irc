/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 11:05:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/27 22:29:35 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/Server.hpp"

int	parseCommand(std::string const &message, t_cmd &cmd)
{
    std::cout << INFO WALL IN << "parseCommand" << TRAIL << std::endl;
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
    
	std::cout << DEBUG WALL WALL YELLOW << "Prefix : " << cmd.prefix << RESET << std::endl;
	std::cout << DEBUG WALL WALL YELLOW << "Command: " << cmd.command << RESET << std::endl;
	std::cout << DEBUG WALL WALL YELLOW << "Params : " << cmd.params << RESET << std::endl;
    std::cout << INFO WALL OUT << "parseCommand" << TRAIL << std::endl;
    return 0; 
}
