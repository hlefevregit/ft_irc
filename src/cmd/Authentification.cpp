/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Authentification.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ldalmass <ldalmass@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 10:47:25 by hulefevr          #+#    #+#             */
/*   Updated: 2025/04/01 20:21:22 by ldalmass         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/Server.hpp"

/*********************************************************************/
/*********************************************************************/
/******************************    PASS    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::connectToServerWithPass(Client &sender, std::string &params)
{
	AUTO_LOG
	// Get PASS
	std::string::iterator	start = params.begin();
	std::string				pass = std::string(start, params.end());

	// Get position of the end of the pass
	size_t	endPos = 0;
	while (start != params.end())
	{
		if (*start == ' ' || *start == '\n' || *start == '\r')	// Break to separator
			break;
		++endPos;
		++start;
	}

	// Truncate pass until separator
	pass = pass.substr(0, endPos);

	// Last checks
	if (pass.empty())
	{
		LOG(ERROR "pass empty !")
		return ;
	}
	LOG(DEBUG "extracted pass : " << pass )

	// Authenticate user
	if (pass == _password)
	{
		LOG(INFO "Client #" << sender.getFd() << " successfully authentificated")
		sender.setPassword(pass);
		authenticateClient(sender);
	}
	else
	{
		std::string numerical = ERR_WRONGPASSWORD(SERVER_NAME);
		send(sender.getFd(), numerical.c_str(), numerical. size(), 0);
		LOG(ERROR "Client #" << sender.getFd() << " failed authentificate : Wrong password !")
	}
}

/*********************************************************************/
/*********************************************************************/
/******************************    NICK    ***************************/
/*********************************************************************/
/*********************************************************************/

bool	Server::isNicknameAvailable(std::string &nickname)
{
	std::map<const int, Client>::iterator	start = _clients.begin();
	std::map<const int, Client>::iterator	end = _clients.end();

	// Check if nickname correspond to a any Client's nickname
	while (start != end)
	{
		if (start->second.getNickname() == nickname)
			return false;
		++start;
	}
	return true;
}

void	Server::changeNickname(Client &sender, std::string &params)
{
	AUTO_LOG

	// Get nickname
	std::string::iterator	start = params.begin();
	std::string				nickname = std::string(start, params.end());

	// Get position of the end of the nickname
	size_t	endPos = 0;
	while (start != params.end())
	{
		if (*start == ' ' || *start == '\n' || *start == '\r')	// No more a word we can stop counting
			break;
		++endPos;
		++start;
	}
	endPos = endPos > 9 ? 9 : endPos;

	// Truncate nickname to fit 9 chars
	nickname = nickname.length() > 9 ? nickname.substr(0, endPos) : nickname;

	// Trim spaces
	start = nickname.begin();
	std::string::iterator	end = nickname.end();

	while (start != end && (*start == ' ' || *start == '\n' ||  *start == '\r'))
		++start;

	nickname = std::string(start, end);

	// Last checks
	if (nickname.empty())
	{
		std::string numerical = ERR_NONICKNAMEGIVEN;
		send(sender.getFd(), numerical.c_str(), numerical. size(), 0);
		LOG(ERROR "Nickname empty !")
		return ;
	}
	std::string::iterator	authorizedCharsStart = nickname.begin();
	std::string::iterator	authorizedCharsEnd = nickname.end();
	std::string				authorizedChars = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-[]\\^{}_";
	std::string				startWith = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
	// Checks first letter of nickname
	if (startWith.find(nickname[0]) == std::string::npos)
	{
		std::string numerical = ERR_ERRONEUSNICKNAME(nickname);
		send(sender.getFd(), numerical.c_str(), numerical. size(), 0);
		LOG(ERROR "Erroneus nickname !")
		return ;
	}
	// Checks the rest of the nickname
	++authorizedCharsStart;
	while (authorizedCharsStart != authorizedCharsEnd)
	{
		if (authorizedChars.find(*authorizedCharsStart) == std::string::npos)
		{
			std::string numerical = ERR_ERRONEUSNICKNAME(nickname);
			send(sender.getFd(), numerical.c_str(), numerical. size(), 0);
			LOG(ERROR "Erroneus nickname !")
			return ;
		}
		++authorizedCharsStart;
	}
	if (!isNicknameAvailable(nickname) || nickname == sender.getNickname())
	{
		std::string numerical = ERR_NICKNAMEINUSE(nickname);
		send(sender.getFd(), numerical.c_str(), numerical. size(), 0);
		LOG(ERROR "Nickname is already in use !")
		return ;
	}
	LOG(INFO "User #" << sender.getFd())
	LOG(INFO "Changed Nickname from " << sender.getNickname() << " to " << nickname)
	sender.setNickname(nickname);
	authenticateClient(sender);

	return ;
}

/*********************************************************************/
/*********************************************************************/
/******************************    USER    ***************************/
/*********************************************************************/
/*********************************************************************/

void	Server::changeUsername(Client &sender, std::string &params)
{
	AUTO_LOG

	// Get username
	std::string::iterator	start = params.begin();
	std::string				username = std::string(start, params.end());

	// Get position of the end of the username
	size_t	endPos = 0;
	while (start != params.end())
	{
		if (*start == ' ' || *start == '\n' || *start == '\r')	// No more a word we can stop counting
			break;
		++endPos;
		++start;
	}
	endPos = endPos > 9 ? 9 : endPos;

	// Truncate username to fit 9 chars
	username = username.length() > 9 ? username.substr(0, endPos) : username;

	// Last checks
	if (username.empty())
	{
		std::string numerical = ERR_NEEDMOREPARAMS(std::string("USER"));
		send(sender.getFd(), numerical.c_str(), numerical. size(), 0);
		LOG(ERROR "Nickname empty !")
		return ;
	}
	if (username == sender.getUsername())
	{
		LOG(INFO "User is already using this username")
		return ;
	}

	LOG(DEBUG "User #" << sender.getFd())
	LOG(INFO "Changed Username from " << sender.getUsername() << " to " << username )
	sender.setUsername(username);
	authenticateClient(sender);

	return ;
}
