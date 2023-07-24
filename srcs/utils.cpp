/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 18:23:45 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/24 10:37:40 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "utils.hpp"

/**
 * @brief       Int to String
 */

std::string intToString( int number ) {
  std::ostringstream oss;
  oss << number;
  return oss.str();
}

// /**
//  * @brief       Forbidden gai_strerror implementation
//  */

// std::string gaiStrerror( int errorCode ) {
//   switch( errorCode ) {
//     case 0:
//       return "Success";
//     case EAI_AGAIN:
//       return "Temporary failure in name resolution";
//     case EAI_BADFLAGS:
//       return "Invalid value for ai_flags";
//     case EAI_FAIL:
//       return "Non-recoverable failure in name resolution";
//     case EAI_FAMILY:
//       return "ai_family not supported";
//     case EAI_MEMORY:
//       return "Out of memory";
//     case EAI_NONAME:
//       return "Name or service not known";
//     case EAI_SERVICE:
//       return "Invalid value for service";
//     case EAI_SOCKTYPE:
//       return "ai_socktype not supported";
//     case EAI_SYSTEM:
//       return strerror( errno );
//     default:
//       return "Unknown error";
//   }
// }

/**
 * @brief       Split a string and store each substring into a vector element
 *              Delimiter is a specific mentionned 'char'
 */

std::vector<std::string>  splitString( std::string params, char splitter )
{
    std::vector<std::string>  tokens;// initially empty
    std::istringstream        iss( params );
    std::string               token;

    if ( !params.empty() )
    {
      while ( std::getline( iss, token, splitter ) )
      {
        tokens.push_back( token );
      }
      // // Helpers to check params
      // std::cout << "client - number of params before ':' is " << tokens.size() << std::endl;
      // for (size_t i = 0; i < tokens.size(); ++i)
      // {
      //   std::cout << "client - param[" << i << "] is <" << tokens[i] << ">" << std::endl;
      // }
    }
    return ( tokens );
}

/**
 * @brief       Check if the user name provided meets the requirements
 *              user  ::=  <sequence of any characters except NUL, CR, LF, and SPACE>
 *
 *
 * Software SHOULD use the UTF-8 character encoding to encode and decode messages, with fallbacks as described in the Character Encodings implementation considerations appendix.
 *
 *  REVOIR INCEPTION pour le utf...
 *
 */

bool	isValidUser( std::string name )
{
  for ( size_t i = 0; i < name.size(); i++ )
  {
    char c = name[ i ];
    if ( c == '\0' || c == '\r' || c == '\n' || c == ' ' )
      return false;
  }
  return true;
}

bool	isValidParam( std::string name )
{
  if ( name.empty() )
    return false;
  for ( size_t i = 0; i < name.size(); i++ )
  {
    char c = name[ i ];
    if ( !( isalnum(c) || c == '*' || c == '.' || c == '_' || c == '-' ) )
      return false;
  }
  return true;
}

bool    isValidToken(std::string name)
{
  if (name.empty())
    return false;
  for (size_t i = 0; i < name.size(); i++)
  {
    char c = name[i];
    if (!(isalnum(c) || c == '.' || c == '_' || c == '-'))
      return false;
  }
  return true;
}

// Returns a human readable string of the current date
std::string	getCurrentDate()
{
 	std::time_t time = std::time(0);
	std::stringstream	timeStream;
	timeStream << time;
	return timeStream.str();
}