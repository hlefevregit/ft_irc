/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 10:22:27 by hugolefevre       #+#    #+#             */
/*   Updated: 2025/01/29 14:50:22 by hulefevr         ###   ########.fr       */
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
    
    std::cout << YELLOW << "[DEBUG]" << RESET << " Parsing message" << std::endl;
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
            if (!it->second.getReadBuffer().empty()) {
                std::string msg = it->second.getReadBuffer();
                it->second.setReadBuffer("");
                if (msg.find("\r\n") != std::string::npos)
                    msg.erase(msg.find("\r\n"));
                if (msg.find("\n") != std::string::npos)
                    msg.erase(msg.find("\n"));
                if (msg.find("\r") != std::string::npos)
                    msg.erase(msg.find("\r"));
                if (msg.empty())
                    return 1;
                
                std::cout << YELLOW << "[DEBUG]" << RESET << " Message: " << msg << std::endl;
                
                t_cmd cmd;
                int ret = parseCommand(msg, cmd);
                switch (ret)
                {
                    case 0:
                        break;
                    case 1:
                        sendToUserErr421(client->getFd(), cmd.command.c_str());
                        return 1;
                    default:
                        break;
                    
                }
                
                if (cmd.command == "JOIN")
                    joinCommand(it->second, cmd.params);
                else if (msg.find("USER") != std::string::npos)
                    changeUsername(it->second, cmd.params);
                else if (msg.find("PASS") != std::string::npos)
                    changePassword(it->second, cmd.params);
                else if (msg.find("NICK") != std::string::npos)
                    changeNickname(it->second, cmd.params);
                // else if (msg.find("INVITE") != std::string::npos)
                //     it->second.inviteToChannel(msg);
                // else if (msg == "LIST")
                //     it->second.listChannels();
                // else if (msg.find("KICK") != std::string::npos)
                //     it->second.kickFromChannel(msg);
                // else if (msg.find("MODE") != std::string::npos)
                //     it->second.changeMode(msg);
                // else if (msg.find("TOPIC") != std::string::npos)
                //     it->second.changeTopic(msg);
                // else if (msg.find("PART") != std::string::npos)
                //     it->second.leaveChannel(msg);
                // else if (msg.find("PING") != std::string::npos)
                //     it->second.ping(msg);
                // else if (msg.find("PRIVMSG") != std::string::npos)
                //     it->second.sendMessageToUser(msg);
                // else
                //     it->second.sendMessageToChannel(msg);

            }
            client->resetBuffer();
        }
    }
    return 0;
}