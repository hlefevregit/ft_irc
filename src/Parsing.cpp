/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 10:22:27 by hugolefevre       #+#    #+#             */
/*   Updated: 2025/01/27 12:22:27 by hugolefevre      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

static void splitMessage(std::vector<std::string> &cmds, std::string msg)
{
	int pos = 0;
	std::string delimiter = "\n";
	std::string substr;

	while ((pos = msg.find(delimiter)) != static_cast<int>(std::string::npos))
	{
		substr = msg.substr(0, pos);
		cmds.push_back(substr);
		msg.erase(0, pos + delimiter.length());
	}
}

int    Server::parseMessage(Client *client, std::string const &message)
{
    std::vector<std::string> cmds;
    std::map<const int, Client>::iterator it = _clients.find(client->getFd());
    
    splitMessage(cmds, message);
    
    for (size_t i = 0; i != cmds.size(); i++)
    {
        if (it->second.isAuthenticated() == false)
        {
            if (it->second.authentification(client) == 0)
                return 1;
            client->resetBuffer();
        }
        else
        {
            // std::cout << "\033[32m[INFO]\033[0m Message: " << cmds[i] << std::endl;
            //execCommand(client, cmds[i]);
            client->resetBuffer();
        }
    }
    return 0;
}