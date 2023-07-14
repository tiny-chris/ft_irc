/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:16:00 by codespace         #+#    #+#             */
/*   Updated: 2023/07/14 15:15:00 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <cerrno>		// errno
# include <cstring>		// strerror
# include <iostream>	// cerr, cout
# include <sstream>		// stringstream
# include <string>		// string
# include <vector>		// vector
# include <map>         // map

# include <arpa/inet.h>	// inet_ntoa
# include <netdb.h>		// recv, send, sockaddr, accept, addrinfo, getaddrinfo, socket, setsockopt, bind, freeaddrinfo, listen
# include <poll.h>		// pollfd, poll
# include <stdlib.h>	// exit
# include <unistd.h>	// close

# include "Client.hpp"
# include "utils.hpp"
# include "defines.hpp"

class Client;

class Channel
{
    public:
        // Channel(std::string const& name);
        Channel();
        Channel(std::string& name, Client& chanop);
        Channel(Channel const& src);
        ~Channel();

        Channel & operator=(Channel const & rhs);

        std::string getChannelName( void ) const;
        bool        getKeyStatus( void ) const;
        bool        getLimitStatus( void ) const;
        bool        getInviteOnlyStatus( void ) const;
        bool        getTopicRestrictionStatus( void ) const;
        std::string getLimit( void ) const;
        std::string getKey( void ) const;

        std::string getModes( void ) const;
        std::string getModesArgs( void ) const;
        bool        checkValidLimit(std::string limit) const;

        void		setChannelName( std::string& name );
        void        setKeyStatus( bool const& status );
        void        setLimitStatus( bool const& status );
        void        setInviteOnlyStatus( bool const& status );
        void        setTopicRestrictionStatus( bool const& status );
        void        setLimit( std::string const& limit );
        void        setKey( std::string const& key );

        bool        checkChannelOps( std::string name );

        std::vector<Client*>            _channelOps;
    private:
        
        // std::map<std::string, Client*> _connectedClients;
        // std::map<std::string *, Client *> _channelOps;
        std::string                    _channelName;

        bool    _keyStatus;
        bool    _limitStatus;
        bool    _inviteOnlyStatus;
        bool    _topicRestrictionStatus;

        std::string _key;
        std::string _limit;
};

#endif /* __CHANNEL_HPP__ */
