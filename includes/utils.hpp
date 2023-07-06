/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 18:25:05 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/06 16:52:44 by codespace        ###   ########.fr       */
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
	MODE = 103,
	JOIN = 104,
	PRIVMSG = 105,

	ZZ_SHUTDOWN = 1000,
	ZZ_QUIT = 1001,
	ZZ_NAME = 1002,
	ZZ_MSG = 1003,
};
// 	// PART,
// 	// NOTICE,
// 	// NAMES,
// 	// LIST,
// 	// INVITE,
// 	// PING,
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

std::string intToString( int number );
std::string gaiStrerror( int errorCode );

#endif