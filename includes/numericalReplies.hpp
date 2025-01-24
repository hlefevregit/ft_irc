/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericalReplies.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42nice.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 16:05:53 by hulefevr          #+#    #+#             */
/*   Updated: 2024/10/30 17:07:11 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*---------------INVITE-------------------*/

#define ERR_NEEDMOREPARAMS(cmd) "461 " cmd " :Not enough parameters"
#define ERR_NOSUCHNICK(nick) "401 " nick " :No such nick/channel"
#define ERR_NOTONCHANNEL(channel) "442 " channel " :You're not on that channel"
#define ERR_USERONCHANNEL(usr, channel) "443 " usr " " channel " :is already on channel"
#define RPL_INVITING(channel, nick) "341 " nick " " channel " :Inviting"
#define RPL_AWAY(nick, msg) "301 " nick " " msg " :away"

/*---------------JOIN-------------------*/

#define ERR_BADCHANNELKEY(channel) "475 " channel " :Cannot join channel (+k)"
#define ERR_CHANNELISFULL(channel) "471 " channel " :Cannot join channel (+l)"
#define ERR_INVITEONLYCHAN(channel)	"473 " channel " :Cannot join channel (+i)"
#define ERR_BANNEDFROMCHAN(channel) "474 " channel " :Cannot join channel (+b)"
#define ERR_NOSUCHCHANNEL(channel) "403 " channel " :No such channel"
#define ERR_TOOMANYCHANNELS(channel) "405 " channel " :You have joined too many channels"
#define ERR_TOOMANYTARGETS(target, errorCode, abortMessage) "407 " target " :" errorCode "Too many recipients. " abortMessage
#define ERR_UNAVAILRESOURCE(nick, channel) "437 " nick " " channel " :Nick/channel is temporarily unavailable"
#define RPL_TOPIC(channel, topic) "332 " channel " :" topic
#define ERR_BADCHANMASK(channel) "476 " channel " :Bad Channel Mask"

/*---------------KICK-------------------*/

#define ERR_CHANOPRIVSNEEDED(channel) "482 " channel " :You're not channel operator"
#define ERR_USERNOTINCHANNEL(nick, channel) "441 " nick " " channel " :They aren't on that channel"

/*---------------MODE-------------------*/

#define ERR_KEYSET(channel) "467 " channel " :Channel key already set"
#define ERR_NOCHANMODES(channel) "477 " channel " :Channel doesn't support modes"
#define ERR_UNKWOWNMODE(chare, channel) "472 " chare " :is unknown mode char to me for " channel
#define RPL_BANLIST(channel, mask) "367 " channel " " mask
#define RPL_ENDOFBANLIST(channel) "368 " channel " :End of channel ban list" 
#define RPL_EXCEPTLIST(channel, mask) "348 " channel " " mask
#define RPL_ENDOFEXCEPTLIST(channel) "349 " channel " :End of channel exception list"
#define RPL_INVITELIST(channel, mask) "346 " channel " " mask
#define RPL_ENDOFINVITELIST(channel) "347 " channel " :End of channel invite list"
#define RPL_UNIQOPIS(channel, nick) "325 " channel " " nick

/*---------------NICK-------------------*/

#define ERR_NICKNAMEINUSE(nick) "433 " nick " :Nickname is already in use"
#define ERR_NICKCOLLISION(nick, user, host) "436 " nick " :Nickname collision KILL from " user "@" host
#define ERR_RESTRICTED "484 :Your connection is restricted!"
#define ERR_ERRONEUSNICKNAME(nick) "432 " nick " :Erroneus nickname"
#define ERR_NONICKNAMEGIVEN "431 :No nickname given"

/*---------------PING-------------------*/

#define ERR_NOORIGIN "409 :No origin specified"
#define ERR_NOSUCHSERVER(server) "402 " server " :No such server"

/*---------------PASS-------------------*/

#define ERR_ALREADYREGISTRED "462 :Unauthorized command (already registered)"

/*---------------PRIVMSG-------------------*/

#define ERR_NORECIPIENT(cmd) "411 " cmd " :No recipient given"
#define ERR_NOTEXTTOSEND "412 :No text to send"
#define ERR_NOTOPLEVEL(mask) "413 " mask " :No toplevel domain specified"
#define ERR_WILDTOPLEVEL(mask) "414 " mask " :Wildcard in toplevel domain"
#define ERR_CANNOTSENDTOCHAN(channel) "404 " channel " :Cannot send to channel"

/*---------------TOPIC-------------------*/

#define RPL_NOTOPIC(channel) "331 " channel " :No topic is set"