/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/01/23 14:05:58 by hulefevr          #+#    #+#             */
/*   Updated: 2025/03/31 22:13:17 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include <vector>
#include "Server.hpp"
#include "Channel.hpp"

class Channel;

class Client {
private:
	int						_socket;
	bool					_authentificated;
	bool					_hasUsername;
	bool					_hasNickname;
	bool					_hasPassword;
	std::string 			_nickname;
	std::string				_username;
	std::string 			_readBuf;
	std::string 			_password;

	std::vector<Channel*>	_joinedChannels;
	
public:
	Client(int socket);
	Client(const Client &src);
	Client &operator=(const Client &src);
	~Client();

	int				getSocket() const;
	bool			isAuthenticated() const;
	void			authenticate();
	
	void			setUsername(const std::string &username) { _username = username; _hasUsername = true; };
	void			setNickname(const std::string &nickname) { _nickname = nickname; _hasNickname = true; };
	void			setPassword(const std::string &password) { _password = password; _hasPassword = true; };
	void			setReadBuffer(std::string const &buf);
	
	void			resetBuffer() { _readBuf.clear(); };
	void 			appendToReadBuffer(const std::string& str) {_readBuf += str; };
	std::string&	getMutableReadBuffer() {return _readBuf;};


	bool			hasUsername() const { return _hasUsername; };
	bool			hasNickname() const { return _hasNickname; };
	bool			hasPassword() const { return _hasPassword; };
	
	int				authentification(Client *client);
	
	
	std::string 	getReadBuffer() const { return _readBuf; };
	std::string 	getNickname() const { return _nickname; };
	std::string 	getUsername() const { return _username; };
	std::string 	getPassword() const { return _password; };

	int				getFd() const { return _socket; };

	void 			joinedChannel(Channel* channel);
	void 			leaveChannel(const std::string& channelName);
	const 			std::vector<Channel*>& getJoinedChannels() const;

	/**********************************/
	/*************COMMANDS*************/
	/**********************************/

	void 			changeNickname(std::string const &nickname);
	void 			changeUsername(std::string const &username);
	void 			changePassword(std::string const &password);

	/**********************************/
	
	std::string 	getPrefix() const {
		return ":" + _nickname + "!" + _username + "@" + "localhost";
	}
	
	// ldalmass
	std::string		getDataSentByClientCleaned(int fd);
};

#endif