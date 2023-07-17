/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Socket.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/22 15:09:52 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/12 16:50:01 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERSOCKET_HPP_
#define SERVERSOCKET_HPP_

#include <iosfwd>
#include <string>

/** @brief      Represents a server-side listening socket and manages the
 *              underlying socket file descriptor. It follows the RAII principle
 *              by automatically managing the socket life cycle.
 */

class ServerSocket {
 public:
  ServerSocket( void );
  /* ServerSocket( ServerSocket const& src ); */
  virtual ~ServerSocket( void );
  /* ServerSocket& operator=( ServerSocket const& rhs ); */
  virtual void print( std::ostream& o ) const;

  int  get( void );
  void create( void );

 private:
  void close( void );

  int _fd;
};

std::ostream& operator<<( std::ostream& o, ServerSocket const& i );

#endif  // SERVERSOCKET_HPP_

