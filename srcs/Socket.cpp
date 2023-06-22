/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:12:09 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/22 15:29:38 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"

/**
 * @brief       Forbidden gai_strerror implementation
 */

std::string gaiStrerror( int errorCode ) {
  switch( errorCode ) {
    case 0:
      return "Success";
    case EAI_AGAIN:
      return "Temporary failure in name resolution";
    case EAI_BADFLAGS:
      return "Invalid value for ai_flags";
    case EAI_FAIL:
      return "Non-recoverable failure in name resolution";
    case EAI_FAMILY:
      return "ai_family not supported";
    case EAI_MEMORY:
      return "Out of memory";
    case EAI_NONAME:
      return "Name or service not known";
    case EAI_SERVICE:
      return "Invalid value for service";
    case EAI_SOCKTYPE:
      return "ai_socktype not supported";
    case EAI_SYSTEM:
      return strerror( errno );
    default:
      return "Unknown error";
  }
}

void Socket::closeSocket( void ) {
  if( _sockfd != -1 ) {
    close( _sockfd );
    _sockfd = -1;
  }
}

int Socket::createListenerSocket( void ) {
  int             yes = 1;  // For setsockopt() SO_REUSEADDR, below
  int             status;
  struct addrinfo hints, *res, *p;
  // Get us a socket and bind it
  std::memset( &hints, 0, sizeof( hints ) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  status = getaddrinfo( NULL, PORT, &hints, &res );
  if( status != 0 ) {
    std::cerr << "selectserver: " << gaiStrerror( status ) << "\n";
    exit( 1 );
  }
  for( p = res; p != NULL; p = p->ai_next ) {
    _sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
    if( _sockfd < 0 ) {
      continue;
    };
    if( setsockopt( _sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof( int ) )
        != 0 ) {
      std::cerr << "setsockopt: " << strerror( errno ) << "\n";
      exit( 1 );
    }
    if( bind( _sockfd, p->ai_addr, p->ai_addrlen ) != 0 ) {
      close( _sockfd );
      continue;
    }
    break;
  }
  freeaddrinfo( res );
  if( p == NULL ) {
    return -1;
  }
  if( listen( _sockfd, 10 ) == -1 ) {
    return -1;
  }
  return 0;
}