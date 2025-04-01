/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericalReplies.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 11:27:39 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/01 20:58:39 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/numericalReplies.hpp"
#include "../includes/Server.hpp"


void    sendToUserErr421(int fd, const char *cmd) {
    std::string msg = ERR_UNKNOWNCOMMAND(std::string(cmd));
    send(fd, msg.c_str(), msg.size(), 0);
}

void Server::sendNumericReply(int fd, int code, const std::string& message)
{
	std::ostringstream oss;
	oss << ":" << SERVER_NAME << " " << code << " " << getClient(this, fd)->getNickname() << " " << message << "\r\n";
	send(fd, oss.str().c_str(), oss.str().length(), 0);
}