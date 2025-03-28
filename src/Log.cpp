/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Log.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 16:27:34 by ldalmass          #+#    #+#             */
/*   Updated: 2025/03/28 16:57:17 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Log.hpp"

int	Log::depth = 0;
Log::Log(const std::string &funcName) : functionName(funcName)
{
	++depth;
	for (int i = 1; i < depth; i++)
		std::cout << WALL;
	std::cout << IN << functionName << TRAIL << std::endl;
}

Log::~Log()
{
	for (int i = 1; i < depth; i++)
		std::cout << WALL;
	std::cout << OUT << functionName << TRAIL << std::endl;
	--depth;
}

void Log::log(const std::string &message)
{
	for (int i = 0; i < depth; i++)
		std::cout << WALL;
	std::cout << message << std::endl;
}