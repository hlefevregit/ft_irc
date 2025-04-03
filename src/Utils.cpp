/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:24:37 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/03 18:56:02 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

std::vector<pollfd>::iterator Server::findPollfdIterator(int fd, std::vector<pollfd> &pollfds)
{
	std::vector<pollfd>::iterator it = pollfds.begin();
	while (it != pollfds.end())
	{
		if (it->fd == fd)
			return it;
		++it;
	}
	return pollfds.end();
}

std::vector<std::string>	Server::splitByDelimiter(std::string input, char delimiter)
{
	std::vector<std::string>	result;
	std::string::iterator		inputStart = input.begin();
	std::string::iterator		inputEnd = input.end();
	std::string					line = "";

	while (inputStart != inputEnd)
	{
		if (*inputStart == delimiter)
		{
			result.push_back(line);
			line = "";
		}
		else
			line += *inputStart;
		++inputStart;
	}
	result.push_back(line);

	return (result);
}

std::map<const int, Client>::iterator	Server::getClientByNickname(const std::string &nickname)
{
	AUTO_LOG
	// Check if nickname correspond to a Client's nickname
	std::map<const int, Client>::iterator	start = _clients.begin();
	std::map<const int, Client>::iterator	end = _clients.end();

	while (start != end)
	{
		if (start->second.getNickname() == nickname)
		{
			LOG(INFO "Client " << nickname << " found !")
			return (start);
		}
		++start;
	}
	LOG(ERROR "Client nicknamed " << nickname << " not found amongs the clients !")
	return (end);
}