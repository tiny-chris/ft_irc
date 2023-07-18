/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   names.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/18 12:52:41 by cgaillag         ###   ########.fr       */
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

void	Server::displayNames( int clientSocket, Channel& channel )
{
	Channel::mapClientsPtr&					chanMembers = channel.getChannelMembers();
	Channel::mapClientsPtr::const_iterator	it;
	bool									requestorIsOnChannel = false;
	std::string								listMembers;


/*  ****************************************  */
/*  POUR CHECKER LES CLIENTS DANS CHANNEL !!  */
/*  ****************************************  */

		std::cout << ZZ_MSGTEST;
		int n = 0;
		for (Channel::mapClientsPtr::const_iterator	testIt = chanMembers.begin(); testIt != chanMembers.end(); testIt++)
		{
			std::cout << "\n\t channel member [" << n << "] named " << testIt->second->getNickname();
			std::cout << " with status 'is visible' = " << testIt->second->getIfInvisible() << "\n";
			n++;
		}
/*  ****************************************  */
/*  ****************************************  */

	std::cout << ZZ_MSGTEST << "Current channel = '" << channel.getChannelName() << "'\n" << std::endl;
	if ( chanMembers.find( _clients.at( clientSocket ).getNickname() ) != chanMembers.end() ) {
		requestorIsOnChannel = true ;
	}
	std::cout << ZZ_MSGTEST << "requestor client = '" << _clients.at( clientSocket ).getNickname() << "' is on channel :" << requestorIsOnChannel << "\n" << std::endl;
	for ( it = chanMembers.begin(); it != chanMembers.end(); it++ )
	{
		if ( it->second->getIfInvisible() == true && requestorIsOnChannel == false )
			continue ;
		if ( it != chanMembers.begin() ) {
			listMembers += " ";
		}
		listMembers += it->first;
	}

	listMembers = "[ " + listMembers + " ]";

	replyMsg( clientSocket, RPL_NAMREPLY( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), channel.getChannelName(), listMembers ) );
	replyMsg( clientSocket, RPL_ENDOFNAMES( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), channel.getChannelName() ) );
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

void	Server::handleNames( int clientSocket, std::string param )
{
	// std::cout << "ZZ_test:\t " << "on entre dans handleNames\n" << std::endl;
	mapChannels::iterator		chanIt;
	// std::string					chanMembers;
	std::vector<std::string>	tokens = splitString( param, ',' );

	// if there is no param: display all channels' name (one channel by one)
	if ( param.empty() || !tokens.size() )
	{
		for ( chanIt = _channels.begin(); chanIt != _channels.end(); ++chanIt )
		{
			Channel&	channel = chanIt->second;
			displayNames( clientSocket, channel );
		}
	}
	// else: display client names of listed channels that exist (evaluate channels one by one)
	else
	{
		std::cout << "ZZ_test:\t " << "IL Y A DES TOKENS !!\n" << std::endl;
		// utiliser TARGMAX pour limiter les multi-ddes ???
		// créer une macro 'TARGMAX' dans defines !!
		// TARGMAX=ACCEPT:,KICK:1,LIST:1,NAMES:1,NOTICE:4,PRIVMSG:4,WHOIS:1
		// créer une std::map<std::string,int> targmaxLimits pour stocker les targmax
		size_t targmax = ( 3 < tokens.size() ) ? 3 : tokens.size() ;
		for ( size_t i = 0; i < targmax; i++ )
		{
			chanIt = _channels.find(tokens[ i ]);
			if ( chanIt != _channels.end() )
			{
				// std::cout << ZZ_MSGTEST << "'" << tokens[i] << "' est bien dans la liste des channels" << std::endl;
				Channel&	channel = chanIt->second;
				displayNames( clientSocket, channel );
			}
			else
			{
				// std::cout << ZZ_MSGTEST << "'" << tokens[i] << "' n'est pas dans la liste des channels" << std::endl;
				replyMsg( clientSocket, RPL_ENDOFNAMES( _clients.at( clientSocket ).getSource(), _clients.at( clientSocket ).getNickname(), tokens[ i ] ) );
			}

			// 	Channel::mapClientsPtr					chanMembers = channel.getChannelMembers();
			// 	Channel::mapClientsPtr::const_iterator	clientIt;
			// 	bool									requestorIsOnChannel = false;


			// 	if ( chanMembers.find( _clients.at( clientSocket ).getNickname() ) != chanMembers.end() )
			// 		requestorIsOnChannel = true ;
			// 	for (clientIt = chanMembers.begin(); clientIt != chanMembers.end(); clientIt++)
			// 	{
			// 		const std::string& clientName = clientIt->first;
			// 		// Client *client = clientIt->second;

			// 		if ( clientIt->second->getIfInvisible() == true && requestorIsOnChannel == false )
			// 		{
			// 			std::cout << ZZ_MSGTEST << "Le client '" << _clients.at( clientSocket ).getNickname() << "' est invisible et le request client n'est pas dans le channel" << std::endl;
			// 		}
			// 		else
			// 		{
			// 			std::cout << ZZ_MSGTEST << "Le request client " << _clients.at( clientSocket ).getNickname() << " est dans le channel --> il faut ajouter " << clientName << std::endl;
			// 		}
			// 	}
			// }
			// else
			// 	std::cout << ZZ_MSGTEST << "'" << tokens[i] << "' n'est pas dans la liste des channels" << std::endl;
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
