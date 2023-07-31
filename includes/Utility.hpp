/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utility.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:                                            +#+  +:+       +#+        */
/*       lmelard <lmelard@student.42.fr>          +#+#+#+#+#+   +#+           */
/*       cgaillag <cgaillag@student.42.fr>             #+#    #+#             */
/*       cvidon <cvidon@student.42.fr>                ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILITY_HPP_
#define UTILITY_HPP_

#include <iosfwd>
#include <map>
#include <string>

/**
 * @brief       Various utility functions.
 */

class Utility {
 public:
  Utility( void );
  /* Utility( Utility const& src ); */
  virtual ~Utility( void );
  /* Utility& operator=( Utility const& rhs ); */

  static void closeFd( int& fd );
  static bool fdIsValid( int fd );

  static int         stringToInt( const std::string& str );
  static std::string intToString( int number );

  static std::string ntop( const struct sockaddr_storage& socket );
  static std::string gaiStrerror( int errorCode );
};

#endif  // UTILITY_HPP_
