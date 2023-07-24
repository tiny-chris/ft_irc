/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 18:25:05 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 14:47:02 by cgaillag         ###   ########.fr       */
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
# include <stdlib.h>  // exit
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
	NAMES = 120,

	ZZ_SHUTDOWN = 1000,
	ZZ_QUIT = 1001,
	ZZ_MSG = 1003,
};
// 	// PART,
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

std::vector<std::string>	splitString( std::string params, char splitter );
std::string					getCurrentDate( void );

#endif
