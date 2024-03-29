/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by 2.fr>             #+#    #+#             */
/*   Updated: 2023/08/02 10:36:11 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <iosfwd>
#include <string>

#include "Channel.hpp"
#include "Server.hpp"
#include "numericReplies.hpp"

class Channel;

class Client {
 public:
  Client( void );
  explicit Client( int socket );
  Client( Client const& src );
  ~Client( void );
  Client& operator=( Client const& rhs );

  int         getFd( void ) const;
  bool        getPassStatus( void ) const;
  bool        getNickStatus( void ) const;
  bool        getIfRegistered( void ) const;
  bool        getInvisibleMode( void ) const;
  bool        getOperatorMode( void ) const;
  std::string getModes( void ) const;
  std::string getNickname( void ) const;
  std::string getUsername( void ) const;
  std::string getRealname( void ) const;
  std::string getSource( void ) const;
  std::vector<std::string> getClientChannels( void ) const;
  std::string getBuf( void ) const;

  void setPassStatus( bool const& status );
  void setNickStatus( bool const& status );
  void setIfRegistered( bool const& status );
  void setInvisibleMode( bool const& status );
  void setOperatorMode( bool const& status );
  void setNickname( std::string const& name );
  void setUsername( std::string const& name );
  void setRealname( std::string const& name );
  void setSource( std::string nickname, std::string username );
  void setClientChannels( std::vector<std::string> channels ) const;
  void setBuf( std::string const& buf );
  
  void addChannel( std::string channelName );
  void removeClientChannel( std::string chanName );

  bool handleUserModeSet( char modeChar, std::string* modechange );
  bool handleUserModeUnset( char modeChar, std::string* modechange );

 private:
  int _fd;

  bool _passStatus;
  bool _nickStatus;
  bool _isRegistered;

  /* ******* USER MODES ****** */
  bool _invisibleMode;
  bool _operatorMode;

  std::string _nickname;
  std::string _username;
  std::string _realname;
  std::string _source;

  std::vector<std::string> _clientChannels;
  std::string _buf;
  
};

#endif /* __CLIENT_HPP__ */
