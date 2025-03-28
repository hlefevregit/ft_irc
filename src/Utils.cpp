/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/27 18:24:37 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/28 10:49:36 by hulefevr         ###   ########.fr       */
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
