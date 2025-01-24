/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hulefevr <hulefevr@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:58 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/23 16:41:40 by hulefevr         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
	int			_socket;
	bool		_authenticated;
	std::string _nickname;
	std::string	_username;
public:
	Client(int socket);
	~Client();

	int getSocket() const;
	bool isAuthenticated() const;
	void authenticate();

	void	setNickname(const std::string &nickname) { _nickname = nickname; };
	void	setUsername(const std::string &username) { _username = username; };

	std::string getNickname() const { return _nickname; };
	std::string getUsername() const { return _username; };

	int getFd() const { return _socket; };
};

#endif