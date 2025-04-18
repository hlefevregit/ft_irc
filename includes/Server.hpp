/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:41:34 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/08 13:48:41 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <vector>
#include <map>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>
#include <iomanip>
#include <ctime>
#include <netdb.h>
#include <csignal>
#include <errno.h>
#include <fstream>


#include "Client.hpp"
#include "Channel.hpp"
#include "Colors.hpp"
#include "Command.hpp"
#include "numericalReplies.hpp"
#include "Grid.hpp"
#include "Log.hpp"

#define SERVER_NAME "ircserv"

#define NICKNAME 0
#define USERNAME 1

#define MAX_CLIENT 15

extern volatile sig_atomic_t serverRunning;

class Client;
class Channel;

struct FdMatcher
{
	int fd;

	FdMatcher(int fd) : fd(fd) {}

	bool operator()(const pollfd &pfd) const {
		return pfd.fd == fd;
	}
};

struct clientState
{
	bool authenticated;
	int attempts;
};

class Server
{
private:	

	int								_serverSocket;
	std::string						_serverName;
	std::string						_password;
	std::string						_datetime;
	std::vector<pollfd> 			_pollfds;
	std::map<const int, Client>		_clients;
	std::map<int, clientState>		_clientStates;
	std::map<std::string, Channel*>	_channels;


public:

	Server(int port, const std::string &password, struct tm *timeinfo);
	~Server();

	void			 						run();
	void				 					shutDownServer();
	void			 						setDatetime(struct tm *timeinfo);
		

	/******************************************************************/
	/******************************CLIENT******************************/
	/******************************************************************/

	int										acceptNewClient(std::vector<pollfd> &pollfds, std::vector<pollfd> &newPollfds);
	int										readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it);
	void									addClient(int clientSocket, std::vector<pollfd> &pollfds);
	void						 			deleteClient(std::vector<pollfd> &pollfds, int fd);

	std::map<const int, Client>				&getClients();
	std::map<std::string, Channel*>			&getChannels();
	Client									*getClient(Server *server, int fd);
	Client									*getClientsByNickname(const std::string &nickname);
		
	/****************************************************************/
	/****************************COMMANDS****************************/
	/****************************************************************/


	int										parseMessage(Client *client, const std::string &message, std::vector<pollfd> &pollfds);
		
	void									joinCommand(Client &client, const std::string &params);			
	// void									changeNickname(Client client, std::string const &nickname);
	// void									changeUsername(Client client, std::string const &username);
	void									changePassword(Client client, std::string const &password);
	void									inviteCommand(Client &inviter, const std::string &params);

	void									sendAllUsers(const std::string &msg, const std::string &nickname);

	void									sendCapabilities(Client &sender);
	void									testCommand(Client &sender);
	void									changeNickname(Client &sender, std::string &params);
	void									changeUsername(Client &sender, std::string &params);
	void									connectToServerWithPass(Client &sender, std::string &params);
	void									quitServer(Client &sender, std::vector<pollfd> &pollfds);
	void									authenticateClient(Client &sender);

	// ldalmass
	std::map<const int, Client>::iterator	getClientByNickname(const std::string &nickname);
	bool									isNicknameAvailable(std::string &nickname);

	int										sendMessage(Client sender, std::string &params);
	void									sendMessageUser(std::string &msg, const std::string &nickname, Client &sender);
	void									sendNumericReply(int fd, int code, const std::string& message);
	void									printChannelList(void);
	void									printUsersInChannel(std::string &params);
	void									changeTopic(Client &sender, std::string &params);
	void									leaveChannel(Client &sender, std::string &params);
	void									kickFromChannel(Client &sender, std::string &params);
	std::vector<std::string>				splitByDelimiter(std::string input, char delimiter);

	/***********************************************************/
	/****************************BOT****************************/
	/***********************************************************/

	void									botParse(Client sender, std::string &params);
	void									botHelp(Client sender);
	void									botGetRandomAsciiArt(Client sender);
	void									botListConnectedClients(Client sender);
	void									botListChannels(Client sender);
	void									botListChannelDetail(Client sender, std::string &channelName);

	/***********************************************************/
	/**************************CHANNEL**************************/
	/***********************************************************/

	Channel*								getChannel(const std::string &name);
	void 									sendMessageChannel(std::string &params, std::string &first_word,  Client &sender);
	void 									sendJoinReplies(Client &client, Channel &channel);
	std::string 							getOpList(Channel &channel);
	void									modeCommand(Client &client, const std::string &params);
	
	/***************************UTILS***************************/

	std::vector<pollfd>::iterator 			findPollfdIterator(int fd, std::vector<pollfd> &pollfds);
};

void 										sendChillguy(int clientSocket);
void 										sendChad(int clientSocket);

std::string 								messageCleaner(char *buffer);

#endif