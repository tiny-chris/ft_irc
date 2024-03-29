/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By:                                            +#+  +:+       +#+        */
/*       lmelard <lmelard@student.42.fr>          +#+#+#+#+#+   +#+           */
/*       cgaillag <cgaillag@student.42.fr>             #+#    #+#             */
/*       cvidon <cvidon@student.42.fr>                ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <cerrno>    // errno
#include <cstring>   // strerror
#include <iostream>  // cerr, cout
#include <map>       // map
#include <sstream>   // stringstream
#include <string>    // string
#include <vector>    // vector

#include <arpa/inet.h>  // inet_ntoa
#include <netdb.h>  // recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
#include <poll.h>   // pollfd, poll
#include <unistd.h>  // close

#include "Client.hpp"
#include "Server.hpp"
#include "defines.hpp"
#include "utils.hpp"

class Client;
class Server;

class Channel {
 public:
  typedef std::map<std::string, Client*> mapClientsPtr;
  typedef std::vector<std::string>       vecString;

  Channel();
  Channel( std::string const& name );
  Channel( Channel const& src );
  ~Channel();

  Channel& operator=( Channel const& rhs );

  std::string getChannelName( void ) const;
  bool        getKeyStatus( void ) const;
  bool        getLimitStatus( void ) const;
  bool        getInviteOnlyStatus( void ) const;
  bool        getTopicRestrictionStatus( void ) const;
  int         getLimitBis( void ) const;
  std::string getLimit( void ) const;
  std::string getKey( void ) const;
  std::string getTopic( void ) const;
  std::string getTopicSetter( void ) const;
  std::string getTopicDate( void ) const;

  std::string    getModes( void ) const;
  std::string    getModesArgs( void ) const;
  mapClientsPtr& getChannelOps( void );
  mapClientsPtr& getChannelMembers( void );
  vecString&     getInvitedMembers( void );

  bool checkValidLimit( std::string limit ) const;

  void setChannelName( std::string& name );
  void setKeyStatus( bool const& status );
  void setLimitStatus( bool const& status );
  void setInviteOnlyStatus( bool const& status );
  void setTopicRestrictionStatus( bool const& status );
  void setLimitBis( int const& limit );
  void setLimit( std::string const& limit );
  void setKey( std::string const& key );
  void setTopic( std::string const& topic );
  void setTopicSetter( std::string const& topicSetter );
  void setTopicDate( std::string const& topicDate );

  /* ********* METHHODS ******* */
  void addChannelOps( Client* client );
  void removeChannelOp( Client* client );
  void addChannelMember( Client* client );
  void removeChannelMember( Client* client );
  void addInvitedMember( const std::string& clientName );
  void removeInvitedMember( const std::string& clientName );

  bool checkChannelOps( std::string name );
  bool isInvited( std::string clientName ) const;
  bool checkChannelMembers( std::string name );

  void handleChannelModeSet( char modeChar, std::string* modeArgs,
                             std::string*                    modeChange,
                             const std::vector<std::string>& tokens,
                             size_t*                         j );
  void handleChannelModeUnset( char modeChar, std::string* modeArgs,
                               std::string*                    modeChange,
                               const std::vector<std::string>& tokens,
                               size_t*                         j );
  void handleModeSetKey( std::string* modeArgs, std::string* modeChange,
                         const std::vector<std::string>& tokens, size_t* j );
  void handleModeSetLimit( std::string* modeArgs, std::string* modeChange,
                           const std::vector<std::string>& tokens, size_t* j );
  void handleModeSetInviteOnly( std::string* modeChange );
  void handleModeSetTopicRestriction( std::string* modeChange );
  void handleModeSetOperator( std::string* modeArgs, std::string* modeChange,
                              const std::vector<std::string>& tokens,
                              size_t*                         j );
  void handleModeUnsetKey( std::string* modeChange );
  bool isValidModeChar( char const modeChar );

  void handleModeUnsetLimit( std::string* modeChange );
  void handleModeUnsetInviteOnly( std::string* modeChange );
  void handleModeUnsetTopicRestriction( std::string* modeChange );
  void handleModeUnsetOperator( std::string* modeArgs, std::string* modeChange,
                                const std::vector<std::string>& tokens,
                                size_t*                         j );
  void updateChannelMode( const std::vector<std::string>& tokens,
                          std::string& modeChange, std::string& modeArgs );

 private:
  std::string   _channelName;
  mapClientsPtr _channelOps;
  mapClientsPtr _channelMembers;
  vecString     _invitedMembers;

  bool _keyStatus;
  bool _limitStatus;
  bool _inviteOnlyStatus;
  bool _topicRestrictionStatus;

  int         _limitBis;
  std::string _key;
  std::string _limit;
  std::string _topic;
  std::string _topicSetter;
  std::string _topicDate;
};

#endif /* __CHANNEL_HPP__ */
