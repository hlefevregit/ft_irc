/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   printMessage.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/31 14:19:13 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/31 14:22:42 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

void	sendChillguy(int clientSocket)
{
	std::ifstream file;
	char		filePath[25] = "./assets/chillGuy.config";
	
	file.open(filePath);
	if (!file.is_open())
	{
		std::cerr << "\033[31m[ERROR]\033[0m Failed to open file" << std::endl;
		return ;
	}
	std::string line;
	while (std::getline(file, line))
	{
		send(clientSocket, line.c_str(), line.size(), 0);
		send(clientSocket, "\n", 1, 0);
	}
	file.close();
}

void sendChad(int clientSocket)
{
	std::ifstream file;
	char		filePath[21] = "./assets/chad.config";
	
	file.open(filePath);
	if (!file.is_open())
	{
		std::cerr << "\033[31m[ERROR]\033[0m Failed to open file" << std::endl;
		return ;
	}
	std::string line;
	while (std::getline(file, line))
	{
		send(clientSocket, line.c_str(), line.size(), 0);
		send(clientSocket, "\n", 1, 0);
	}
	file.close();
}