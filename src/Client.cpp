/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:50 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/23 16:39:55 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client(int socket) : _socket(socket), _authenticated(false), _nickname(""), _username("") {}

Client::~Client() {}

int Client::getSocket() const {
	return _socket;
}

bool Client::isAuthenticated() const {
	return _authenticated;
}

void Client::authenticate() {
	_authenticated = true;
}