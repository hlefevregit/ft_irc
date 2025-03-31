/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericalReplies.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 11:27:39 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/31 15:52:15 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/numericalReplies.hpp"
#include "../includes/Server.hpp"


void    sendToUserErr421(int fd, const char *cmd) {
    std::string msg = ERR_UNKNOWNCOMMAND(std::string(cmd));
    send(fd, msg.c_str(), msg.size(), 0);
}

void    sendToUserErr403(int fd, const char *channel) {
    std::string msg = ERR_NOSUCHCHANNEL(std::string(channel));
    send(fd, msg.c_str(), msg.size(), 0);
}

void Server::sendNumericReply(int fd, int code, const std::string& message)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " " << code << " " << getClient(this, fd)->getNickname() << " " << message << "\r\n";
	send(fd, oss.str().c_str(), oss.str().length(), 0);
}