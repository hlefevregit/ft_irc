/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Invite.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 11:56:56 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/04 12:41:34 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"
#include "../../includes/Client.hpp"
#include "../../includes/Channel.hpp"

void Server::inviteCommand(Client& inviter, const std::string& params)
{
	AUTO_LOG
    std::istringstream iss(params);
    std::string nick, chanName;
    iss >> nick >> chanName;

    if (nick.empty() || chanName.empty()) {
		LOG(INFO "Client " << inviter.getNickname() << " failed to invite (not enough parameters)")
        sendNumericReply(inviter.getFd(), 461, "INVITE :Not enough parameters");
        return;
    }

    Client* invited = getClientsByNickname(nick);
    if (!invited) {
		LOG(INFO "Client " << inviter.getNickname() << " failed to invite " << nick << " (no such nick)")
        sendNumericReply(inviter.getFd(), 401, nick); // No such nick
        return;
    }

    Channel* channel = getChannel(chanName);
    if (!channel) {
		LOG(INFO "Client " << inviter.getNickname() << " failed to invite " << nick << " to channel " << chanName << " (no such channel)")
        sendNumericReply(inviter.getFd(), 403, chanName); // No such channel
        return;
    }

    if (!channel->hasMember(inviter)) {
		LOG(INFO "Client " << inviter.getNickname() << " failed to invite " << nick << " to channel " << chanName << " (not on channel)")
        sendNumericReply(inviter.getFd(), 442, chanName); // You're not on that channel
        return;
    }

    if (channel->hasMember(*invited)) {
		LOG(INFO "Client " << inviter.getNickname() << " failed to invite " << nick << " to channel " << chanName << " (already on channel)")
        sendNumericReply(inviter.getFd(), 443, nick + " " + chanName + " :is already on channel");
        return;
    }

    // Add to invited list
    channel->inviteClient(*invited);

    // Notify inviter
    std::string numeric = "341 " + inviter.getNickname() + " " + invited->getNickname() + " " + chanName + "\r\n";
    send(inviter.getFd(), numeric.c_str(), numeric.length(), 0);

    // Notify invited client
    std::string inviteMsg = ":" + inviter.getPrefix() + " INVITE " + invited->getNickname() + " :" + chanName + "\r\n";
    send(invited->getFd(), inviteMsg.c_str(), inviteMsg.length(), 0);
	LOG(INFO "Client " << inviter.getNickname() << " invited " << nick << " to channel " << chanName)
}
