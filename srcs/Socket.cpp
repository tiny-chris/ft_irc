/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:12:09 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/27 19:55:35 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Socket.hpp"
#include "Server.hpp"
#include "utils.hpp"

void Socket::closeSocket( void ) {
  if( _sockfd != -1 ) {
    close( _sockfd );
    _sockfd = -1;
  }
}

int Socket::createListenerSocket( size_t port ) {
  int             yes = 1;  // For setsockopt() SO_REUSEADDR, below
  int             status;
  struct addrinfo hints, *res, *p;
  // Get us a socket and bind it
  std::memset( &hints, 0, sizeof( hints ) );
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;
  status = getaddrinfo( NULL, intToString(port).c_str(), &hints, &res );
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