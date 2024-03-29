/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/01 15:58:57 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "utils.hpp"

/**
 * @brief       Int to String
 */

std::string intToString( int number ) {
  std::ostringstream oss;
  oss << number;
  return oss.str();
}

/**
 * @brief       String to Int
 */

int StringToInt( std::string str ) {
  std::stringstream iss( str );
  int               number;
  iss >> number;
  return number;
}

/**
 * @brief       Split a string and store each substring into a vector element
 *              Delimiter is a specific mentionned 'char'
 */

std::vector<std::string> splitString( std::string params, char splitter ) {
  std::vector<std::string> tokens;  // initially empty
  std::istringstream       iss( params );
  std::string              token;

  if( !params.empty() ) {
    while( std::getline( iss, token, splitter ) ) {
      tokens.push_back( token );
    }
  }
  return ( tokens );
}

/**
 * @brief       Splits a string into two and store each substring into str1 
 *              and str2 Delimiter is a specific mentionned 'char'
 */

bool splitStringInTwo( std::string strToSplit, char delimiter,
                       std::string* str1, std::string* str2 ) {
  size_t splitter = strToSplit.find( delimiter, 0 );

  if( strToSplit.empty() ) {
    return false;
  }
  if( splitter != std::string::npos ) {
    *str1 = strToSplit.substr( 0, splitter );
    *str2 = strToSplit.substr( splitter + 1 );
  } else {
    *str1 = strToSplit;
    *str2 = "";
  }
  return true;
}

/**
 * @brief       Checks if all string from a vector<std::string> are empty
 */

bool vecStringsAllEmpty( const std::vector<std::string>& strings ) {
  std::vector<std::string>::const_iterator it;
  for ( it = strings.begin(); it != strings.end(); ++it ) {
    if ( !it->empty() ) {
        return false;
    }
  }
  return true;
}

/**
 * @brief       Check if the user name provided meets the requirements
 *              user  ::=  <sequence of any characters except NUL, CR, LF, 
 *              and SPACE>
 */

bool isValidUser( std::string name ) {
  for( size_t i = 0; i < name.size(); i++ ) {
    char c = name[i];
    if( c == '\0' || c == '\r' || c == '\n' || c == ' ' )
      return false;
  }
  return true;
}

bool isValidParam( std::string name ) {
  if( name.empty() ) {
    return false;
  }
  for( size_t i = 0; i < name.size(); i++ ) {
    char c = name[i];
    if( !( isalnum( c ) || c == '*' || c == '.' || c == '_' || c == '-' ) ) {
      return false;
    }
  }
  return true;
}

bool isValidToken( std::string name ) {
  if( name.empty() ) {
    return false;
  }
  for( size_t i = 0; i < name.size(); i++ ) {
    char c = name[i];
    if( !( isalnum( c ) || c == '.' || c == '_' || c == '-' ) ) {
      return false;
    }
  }
  return true;
}

/**
 * @brief       Returns a human readable string of the current date
 */

std::string getCurrentDate() {
  std::time_t       time = std::time( 0 );
  std::stringstream timeStream;
  timeStream << time;
  return timeStream.str();
}

/**
 * @brief       Gets the prefix of the modestring
 */

char getModePrefix( std::string const& token ) {
  char sign = 'O';
  if( token.find( '+', 0 ) != std::string::npos ) {
    return ( sign = '+' );
  }
  if( token.find( '-', 0 ) != std::string::npos ) {
    return ( sign = '-' );
  }
  return ( sign );
}
