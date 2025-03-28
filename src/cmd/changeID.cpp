/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   changeID.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 10:41:48 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/28 19:23:24 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Command.hpp"

// void   Server::changeNickname(Client client, std::string const &nickname)
// {
// 	if (nickname.empty())
// 	{
// 		std::string msg = ERR_NONICKNAMEGIVEN;
// 		send(client.getFd(), msg.c_str(), msg.size(), 0);
// 		return ;
// 	}
// 	std::map<const int, Client>::iterator it;
// 	for (it = _clients.begin(); it != _clients.end(); it++)
// 	{
// 		if (it->second.getNickname() == nickname)
// 		{
// 			std::string msg = ERR_NICKNAMEINUSE(nickname);
// 			send(client.getFd(), msg.c_str(), msg.size(), 0);
// 			return ;
// 		}
// 	}
// 	if (nickname.length() > 9)
// 	{
// 		std::string msg = ERR_ERRONEUSNICKNAME(nickname);
// 		send(client.getFd(), msg.c_str(), msg.size(), 0);
// 		return ;
// 	}
// 	if (nickname.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789[]\\`_^{|}-") != std::string::npos)
// 	{
// 		std::string msg = ERR_ERRONEUSNICKNAME(nickname);
// 		send(client.getFd(), msg.c_str(), msg.size(), 0);
// 		return ;
// 	}
// 	std::cout << YELLOW << "[INFO]" << RESET << " " << client.getNickname() << " changed nickname to " << nickname << std::endl;
// 	client.setNickname(nickname);
// 	std::string msg = "Successfully changed nickname to " + nickname + "\n";
// 	send(client.getFd(), msg.c_str(), msg.size(), 0);
// }

// void   Server::changeUsername(Client client, std::string const &username)
// {
// 	if (username.empty())
// 	{
// 		std::string tmp = "USER";
// 		std::string msg = ERR_NEEDMOREPARAMS(tmp);
// 		send(client.getFd(), msg.c_str(), msg.size(), 0);
// 		return ;
// 	}
// 	std::cout << YELLOW << "[INFO]" << RESET << " " << client.getNickname() << " changed username to " << username << std::endl;
// 	client.setUsername(username);
// 	std::string msg = "Successfully changed username to " + username + "\n";
// 	send(client.getFd(), msg.c_str(), msg.size(), 0);
// }

// void   Server::changePassword(Client client, std::string const &password)
// {
// 	if (password.empty())
// 	{
// 		std::string tmp = "PASS";
// 		std::string msg = ERR_NEEDMOREPARAMS(tmp);
// 		send(client.getFd(), msg.c_str(), msg.size(), 0);
// 		return ;
// 	}
// 	std::cout << YELLOW << "[INFO]" << RESET << " " << client.getNickname() << " changed password" << std::endl;
// 	client.setPassword(password);
// 	std::string msg = "Successfully changed password\n";
// 	send(client.getFd(), msg.c_str(), msg.size(), 0);
// }
 