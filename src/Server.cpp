/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 13:46:11 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/31 22:26:14 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

/*********************************************************************/
/*********************************************************************/
/****************************SERV HANDLER*****************************/
/*********************************************************************/
/*********************************************************************/


Server::Server(int port, const std::string &password, struct tm *timeinfo) : _password(password) {
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	_serverName = SERVER_NAME;
	if (_serverSocket == -1)
		throw std::runtime_error("Error: Failed to create socket");

	sockaddr_in serverAddr;
	std::memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);
	int opt = 1;
	setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	if (bind(_serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
		throw std::runtime_error("Error: Failed to bind socket");
	if (listen(_serverSocket, MAX_CLIENT) < 0)
		throw std::runtime_error("Error: Failed to listen on socket");
	
	pollfd pfd;
	pfd.fd = _serverSocket;
	pfd.events = POLLIN;
	_pollfds.push_back(pfd);

	setDatetime(timeinfo);

	std::cout << "Server started on port " << port << std::endl;
}

Server::~Server() {
	shutDownServer();
}

void	Server::setDatetime(struct tm *timeinfo)
{
	char buffer[80];

	strftime(buffer,sizeof(buffer),"%d-%m-%Y %H:%M:%S",timeinfo);
  	std::string str(buffer);

	_datetime = str;
}

void Server::shutDownServer() {
	for (std::map<std::string, Channel*>::iterator it = _channels.begin(); it != _channels.end(); ++it)
	    delete it->second;
	_channels.clear();
	for (std::vector<pollfd>::iterator it = _pollfds.begin(); it != _pollfds.end(); ++it) {
		close(it->fd);
	}
	shutdown(_serverSocket, SHUT_RDWR);
	close(_serverSocket);
	std::cout << "Server shut down" << std::endl;
}

void Server::addClient(int clientSocket, std::vector<pollfd> &pollfds)
{
	AUTO_LOG
	Client		newClient(clientSocket);
	std::string	message;

	pollfd pfd;
	pfd.fd = clientSocket;
	pfd.events = POLLIN;

	/* Add new client to std::map<const int, Client> */
	LOG(INFO "New unregistered client added #" << clientSocket)
	_clients.insert(std::pair<int, Client>(clientSocket, newClient));
	pollfds.push_back(pfd);

	return ;
}

void Server::deleteClient(std::vector<pollfd> &pollfds, int fd) {
	AUTO_LOG
	LOG(INFO "Deconnecting client #" << fd)

	// 1. Fermer le socket
	close(fd);

	// 2. Supprimer le client du map
	if (_clients.find(fd) != _clients.end())
		_clients.erase(fd);

	// 3. Trouver l'itérateur pollfd et le supprimer
	std::vector<pollfd>::iterator it = findPollfdIterator(fd, pollfds);
	if (it != pollfds.end())
		pollfds.erase(it);
	else
		{ LOG(ERROR "pollfd for fd " << fd << " not found.") }

	LOG(INFO "Total clients: " << _clients.size())
}


int Server::acceptNewClient(std::vector<pollfd> &pollfds, std::vector<pollfd> &newPollfds)
{
	AUTO_LOG
	sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	int clientSocket = accept(_serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
	if (clientSocket < 0) {
		if (errno == EINTR) {
			LOG(INFO "Signal received")
			return 1;
		}
		LOG(ERROR "Failed to accept client")
		return 2;
	}
	
	if (pollfds.size() >= MAX_CLIENT) {
		LOG(ERROR "Server is full")
		std::string msg = "Server is full\n";
		send(clientSocket, msg.c_str(), msg.size(), 0);
		close(clientSocket);
	} else {
		addClient(clientSocket, newPollfds);
		LOG(INFO "New client connected #" << clientSocket)
		sendChillguy(clientSocket);}
	return 0;
	
}

std::map<const int, Client> &Server::getClients() { return (_clients); }

Client *Server::getClient(Server *server, int fd)
{
	std::map<const int, Client> &clients = server->getClients();
	std::map<const int, Client>::iterator it = clients.find(fd);
	
	if (it == clients.end()) {
		std::cerr << "\033[31m[ERROR]\033[0m Client not found" << std::endl;
		return NULL;
	}
	return &it->second;
}

int Server::readFromClient(std::vector<pollfd> &pollfds, std::vector<pollfd>::iterator &it) {
	int fd = it->fd;
	Client* client = getClient(this, fd);
	if (!client) {
		std::cerr << RED << "[ERROR] Client not found for fd " << fd << RESET << std::endl;
		deleteClient(pollfds, fd);
		return 3;
	}

	char buffer[4096];
	std::memset(buffer, 0, sizeof(buffer));
	int bytesRead = recv(fd, buffer, sizeof(buffer), 0);

	if (bytesRead <= 0) {
		if (bytesRead == 0)
			std::cout << GREEN << "[INFO] Client disconnected: " << fd << RESET << std::endl;
		else
			std::cerr << RED << "[ERROR] Failed to read from client " << fd << RESET << std::endl;

		deleteClient(pollfds, fd);
		return 3;
	}

	std::cout << GREEN << "[INFO] Received " << bytesRead << " bytes from client " << fd << RESET << std::endl;

	// ✅ Append only new data to read buffer
	std::string newData(buffer, bytesRead);
	client->appendToReadBuffer(newData);
	std::string totalBuffer = client->getReadBuffer();
	std::cout << "[DEBUG] Buffer accumulated: " << totalBuffer << std::endl;

	std::string::size_type pos;
	// std::cout << "BEFORE : '" << totalBuffer << "'" << std::endl;

	while ((pos = totalBuffer.find('\n')) != std::string::npos)
	{
		std::string command = totalBuffer.substr(0, pos);
		totalBuffer.erase(0, pos + 1);

		this->parseMessage(client, command, pollfds);
		client = getClient(this, fd);
		if (!client)
			return 3;
	}

	if (totalBuffer.empty())
		client->resetBuffer();
	// std::cout << "AFTER : '" << client->getReadBuffer() << "'" << std::endl;

	return 0;
}

void Server::run() {
	
	std::vector<pollfd> pollfds;
	pollfd serverPfd;
	serverPfd.fd = _serverSocket;
	serverPfd.events = POLLIN;

	pollfds.push_back(serverPfd);


	while (serverRunning == 1) {
		std::cout << std::endl << RED << "[INFO]" << RESET << " Server running = " << serverRunning << std::endl;
		std::vector<pollfd> newPollfds;
		
		if (poll((pollfd *)&pollfds[0], (unsigned int)pollfds.size(), -1) < 0) {
			if (errno == EINTR) {
				std::cout << GREEN << "[INFO]" << RESET << " Signal received" << std::endl;
				continue ;
			}
			std::cerr << "\033[31m[ERROR]\033[0m Failed to poll" << std::endl;
			break;
		}
		
		std::vector<pollfd>::iterator it = pollfds.begin();
		while (it != pollfds.end()) {
			if (it->revents & POLLIN)
			{
				if (it->fd == _serverSocket)
				{
					if (this->acceptNewClient(pollfds, newPollfds) == 2)
						continue ;
				}
				else
				{
					if (this->readFromClient(pollfds, it) == 3)
						break ;
				}
			}
			it++;
		}
		pollfds.insert(pollfds.end(), newPollfds.begin(), newPollfds.end());
	}
	return ;
}

void	Server::authenticateClient(Client &sender)
{
	AUTO_LOG
	LOG("sender nickname : " << sender.hasNickname())
	LOG("sender username : " << sender.hasUsername())
	LOG("sender password : " << sender.hasPassword())
	if (sender.hasNickname() && sender.hasUsername() && sender.hasPassword())
	{
		std::string	msg = RPL_WELCOME(sender.getNickname());
		send(sender.getFd(), msg.c_str(), msg.size(), 0);
		sendChad(sender.getFd());
		sender.authenticate();
	}
	if (sender.isAuthenticated() == true)
		{LOG(DEBUG "client #" << sender.getFd() << GREEN " is fully authentificated" RESET)}
	else
		{LOG(DEBUG "client #" << sender.getFd() << YELLOW " is not fully authentificated yet" RESET)}
	return ;
}


/*********************************************************************/
/*********************************************************************/
/******************************    QUIT    ***************************/
/*********************************************************************/
/*********************************************************************/

void Server::quitServer(Client &sender, std::vector<pollfd> &pollfds)
{
	AUTO_LOG

	int fd = sender.getFd();

	// Vérifier si le client est présent dans _clients
	if (_clients.find(fd) == _clients.end())
	{
		LOG(ERROR "The quitting client's fd is not present in our std::map!")
		return;
	}

	// Vérifier si le pollfd correspondant existe
	std::vector<pollfd>::iterator it = findPollfdIterator(fd, pollfds);
	if (it == pollfds.end())
	{
		LOG(ERROR "Could not find the matching fd in pollfds!")
		return;
	}

	// Fermer proprement le client
	deleteClient(pollfds, fd);
}


/*********************************************************************/
/*********************************************************************/
/******************************    TEST    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::testCommand(Client &sender)
{
	AUTO_LOG

	std::string	msg(10000000, 'A');
	send(sender.getSocket(), msg.c_str(), msg.size(), 0);

	send(sender.getSocket(), "B", 1, 0);
	return ;
}


/*********************************************************************/
/*********************************************************************/
/*****************************    CAP LS    **************************/
/*********************************************************************/
/*********************************************************************/

void	Server::sendCapabilities(Client &sender)
{
	AUTO_LOG
	LOG(DEBUG "CAP LS 302 triggered")

	// Return to HexChat that we do not have capabilities
	std::string msg = ":" SERVER_NAME " CAP * LS :\r\n";
	send(sender.getSocket(), msg.c_str(), msg.size(), 0);

	return ;

}

/*********************************************************************/
/*********************************************************************/
/**************************** GET CHANNELS  **************************/
/*********************************************************************/
/*********************************************************************/

std::map<std::string, Channel*>	&Server::getChannels() { return (_channels); }

/*********************************************************************/
/*********************************************************************/
/**************************** LIST CHANNELS **************************/
/*********************************************************************/
/*********************************************************************/

void	Server::printChannelList(void)
{
	AUTO_LOG
	t_cell_data									data;
	unsigned short								y = 1;
	std::map<std::string, Channel*>&			channels = getChannels();
	std::map<std::string, Channel*>::iterator	end = channels.end();

	addCellData(data, "Channel's name", CENTER);
	for (std::map<std::string, Channel*>::iterator start = channels.begin(); start != end; ++start)
	{
		addCellData(data, start->second->getName(), LEFT);
		y++;
	}
	printGrid(data, 40, 1, y);
	return ;
}

/*********************************************************************/
/*********************************************************************/
/*********************** LIST USERS IN A CHANNEL *********************/
/*********************************************************************/
/*********************************************************************/

void	Server::printUsersInChannel(std::string &params)
{
	AUTO_LOG
	Channel										channel;
	std::map<std::string, Channel*>&			channels = getChannels();
	std::map<std::string, Channel*>::iterator	start = channels.begin();
	std::map<std::string, Channel*>::iterator	end = channels.end();

	// Parsing
	std::string::iterator	pStart = params.begin();
	std::string::iterator	pEnd = params.end();
	while (pStart != pEnd)
	{
		if (*pStart == ' ')
			break;
		++pStart;
	}
	if (std::string(params.begin(), pStart) != "channel")
	{
		LOG(ERROR "Parsing error : " << params)
		LOG(ERROR "Expecting : PRINT channel [channel name]")
		return ;
	}
	LOG(DEBUG "Parsing : " << std::string(params.begin(), pStart))

	// Get channel name
	std::string	channelName;
	if (*pStart == ' ')
		++pStart;
	while (pStart != pEnd)
	{
		if (*pStart == ' ')
			break;
		channelName += *pStart;
		++pStart;
	}

	// Get asked channel
	while (start != end)
	{
		if ((*start).second->getName() == channelName)
		{
			LOG(DEBUG "Found channel : " << channelName)
			channel = *(*start).second; // Dereference the pointer before assigning it to channel
			break;
		}
		++start;
	}
	if (start == end)
	{
		LOG(ERROR "Channel not found amongs all of them !")
		return ;
	}

	// Get members names
	std::vector<std::string>			names = channel.getMemberNames();

	// Get operators members
	unsigned short						opNbr = 0;
	unsigned short						memberNbr = 0;
	std::vector<std::string>			op;
	std::vector<std::string>::iterator	opStart = names.begin();
	std::vector<std::string>::iterator	opEnd = names.end();

	while (opStart != opEnd)
	{
		// Get fd from a given nickname
		std::map<const int, Client>::iterator	nick = getClientByNickname(*opStart);
		int										fd = nick->second.getFd();
		// Checks if the fd is operator or not
		if (channel.isOperator(fd) == true)
		{
			opNbr++;
			op.push_back("Operator");
		}
		else
		{
			memberNbr++;
			op.push_back("Member");
		}
		++opStart;
	}

	// Display the  total number of members and operators
	t_cell_data	numbers;
	std::ostringstream oss;
	oss << memberNbr;
	std::string	members = oss.str();
	std::ostringstream osss;
	osss << opNbr;
	std::string	operators = osss.str();

	addCellData(numbers, "Total members", CENTER);
	addCellData(numbers, "Total operators", CENTER);
	addCellData(numbers, members + operators, LEFT);
	addCellData(numbers, operators, LEFT);
	printGrid(numbers, 50, 2, 2);

	// Display all members' names and roles
	t_cell_data	displayMembersName;
	unsigned short	membersCount = 0;
	std::vector<std::string>::iterator	namesStart = names.begin();
	std::vector<std::string>::iterator	namesEnd = names.end();
	std::vector<std::string>::iterator	operatorStart = op.begin();
	addCellData(displayMembersName, "Users in " + channelName, CENTER);
	addCellData(displayMembersName, "Roles", CENTER);
	while (namesStart != namesEnd)
	{
		addCellData(displayMembersName, *namesStart, LEFT);
		addCellData(displayMembersName, *operatorStart, LEFT);
		++namesStart;
		++operatorStart;
		++membersCount;
	}
	printGrid(displayMembersName, 50, 2, membersCount + 1);
	return ;
}