/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 19:38:50 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <cerrno>    // errno
#include <cstring>   // strerror
#include <iostream>  // cerr, cout
#include <sstream>   // stringstream
#include <string>    // string
#include <vector>    // vector

#include <arpa/inet.h>  // inet_ntoa
#include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
#include <poll.h>   // pollfd, poll
#include <unistd.h>  // close

enum commands
{
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
  QUIT = 1001,
  SQUIT = 1000,
  KILL = 1002
};

bool isValidUser( std::string name );
bool isValidParam( std::string name );
bool isValidToken( std::string name );

std::string intToString( int number );
int         StringToInt( std::string str );

bool vecStringsAllEmpty( const std::vector< std::string >& strings );
std::vector<std::string> splitString( std::string params, char splitter );
bool splitStringInTwo( std::string strToSplit, char delimiter,
                       std::string* str1, std::string* str2 );

std::string getCurrentDate( void );
char        getModePrefix( std::string const& token );

#endif
