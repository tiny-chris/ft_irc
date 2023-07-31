/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:                                            +#+  +:+       +#+        */
/*       lmelard <lmelard@student.42.fr>          +#+#+#+#+#+   +#+           */
/*       cgaillag <cgaillag@student.42.fr>             #+#    #+#             */
/*       cvidon <cvidon@student.42.fr>                ###   ########.fr       */
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
  // typedef std::vector< Channel* >	channelPtr;
  Client( void );
  explicit Client( int socket );
  Client( Client const& src );
  ~Client( void );
  Client& operator=( Client const& rhs );

  // Client&			operator=(const Client& other);

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
  // channelPtr		getClientChannels( void ) const;
  std::vector<std::string> getClientChannels( void ) const;

  void setPassStatus( bool const& status );
  void setNickStatus( bool const& status );
  void setIfRegistered( bool const& status );
  void setInvisibleMode( bool const& status );
  void setOperatorMode( bool const& status );
  void setNickname( std::string const& name );
  void setUsername( std::string const& name );
  void setRealname( std::string const& name );
  void setSource( std::string nickname, std::string username );
  // void			setClientChannels( channelPtr channels ) const;
  void setClientChannels( std::vector<std::string> channels ) const;

  // bool			setChannelModes( std::string const& mode );
  void addChannel( std::string channelName );
  void removeClientChannel( std::string chanName );

  void handleUserModeSet( char modeChar, std::string* modechange );
  void handleUserModeUnset( char modeChar, std::string* modechange );

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

  // channelPtr	_clientChannels;
  std::vector<std::string> _clientChannels;
};

#endif /* __CLIENT_HPP__ */
