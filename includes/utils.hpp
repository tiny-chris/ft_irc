/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 18:25:05 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 18:43:39 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <cerrno>    // errno
# include <cstring>   // strerror
# include <iostream>  // cerr, cout
# include <sstream>   // stringstream
# include <string>    // string
# include <vector>    // vector

# include <arpa/inet.h>  // inet_ntoa
# include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
# include <poll.h>   // pollfd, poll
# include <unistd.h>  // close

enum commands {
	UNDEFINED = 0,

	CAP = 99,
	PASS = 100,
	NICK = 101,
	USER = 102,
	PING = 103,
	MODE = 109,
	JOIN = 110,
	PRIVMSG = 111,
	KICK = 112,
	TOPIC = 113,
	INVITE = 114,
	NAMES = 120,
	PART = 130,
	WHO = 131,

	ZZ_SHUTDOWN = 1000,
	ZZ_QUIT = 1001,
	ZZ_MSG = 1003,
};
// 	// NOTICE,
// 	// LIST,
// 	// INVITE,
// 	// PONG,
// 	// MODE,
// 	// WHO,
// 	// WHOIS,
// 	// WHOWAS,
// 	// QUIT,
// 	// KICK,
// 	// TOPIC,
// 	// OPER,
// 	// KILL

bool						isValidUser( std::string name );
bool						isValidParam( std::string name );
bool						isValidToken(std::string name);

std::string					intToString( int number );
int							StringToInt( std::string str );

std::vector<std::string>	splitString( std::string params, char splitter );
bool						splitStringInTwo( std::string strToSplit, char delimiter, std::string* str1, std::string* str2);

std::string					getCurrentDate( void );

#endif
