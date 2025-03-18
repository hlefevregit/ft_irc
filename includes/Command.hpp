/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Command.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/29 10:43:04 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/18 16:34:58 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef COMMAND_HPP
#define COMMAND_HPP

#include "Server.hpp"
#include "Client.hpp"
#include "Channel.hpp"

class Client;

typedef struct	s_cmd {
	std::string prefix;
	std::string command;
	std::string params;
} 				t_cmd;

int		parseCommand(std::string const &message, t_cmd &cmd);

#endif