/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:41:34 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/27 18:33:50 by hulefevr         ###   ########.fr       */
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
#include <sstream>

#include "Client.hpp"
#include "Channel.hpp"
#include "Colors.hpp"
#include "Command.hpp"
#include "numericalReplies.hpp"
#include "Grid.hpp"

#define SERVER_NAME "ircserv"

#define NICKNAME 0
#define USERNAME 1

#define MAX_CLIENT 15

extern volatile sig_atomic_t serverRunning;

class Client;
class Channel;

struct FdMatcher {
	int fd;

	FdMatcher(int fd) : fd(fd) {}

	bool operator()(const pollfd &pfd) const {
		return pfd.fd == fd;
	}
};

struct clientState {
	bool authenticated;
	int attempts;
};

class Server
{
private:	
	int								_serverSocket;
	std::string						_password;
	std::string						_datetime;
	std::vector<pollfd> 			_pollfds;
	std::map<const int, Client>		_clients;
	std::map<int, clientState>		_clientStates;
	std::map<std::string, Channel>	_channels;


public:
	Server(int port, const std::string &password, struct tm *timeinfo);
	~Server();

	void 	run();
	void 	shutDownServer();
	void 	setDatetime(struct tm *timeinfo);

	std::map<const int, Client>	&getClients();
	std::map<std::string, Channel>	&getChannels();


	int		acceptNewClient(std::vector<pollfd> &pollfds, std::vector<pollfd> &newPollfds);
	int		readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it);

	void	addClient(int clientSocket, std::vector<pollfd> &pollfds);
	void 	deleteClient(std::vector<pollfd> &pollfds, int fd);


	int		parseMessage(Client *client, const std::string &message, std::vector<pollfd> &pollfds);

	void	joinCommand(Client client, std::string &channelName);
	void	addClientToChannel(Client client, const std::string &channelName);
	
	void	changeNickname(Client client, std::string const &nickname);
	void	changeUsername(Client client, std::string const &username);
	void	changePassword(Client client, std::string const &password);

	void	sendAllUsers(const std::string &msg, const std::string &nickname);


	// ldalmass
	std::map<const int, Client>::iterator	getClientByNickname(const std::string &nickname);
	bool									isNicknameAvailable(std::string &nickname);

	int		sendMessage(Client sender, std::string &params);
	void	sendMessageUser(std::string &msg, const std::string &nickname, Client &sender);
	
	// void	botParse(Client sender, std::string &params);
	// void	botHelp(Client sender);

	void	sendCapabilities(Client &sender);
	void	testCommand(Client &sender);
	void	changeNickname(Client &sender, std::string &params);
	void	changeUsername(Client &sender, std::string &params);
	void	connectToServerWithPass(Client &sender, std::string &params);
	void	quitServer(Client &sender, std::vector<pollfd> &pollfds);
	void	authenticateClient(Client &sender);

	/*********************UTILS*********************************/

	std::vector<pollfd>::iterator findPollfdIterator(int fd, std::vector<pollfd> &pollfds);
};

void 	sendChillguy(int clientSocket);
void 	sendChad(int clientSocket);

std::string messageCleaner(char *buffer);

#endif