/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericalReplies.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/30 16:05:53 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/04 13:25:03 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

/*---------------INVITE-------------------*/

#define ERR_NEEDMOREPARAMS(cmd) "461 " + cmd + " :Not enough parameters\n"
#define ERR_NOSUCHNICK(nick) "401 " + nick + " :No such nick/channel\n"
#define ERR_NOTONCHANNEL(channel) "442 " + channel + " :You're not on that channel\n"
#define ERR_USERONCHANNEL(usr, channel) "443 " + usr + " " + channel + " :is already on channel\n"
#define RPL_INVITING(channel, nick) "341 " + nick + " " + channel + " :Inviting\n"
#define RPL_AWAY(nick, msg) "301 " + nick + " " + msg + " :away\n"
#define RPL_WELCOME(nick) ":irc.ircserv.fr 001 " + nick + " :Welcome to our IRC server\n"
#define RPL_YOURHOST(server) ":" server " 002 nickname :Your host is " server "\n"
#define RPL_CREATED(server, nick, host) ":" server " 003 " nick " :This server was created Sat Mar 28 2025 at 12:00:00 UTC"
#define RPL_MYINFO(nick, user, host) "003 " + nick + " :Welcome to our IRC server\n"

/*---------------JOIN-------------------*/

#define ERR_BADCHANNELKEY(channel) "475 " + channel + " :Cannot join channel (+k)\n"
#define ERR_CHANNELISFULL(channel) "471 " + channel + " :Cannot join channel (+l)\n"
#define ERR_INVITEONLYCHAN(channel)	"473 " + channel + " :Cannot join channel (+i)\n"
#define ERR_BANNEDFROMCHAN(channel) "474 " + channel + " :Cannot join channel (+b)\n"
#define ERR_NOSUCHCHANNEL(server, user, channel) ":" + server + " 403 " + user + " " + channel + " :No such channel\r\n"
#define ERR_TOOMANYCHANNELS(channel) "405 " + channel + " :You have joined too many channels\n"
#define ERR_TOOMANYTARGETS(target, errorCode, abortMessage) "407 " + target + " :" + errorCode + "Too many recipients. " + abortMessage + "\n"
#define ERR_UNAVAILRESOURCE(nick, channel) "437 " + nick + " " + channel + " :Nick/channel is temporarily unavailable\n"
#define ERR_BADCHANMASK(channel) "476 " + channel + " :Bad Channel Mask\n"

/*---------------KICK-------------------*/

#define ERR_CHANOPRIVSNEEDED(channel) "482 " + channel + " :You're not channel operator\n"
#define ERR_USERNOTINCHANNEL(nick, channel) "441 " + nick + " " + channel + " :They aren't on that channel\n"

/*---------------MODE-------------------*/

#define ERR_KEYSET(channel) "467 " + channel + " :Channel key already set\n"
#define ERR_NOCHANMODES(channel) "477 " + channel + " :Channel doesn't support modes\n"
#define ERR_UNKWOWNMODE(chare, channel) "472 " + chare + " :is unknown mode char to me for " + channel + "\n"
#define RPL_BANLIST(channel, mask) "367 " + channel + " " + mask + "\n"
#define RPL_ENDOFBANLIST(channel) "368 " + channel + " :End of channel ban list\n" 
#define RPL_EXCEPTLIST(channel, mask) "348 " + channel + " " + mask + "\n"
#define RPL_ENDOFEXCEPTLIST(channel) "349 " + channel + " :End of channel exception list\n"
#define RPL_INVITELIST(channel, mask) "346 " + channel + " " + mask + "\n"
#define RPL_ENDOFINVITELIST(channel) "347 " channel " :End of channel invite list\n"
#define RPL_UNIQOPIS(channel, nick) "325 " + channel + " " + nick + "\n"

/*---------------NICK-------------------*/

#define ERR_NICKNAMEINUSE(nick) ":irc.ircserv.fr 433 " + nick + " :Nickname is already in use\n"
#define ERR_NICKCOLLISION(nick, user, host) "436 " +  nick + " :Nickname collision KILL from " + user + "@" + host + "\n"
#define ERR_RESTRICTED "484 :Your connection is restricted!\n"
#define ERR_ERRONEUSNICKNAME(nick) "432 " + nick + " :Erroneus nickname\n"
#define ERR_NONICKNAMEGIVEN "431 :No nickname given\n"
#define NICK_CHANGE_ANNOUNCE(server, old_nick, new_nick) ":" + server + " " + old_nick + " NICK :" + new_nick + "\n"

/*---------------PING-------------------*/

#define ERR_NOORIGIN "409 :No origin specified\n"
#define ERR_NOSUCHSERVER(server) "402 " + server + " :No such server\n"

/*---------------PASS-------------------*/

#define ERR_WRONGPASSWORD(server) ":" server " 464 :Password incorrect\n"
#define ERR_ALREADYREGISTRED "462 :Unauthorized command (already registered)\n"

/*---------------PRIVMSG-----------------*/
#define ERR_NORECIPIENT(cmd) "411 " + cmd + " :No recipient given\n"
#define ERR_NOTEXTTOSEND "412 :No text to send"
#define ERR_NOTOPLEVEL(mask) "413 " + mask + " :No toplevel domain specified\n"
#define ERR_WILDTOPLEVEL(mask) "414 " + mask + " :Wildcard in toplevel domain\n"
#define ERR_CANNOTSENDTOCHAN(channel) "404 " + channel + " :Cannot send to channel\n"

/*---------------TOPIC-------------------*/

#define RPL_NOTOPIC(channel) "331 " + channel + " :No topic is set\n"
#define RPL_TOPIC(user, channel, topic) "332 " + user + " " + channel + " :" + topic + "\r\n"
#define RPL_TOPIC_BROADCAST(user, channel, topic) ":" + user + " TOPIC " + channel + " :" + topic + "\r\n"

/*---------------KICK--------------------*/
#define RPL_KICK_SUCCESS(server, channel, victim, reason) ":" + server + " KICK " + channel + " " + victim + " :" + reason + "\r\n"

#define ERR_UNKNOWNCOMMAND(cmd) "421 " + cmd + " :Unknown command\n"
void    sendToUserErr421(int fd, const char *cmd);
