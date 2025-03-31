/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 10:22:27 by hugolefevre       #+#    #+#             */
/*   Updated: 2025/03/31 15:47:23 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

static void splitMessage(std::vector<std::string> &cmds, std::string msg)
{
	AUTO_LOG
	// std::cout << INFO WALL IN << "splitMessage" << TRAIL << std::endl;
	std::string::iterator	start = msg.begin();

	while (start != msg.end())
	{
		std::string::iterator	end = std::find(start, msg.end(), '\n');
		std::string				command = std::string(start, end);
		LOG(DEBUG YELLOW << "Command #" << cmds.size() << " -> " << command << RESET)
		command += '\n';
		cmds.push_back(command);

		
		if (end != msg.end())
			start = end + 1;
		else
			break;

	}
	
	// std::cout << INFO WALL OUT << "splitMessage" << TRAIL << std::endl;
	return ;
}

int	Server::parseMessage(Client *client, std::string const &message, std::vector<pollfd> &pollfds)
{
	AUTO_LOG
	std::vector<std::string>				cmds;
	std::map<const int, Client>::iterator	sender = _clients.find(client->getFd());
	
	splitMessage(cmds, message);
	
	for (size_t i = 0; i != cmds.size(); i++)
	{
		sender->second.setReadBuffer(cmds[i]);
		if (!sender->second.getReadBuffer().empty())
		{
			std::string msg = sender->second.getReadBuffer();
			sender->second.setReadBuffer("");
			if (msg.find("\r\n") != std::string::npos)
				msg.erase(msg.find("\r\n"));
			if (msg.find("\n") != std::string::npos)
				msg.erase(msg.find("\n"));
			if (msg.find("\r") != std::string::npos)
				msg.erase(msg.find("\r"));
			if (msg.empty())
				return 1;
			
			t_cmd	cmd;
			int		ret = parseCommand(msg, cmd);
			switch (ret)
			{
				case 0:
					break;
				case 1:
					sendToUserErr421(client->getFd(), cmd.command.c_str());
					// client->resetBuffer();
					return 1;
				default:
					break;
			}

			// UNAUTHENTICATED USERS COMMANDS
			if (sender->second.isAuthenticated() == false)
			{
				if (cmd.command == "CAP" && cmd.params == "LS 302" )
					sendCapabilities(sender->second);
				else if (cmd.command == "USER")
					changeUsername(sender->second, cmd.params);
				else if (cmd.command == "TEST")
					testCommand(sender->second);
				else if (cmd.command == "PASS")
					connectToServerWithPass(sender->second, cmd.params);
				else if (cmd.command == "NICK")
					changeNickname(sender->second, cmd.params);
				else if (cmd.command == "QUIT")
					quitServer(sender->second, pollfds);
				// client->resetBuffer();
			}
			// AUTHENTICATED USERS COMMANDS
			else
			{
				// sendAllUsers(msg, sender->second.getNickname());
				if (cmd.command == "CAP" && cmd.params == "LS 302" )
					sendCapabilities(sender->second);
				else if (cmd.command == "NICK")
					changeNickname(sender->second, cmd.params);
				else if (cmd.command == "JOIN")
					this->joinCommand(sender->second, cmd.params);
				// 	else if (msg.find("INVITE") != std::string::npos)
				// 		sender->second.inviteToChannel(msg);
				// 	else if (msg == "LIST")
				// 		sender->second.listChannels();
				// 	else if (msg.find("KICK") != std::string::npos)
				// 		sender->second.kickFromChannel(msg);
				// 	else if (msg.find("MODE") != std::string::npos)
				// 		sender->second.changeMode(msg);
				// 	else if (msg.find("TOPIC") != std::string::npos)
				// 		sender->second.changeTopic(msg);
				// 	else if (msg.find("PART") != std::string::npos)
				// 		sender->second.leaveChannel(msg);
				// 	else if (msg.find("PING") != std::string::npos)
				// 		sender->second.ping(msg);
				else if (cmd.command == "PRIVMSG")
				{
					sendMessage(sender->second, cmd.params);
					// botParse(sender->second, cmd.params);
				}
				else if (cmd.command == "QUIT")
					quitServer(sender->second, pollfds);
				else if (cmd.command == "PASS")
				{
					std::string numerical = ERR_ALREADYREGISTRED;
					send(client->getFd(), numerical.c_str(), numerical. size(), 0);
				}
				else if (cmd.command == "USER")
				{
					std::string numerical = ERR_ALREADYREGISTRED;
					send(client->getFd(), numerical.c_str(), numerical. size(), 0);
				}
				// 	else if (msg.find("@joe") != std::string::npos || msg.find("/joe") != std::string::npos)
				// 		botParse(sender->second, cmd.params);
				// 		sender->second.sendMessage(sender->second, cmd.params);
			}
			client->resetBuffer();
		}
	}

	return 0;
}
