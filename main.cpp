/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 14:01:19 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/29 14:28:18 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include "./includes/Server.hpp"
#include <termios.h>

volatile sig_atomic_t serverRunning = 1;


static void	signal_handler(int signal)
{
	(void)signal;
    serverRunning = 0;
}

int main(int ac, char **av)
{
    if (ac != 3) {
        std::cerr << "\033[1;31mUsage: ./ircserv <port> <password>\033[0m" << std::endl;
        return 1;
    }
    int port = std::atoi(av[1]);
    std::string password = av[2];
    
    if (port <= 0 || port > 65535) {
        std::cerr << "\033[1;31mInvalid port number\033[0m" << std::endl;
        return 1;
    }
    
    try {
        time_t rawtime;
		struct tm * timeinfo;

		time (&rawtime);
		timeinfo = localtime(&rawtime);
        
        Server server(port, password, timeinfo);
        signal(SIGINT, signal_handler);
        server.run();
    } catch (const std::exception &e) {
        std::cerr << "\033[1;31mError: " << e.what() << "\033[0m" << std::endl;
        return 1;
    }
    
    return 0;
}