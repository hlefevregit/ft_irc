/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Bot.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 10:49:14 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/07 18:27:31 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Bot.hpp"



void	Server::botParse(Client sender, std::string &params)
{
	AUTO_LOG

	LOG(INFO << "params: " << params)
	std::vector<std::string>	parameters = splitByDelimiter(params, ' ');
	if (parameters.size() < 2)
	{
		LOG(ERROR "Not enough parameters !")
		botHelp(sender);
		return ;
	}

	std::string	nickname = parameters[0];
	std::string	command = parameters[1];
	LOG(INFO << "nickname: " << nickname)
	LOG(INFO << "command: " << command)

	if (nickname != "joe")
	{
		LOG(ERROR "First word is not joe !")
		return ;
	}
	if (command == ":ascii" || command == "ascii")
		botGetRandomAsciiArt(sender);
	else if (command == ":list" || command == "list")
		botListConnectedClients(sender);
	else if (command == ":channels" || command == "channels")
		botListChannels(sender);
	else
		botHelp(sender);

	return ;
}

void	Server::botHelp(Client sender)
{
	AUTO_LOG

	std::vector<std::string>	lines;

	lines.push_back("Hey " + sender.getUsername() + " I'm joe !\r\n");
	lines.push_back("[ How to use ]\r\n");
	lines.push_back("for netcat  : PRIVMSG joe ascii\r\n");
	lines.push_back("for HexChat : /msg joe ascii\r\n");

	std::vector<std::string>::iterator	start = lines.begin();
	std::vector<std::string>::iterator	end = lines.end();

	while (start != end)
	{
		std::string	line = *start;
		std::string	message = 
			":joe!joe@localhost PRIVMSG "\
			+ sender.getNickname()\
			+ " :" + line + "\r\n";
		send(sender.getFd(), message.c_str(), message.size(), 0);
		++start;
	}

	return ;
}

void Server::botGetRandomAsciiArt(Client sender)
{
	AUTO_LOG

	// Get the list of ascii art files in the assets directory
	std::vector<std::string>	fileNames;
	DIR*						dir = opendir("./assets");
	struct dirent*				ent;

	// Check if the directory was opened successfully
	if (dir != NULL)
	{
		// Read the directory entries
		while ((ent = readdir(dir)) != NULL)
		{
			std::string	filename = ent->d_name;

			if (filename.length() > 7 && filename.substr(filename.length() - 7) == ".config")
			{
				std::string	fullPath = std::string("./assets/") + filename;
				fileNames.push_back(fullPath);
			}
		}
		closedir(dir);
	}
	else
	{
		LOG(ERROR "Failed to open assets directory");
		std::string	message = 
			":joe!joe@localhost PRIVMSG "\
			+ sender.getNickname()\
			+ " :I'm sooooo sorry but I don't have any ascii art at the moment :(\r\n";
		send(sender.getFd(), message.c_str(), message.size(), 0);
		return;
	}

	// Check if we found any ascii art files
	if (fileNames.empty())
	{
		LOG(ERROR "No ascii art files found");
		return;
	}

	// Select a random file from the list
	int	selectedIndex = 0;
	if (fileNames.size() > 1)
		selectedIndex = rand() % fileNames.size();

	// Open the selected file and send its content to the client
	std::fstream file(fileNames[selectedIndex].c_str(), std::ios::in);
	if (!file.is_open())
	{
		LOG(ERROR "Failed to open selected ascii art file");
		return;
	}
	LOG(INFO "Selecting ascii : " << fileNames[selectedIndex])

	// Read the file line by line and send it to the client
	std::string	line;
	while (std::getline(file, line))
	{
		std::string	message = 
			":joe!joe@localhost PRIVMSG "\
			+ sender.getNickname()\
			+ " :" + line + "\r\n";
		send(sender.getFd(), message.c_str(), message.size(), 0);
	}
	file.close();
}

void	Server::botListConnectedClients(Client sender)
{
	AUTO_LOG
	std::map<const int, Client>				clients = getClients();
	std::map<const int, Client>::iterator	start = clients.begin();
	std::map<const int, Client>::iterator	end = clients.end();
	t_cell_data								data;

	addCellData(data, "Nickname", CENTER);
	addCellData(data, "Username", CENTER);
	addCellData(data, "Socket", CENTER);

	while (start != end)
	{
		Client				client = start->second;
		std::ostringstream	oss;
	
		oss << client.getFd();
		if (!client.getNickname().empty())
			addCellData(data, client.getNickname(), LEFT);
		else
			addCellData(data, "No nickname", CENTER);
		if (!client.getUsername().empty())
			addCellData(data, client.getUsername(), LEFT);
		else
			addCellData(data, "No username", CENTER);
		addCellData(data, oss.str(), LEFT);
		++start;
		oss.clear();
	}

	unsigned short						y = clients.size() + 1;
	std::vector<std::string>			grid = getGrid(data, 60, 3, y);
	std::vector<std::string>::iterator	gridStart = grid.begin();
	std::vector<std::string>::iterator	gridEnd = grid.end();

	LOG(INFO "Clients connected : " << clients.size())
	LOG(INFO "Sent list of connected clients to " << sender.getNickname())
	while (gridStart != gridEnd)
	{
		std::string	message = 
			":joe!joe@localhost PRIVMSG "\
			+ sender.getNickname()\
			+ " :" + *gridStart;
		send(sender.getFd(), message.c_str(), message.size(), 0);
		++gridStart;
	}


	return ;
}

void	Server::botListChannels(Client sender)
{
	AUTO_LOG
	std::map<std::string, Channel*>				channels = getChannels();
	std::map<std::string, Channel*>::iterator	start = channels.begin();
	std::map<std::string, Channel*>::iterator	end = channels.end();	
	t_cell_data									data;

	addCellData(data, "Channels", CENTER);
	addCellData(data, "Topic", CENTER);

	while (start != end)
	{
		Channel				*channel = start->second;
	
		addCellData(data, channel->getName(), LEFT);
		if (!channel->getTopic().empty())
			addCellData(data, channel->getTopic(), LEFT);
		else
			addCellData(data, "No topic", CENTER);
		++start;
	}

	unsigned short						y = channels.size() + 1;
	std::vector<std::string>			grid = getGrid(data, 60, 2, y);
	std::vector<std::string>::iterator	gridStart = grid.begin();
	std::vector<std::string>::iterator	gridEnd = grid.end();

	LOG(INFO "Channels existing : " << channels.size())
	LOG(INFO "Sent list of existing channels to " << sender.getNickname())
	while (gridStart != gridEnd)
	{
		std::string	message = 
			":joe!joe@localhost PRIVMSG "\
			+ sender.getNickname()\
			+ " :" + *gridStart;
		send(sender.getFd(), message.c_str(), message.size(), 0);
		++gridStart;
	}

	return ;
}