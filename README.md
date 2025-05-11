# ft_irc

<img src="https://github.com/hlefevregit/ft_irc/blob/main/Screenshot%20from%202025-04-30%2017-36-04.png"/>




# IRC Server (C++98)


An IRC (Internet Relay Chat) server developed in C++98, compliant with the IRC RFC 1459 protocol specification.  
This project was built and tested locally using the HexChat IRC client.



## 🚀 Getting Started

```bash
./ircserv <port> <password>
```

•	port: the port number on which the server listens for incoming connections
•	password: the password clients must provide to authenticate with the server



✅ Core Features


The server supports multiple simultaneous clients using non-blocking operations and a single poll() loop.
It communicates over TCP/IP (IPv4 or IPv6).



Implemented Commands and Behaviors:
•	Authentication: PASS, NICK, USER
•	Channel interaction: JOIN, PART
•	Private messaging: PRIVMSG
•	Channel operator commands:
•	KICK: remove a user from a channel
•	INVITE: invite a user to a channel
•	TOPIC: view or set a channel topic
•	MODE: manage channel modes:
  •	i: invite-only
  •	t: topic change restricted to operators
  •	k: channel password
  •	o: grant/remove operator status
  •	l: user limit
•	Broadcast messages to all users in a channel
•	Proper numeric replies following the IRC RFC
•	Error handling and command feedback



🤖 Integrated Bot

A lightweight bot is included to respond to specific commands:
•	List all channels
•	List all users
•	Display users in a specific channel


🧪 Reference Client

The server has been fully tested with HexChat, a popular IRC client.


🛠 Technical Constraints

•	Written in C++98
•	No forking or multithreading
•	Only one poll() for all I/O
•	Clean and modular codebase

## SOURCES

Everything you need to know about how we did this project is [here](https://www.figma.com/board/TEviHsREY663xd6BxfD1UQ/ft_irc?node-id=1-2&t=7nu0W9odqgLg8YLp-1) 

This is a figjam board, where you can see how the project is splitted. There is also a part LEXIQUE, where everything you need to know about IRC portocole and sockets uses are explained.

This is for educationnal purpose. Please don't copy and paste the code and search for other sources if you are doing this project. There is a credits part, that is where I learn everything, in a blue case on the bottom right of the canva
