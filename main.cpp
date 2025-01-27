/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/10/09 14:01:19 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/27 16:07:57 by hugolefevre      ###   ########.fr       */
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

struct termios orig_termios;

void disable_echo() {
    struct termios new_termios;
    tcgetattr(STDIN_FILENO, &orig_termios); 
    new_termios = orig_termios;
    new_termios.c_lflag &= ~ECHOCTL;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &new_termios);
}

void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

static void	signal_handler(int signal)
{
	(void)signal;
    restore_terminal();
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

        disable_echo();
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