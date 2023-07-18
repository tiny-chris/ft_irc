/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/12 19:34:51 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief	Display names on one channel
 *
 *			By using RPL_NAMEREPLY + RPL_ENDOFNAMES
 *			/!\ if a user is invisible --> do not show
 *				unless requesting client is also joined to that channel --> show
 *			remark: case 'secret channel' not taken into account
 */

void	Server::displayNames( const Client& requestor, const Channel& channel )
{
	std::map<std::string, Client>::const_iterator	it;
	std::string										chanMembers;
	bool											requestorIsInChannel = false;
	const std::map<std::string, Client>&			clients = channel.getConnectedClients();

	if ( clients.find( requestor.getNickname() ) != clients.end() )
		requestorIsInChannel = true ;
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if ( it->second.getIfInvisible() == true && requestorIsInChannel == false )
			continue ;
		if (it != clients.begin())
			chanMembers += " ";
		chanMembers += it->first;
	}
	//
	// IL FAUT RETESTER CAR LE SERVEUR ET LE NICKNAME NE S'AFFICHENT PAS
	//
	//
	std::cout << ZZ_MSGTEST << "valeur de req client nickname = '" << requestor.getNickname() << "'\n" << std::endl;
	std::cout << ZZ_MSGTEST << "channel = '" << channel.getChannelName() << "'\n" << std::endl;

	replyMsgFd( requestor.getFd(), RPL_NAMREPLY( requestor.getSource(), requestor.getNickname(), channel.getChannelName(), chanMembers ) );
	replyMsgFd( requestor.getFd(), RPL_ENDOFNAMES( requestor.getSource(), requestor.getNickname(), channel.getChannelName() ) );
}

/**
 * @brief	NAMES command to view the nicknames joined to a channel
 * 			and their channel membership prefixes.
 * 			The param of this command is a list of channel names, delimited by
 * 			a comma (",", 0x2C) character
 *
 * 			syntax:	NAMES <channel>{,<channel>}
 *
 */

void	Server::handleNames( int fd, std::string param )
// void	Server::handleNames( const Client& client, std::string param )
// void	Server::handleNames( size_t cid, std::string param )
{
	std::cout << "ZZ_test:\t " << "on entre dans handleNames\n" << std::endl;

	std::map<std::string, Channel>::iterator	chanIt;
	std::vector<std::string>  					tokens = splitString( param, ',' );

	// if there is no param: display all channels' name (one channel by  one)
	if ( param.empty() || !tokens.size() )
	{
		for ( chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt )
		{
			const Channel&	channel = chanIt->second;
			displayNames(_mapClients[fd], channel);
		}
	}
	// else: display client names of listed channels that exist (evaluate channels one by one)
	else
	{
		// utiliser TARGMAX pour limiter les multi-ddes ???
		// créer une macro 'TARGMAX' dans defines !!
		// TARGMAX=ACCEPT:,KICK:1,LIST:1,NAMES:1,NOTICE:4,PRIVMSG:4,WHOIS:1
		// créer une std::map<std::string,int> targmaxLimits pour stocker les targmax
		size_t targmax = (3 < tokens.size()) ? 3 : tokens.size() ;
		for ( size_t i = 0; i < targmax; i++ )
		{
			chanIt = _channels.find(tokens[ i ]);
			if ( chanIt != _channels.end() )
			{
				const Channel&	channel = chanIt->second;
				displayNames(_mapClients[fd], channel);
			}
			else
				replyMsg( _mapClients[fd].getFd(), RPL_ENDOFNAMES( _mapClients[fd].getSource(), _mapClients[fd].getNickname(), tokens[i] ) );
		}
	}
	return ;
}

/*
RPL_NAMREPLY (353)
  "<client> <symbol> <channel> :[prefix]<nick>{ [prefix]<nick>}"
Sent as a reply to the NAMES command, this numeric lists the clients that are joined to <channel> and their status in that channel.

<symbol> notes the status of the channel. It can be one of the following:

("=", 0x3D) - Public channel.
("@", 0x40) - Secret channel (secret channel mode "+s").
("*", 0x2A) - Private channel (was "+p", no longer widely used today).
<nick> is the nickname of a client joined to that channel, and <prefix> is the highest channel membership prefix that client has in the channel, if they have one. The last parameter of this numeric is a list of [prefix]<nick> pairs, delimited by a SPACE character (' ', 0x20).

RPL_ENDOFNAMES (366)
  "<client> <channel> :End of /NAMES list"
Sent as a reply to the NAMES command, this numeric specifies the end of a list of channel member names.
*/
