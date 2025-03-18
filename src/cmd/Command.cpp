/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 11:05:55 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/18 15:26:02 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"
#include "../../includes/Server.hpp"

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
