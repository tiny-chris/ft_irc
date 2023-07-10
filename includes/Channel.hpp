/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/07 15:16:00 by codespace         #+#    #+#             */
/*   Updated: 2023/07/10 16:09:59 by lmelard          ###   ########.fr       */
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

class Client;

class Channel
{
    public:
        Channel();
        Channel(std::string name);
        Channel(Channel const& src);
        ~Channel();

        Channel & operator=(Channel const & rhs);

        std::string getChannelName( void ) const;
        void		setChannelName( std::string& name );
        
    private:
        std::map<std::string, Client> _connectedClients; 
        std::string _channelName;
};

#endif