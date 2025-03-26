/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parsing.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/27 10:22:27 by hugolefevre       #+#    #+#             */
/*   Updated: 2025/03/26 18:15:11 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

// static void splitMessage(std::vector<std::string> &cmds, std::string msg)
// {
// 	int pos = 0;
// 	std::string delimiter = "\n";
// 	std::string substr;

// 	while ((pos = msg.find(delimiter)) != static_cast<int>(std::string::npos))
// 	{
// 		substr = msg.substr(0, pos);
// 		cmds.push_back(substr);
// 		msg.erase(0, pos + delimiter.length());
// 	}
// }

static void splitMessage(std::vector<std::string> &cmds, std::string msg)
{
	std::cout << DEBUG << "┌─ IN  splitMessage ────────┐" << std::endl;
	std::string::iterator	start = msg.begin();

	while (start != msg.end())
	{
		std::string::iterator	end = std::find(start, msg.end(), '\n');
		std::string				command = std::string(start, end);
		command += '\n';
		cmds.push_back(command);

		if (end != msg.end())
			start = end + 1;
		else
			break;

		// std::cerr << DEBUG << "│  " << cmds.size() << " : " << command << std::endl;
	}
	
	std::cerr << DEBUG << "│  nb commands : " << cmds.size() << std::endl;
	std::cout << DEBUG << "└─ OUT splitMessage ────────┘" << std::endl;
	return ;
}

int	Server::parseMessage(Client *client, std::string const &message, std::vector<pollfd> &pollfds)
{
	std::vector<std::string>				cmds;
	std::map<const int, Client>::iterator	sender = _clients.find(client->getFd());
	
	std::cout << YELLOW << "[DEBUG]" << RESET << " Parsing message" << std::endl;
	std::cout << YELLOW << "[DEBUG]" << RESET << " WHOLE COMMAND\n" << message << std::endl;
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
			
			std::cout << YELLOW << "[DEBUG]" << RESET << " Message: " << msg << std::endl;
			t_cmd	cmd;
			int		ret = parseCommand(msg, cmd);
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

			// UNAUTHENTICATED USERS COMMANDS
			if (sender->second.isAuthenticated() == false)
			{
				if (msg.find("CAP LS 302") != std::string::npos)
					sendCapabilities(sender->second);
				else if (msg.find("USER") != std::string::npos)
					changeUsername(sender->second, cmd.params);
				else if (msg.find("TEST") != std::string::npos)
					testCommand(sender->second);
				else if (msg.find("PASS") != std::string::npos)
					connectToServerWithPass(sender->second, cmd.params);
				else if (msg.find("NICK") != std::string::npos)
					changeNickname(sender->second, cmd.params);
				else if (msg.find("QUIT") != std::string::npos)
					quitServer(sender->second, pollfds);
				client->resetBuffer();
			}
			// AUTHENTICATED USERS COMMANDS
			else
			{
				// sendAllUsers(msg, sender->second.getNickname());
				if (msg.find("CAP LS 302") != std::string::npos)
					sendCapabilities(sender->second);
				else if (msg.find("NICK") != std::string::npos)
					changeNickname(sender->second, cmd.params);
				// 	if (cmd.command == "JOIN")
				// 		joinCommand(sender->second, cmd.params);
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
				if (msg.find("PRIVMSG") != std::string::npos)
				{
					sendMessage(sender->second, cmd.params);
					// botParse(sender->second, cmd.params);
				}
				if (msg.find("QUIT") != std::string::npos)
					quitServer(sender->second, pollfds);
				// 	else if (msg.find("@joe") != std::string::npos || msg.find("/joe") != std::string::npos)
				// 		botParse(sender->second, cmd.params);
				// 		sender->second.sendMessage(sender->second, cmd.params);
			}
			client->resetBuffer();
		}
	}
	return 0;
}


// int    Server::parseMessage(Client *client, std::string const &message)
// {
//     std::vector<std::string> cmds;
//     std::map<const int, Client>::iterator it = _clients.find(client->getFd());
    
//     std::cout << YELLOW << "[DEBUG]" << RESET << " Parsing message" << std::endl;
//     splitMessage(cmds, message);
    
//     for (size_t i = 0; i != cmds.size(); i++)
//     {
//         if (it->second.isAuthenticated() == false)
//         {
//             if (it->second.authentification(client) == 0)
//                 return 1;
//             client->resetBuffer();
//         }
//         else
//         {
//             if (!it->second.getReadBuffer().empty()) {
//                 std::string msg = it->second.getReadBuffer();
//                 it->second.setReadBuffer("");
//                 if (msg.find("\r\n") != std::string::npos)
//                     msg.erase(msg.find("\r\n"));
//                 if (msg.find("\n") != std::string::npos)
//                     msg.erase(msg.find("\n"));
//                 if (msg.find("\r") != std::string::npos)
//                     msg.erase(msg.find("\r"));
//                 if (msg.empty())
//                     return 1;
                
//                 std::cout << YELLOW << "[DEBUG]" << RESET << " Message: " << msg << std::endl;
                
//                 t_cmd cmd;
//                 int ret = parseCommand(msg, cmd);
//                 switch (ret)
//                 {
//                     case 0:
//                         break;
//                     case 1:
//                         sendToUserErr421(client->getFd(), cmd.command.c_str());
//                         return 1;
//                     default:
//                         break;
                    
//                 }
                
//             //    sendAllUsers(msg, it->second.getNickname());
//                 // if (cmd.command == "JOIN")
//                 //     joinCommand(it->second, cmd.params);
//                 // if (msg.find("USER") != std::string::npos)
//                 //     changeUsername(it->second, cmd.params);
//                 // else if (msg.find("PASS") != std::string::npos)
//                 //     connectToServerWithPass(it->second, cmd.params);
//                 // else if (msg.find("NICK") != std::string::npos)
//                 //     changeNickname(it->second, cmd.params);
//                 // else if (msg.find("INVITE") != std::string::npos)
//                 //     it->second.inviteToChannel(msg);
//                 // else if (msg == "LIST")
//                 //     it->second.listChannels();
//                 // else if (msg.find("KICK") != std::string::npos)
//                 //     it->second.kickFromChannel(msg);
//                 // else if (msg.find("MODE") != std::string::npos)
//                 //     it->second.changeMode(msg);
//                 // else if (msg.find("TOPIC") != std::string::npos)
//                 //     it->second.changeTopic(msg);
//                 // else if (msg.find("PART") != std::string::npos)
//                 //     it->second.leaveChannel(msg);
//                 // else if (msg.find("PING") != std::string::npos)
//                 //     it->second.ping(msg);
//                 // else if (msg.find("PRIVMSG") != std::string::npos)
//                 // {
//                 //     sendMessage(it->second, cmd.params);
//                 //     botParse(it->second, cmd.params);
//                 // }
//                 // else if (msg.find("@joe") != std::string::npos || msg.find("/joe") != std::string::npos)
//                 //     botParse(it->second, cmd.params);
//                 //     it->second.sendMessage(it->second, cmd.params);

//             }
//             client->resetBuffer();
//         }
//     }
//     return 0;
// }