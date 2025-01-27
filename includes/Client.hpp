/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hugolefevre <hugolefevre@student.42.fr>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:58 by hulefevr          #+#    #+#             */
/*   Updated: 2025/01/27 13:41:43 by hugolefevre      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "Server.hpp"

class Client {
private:
	int			_socket;
	bool		_authentificated;
	bool		_hasUsername;
	bool		_hasNickname;
	bool		_hasPassword;
	std::string _nickname;
	std::string	_username;
	std::string _readBuf;
	std::string _password;
public:
	Client(int socket);
	Client(const Client &src);
	Client &operator=(const Client &src);
	~Client();

	int getSocket() const;
	bool isAuthenticated() const;
	void authenticate();

	void	setNickname(const std::string &nickname) { _nickname = nickname; _hasNickname = true; };
	void	setUsername(const std::string &username) { _username = username; _hasUsername = true; };
	void	setPassword(const std::string &password) { _password = password; _hasPassword = true; };
	void	setReadBuffer(std::string const &buf);

	void	resetBuffer() { _readBuf.clear(); };

	bool	hasUsername() const { return _hasUsername; };
	bool	hasNickname() const { return _hasNickname; };

	int	authentification(Client *client);


	std::string getReadBuffer() const { return _readBuf; };
	std::string getNickname() const { return _nickname; };
	std::string getUsername() const { return _username; };
	std::string getPassword() const { return _password; };

	int getFd() const { return _socket; };
};

#endif