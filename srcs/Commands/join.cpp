/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   join.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: codespace <codespace@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/30 17:36:40 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/24 11:53:08 by codespace        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include "Client.hpp"
#include "defines.hpp"
#include "numericReplies.hpp"
#include "utils.hpp"

/**
 * @brief       JOIN command
 *
 *				spécification IRC (RFC 2812)
 *				http://abcdrfc.free.fr/rfc-vf/rfc2812.html
 *				https://modern.ircdocs.horse/#connection-setup

				syntax:
					JOIN <channel> [key] [invitationkey] [mode]

				Numeric Replies:
					ERR_NEEDMOREPARAMS (461)	x
					ERR_NOSUCHCHANNEL (403)		x
					ERR_TOOMANYCHANNELS (405)	x
					ERR_BADCHANNELKEY (475)		x
					ERR_BANNEDFROMCHAN (474)	x
					ERR_CHANNELISFULL (471)		x
					ERR_INVITEONLYCHAN (473)	x
					ERR_BADCHANMASK (476)		x
					RPL_TOPIC (332)				x
					RPL_TOPICWHOTIME (333)
					RPL_NAMREPLY (353)
					RPL_ENDOFNAMES (366)
 *
 */

void	Server::handleJoin( int clientSocket, std::string cmd, std::string param )
{
	std::vector< std::string >	tokens = splitString(param, ' ');
	Client& 					client = _clients.at( clientSocket );

	std::cout << ZZ_MSGTEST << cmd << " - nombre de tokens: " << tokens.size() << std::endl;

	/* ******************************* ***
	** CHECK 1 - NOT ENOUGH PARAMETERS ***
	** ******************************* ***
	**	check if param is empty --> ERR_NEEDMOREPARAMS
	*/
	if ( param.empty() || tokens.size() == 0 ) {
		replyMsg( clientSocket, ERR_NEEDMOREPARAMS( client.getSource(), client.getNickname(), cmd ) );
		return ;
	}

	/*	count the number parameters
	**	- if tokens size > 15 - 1 (-1 for the command) --> too many parameters
	*/
	// --> revoir car on ne devrait pas avoir autant de tokens !!
	// même indiquer que uniquement 2 params ici

	// if ( tokens.size() > MAXPARAM - 1 )
	if ( tokens.size() > 2 )
	{
		// check if there is an existing numeric reply
		std::cout << MSGERROR << cmd << ": too many parameters for this command\n" << std::endl;
		return ;
	}

	if ( tokens.size() == 1 && tokens[ 0 ] == "0" )
	{
		std::cout << ZZ_MSGTEST << "TODO supprimer le client de tous les canaux (utiliser PART)\n" << std::endl;
	}

	/* *************************************************** ***
	** CHECK 2 - EACH CHANNEL STARTS WITH '#' AND IS VALID ***
	** *************************************************** ***
	**	Let's consider tokens[0] to get the channels
	**	- if there is 0 channel --> reject the request as not aligned with our convention
	**		(there must be at least a channel starting with #)
	**	- if a channel is empty or if it does not start with # --> ERR_BADCHANMASK (476)
	**						+ leave
	**	- or if its content is invalid
	**
	**	"channel" (nom du canal) : Le nom du canal doit commencer par un préfixe spécifique
	**	pour indiquer le type de canal, suivi d'un nom unique. Les préfixes couramment
	**	utilisés sont "#" pour les canaux publics et "&" pour les canaux privés.
	**	Le nom du canal peut contenir des lettres, des chiffres et certains caractères
	**	spéciaux tels que "-", "_", et ".".
	*/
	std::vector< std::string >	channels = splitString( tokens[ 0 ], ',' );
	std::cout << ZZ_MSGTEST << "nombre de channels (dans tokens[0]): " << channels.size() << std::endl;
	std::vector< std::string >	keys;
	if ( tokens.size() == 2 ) {
		keys = splitString( tokens[ 1 ], ',' );
	}

	if ( validChannelNames( clientSocket, channels ) == false )
		return ;

	// std::cout << ZZ_MSGTEST << "Print les valeurs de channelNames :";
	// for (std::vector<std::string>::iterator it = channels.begin(); it != channels.end(); it++)
	// {
	// 	std::cout << " " << *it;
	// }
	// std::cout << "\n";

	/* *************************************************** ***
	** CHECK 3 - EACH CHANNEL ALREADY EXISTS OR NOT        ***
	** *************************************************** ***
	*/
	for ( size_t i = 0; i < channels.size(); ++i )
	{
		if ( existingChannel( channels[ i ] ) == true )
		{
			if ( joinExistingChannel( clientSocket, channels[ i ] ) == false )
				return ;
		}
		else
		{
			if ( joinNonExistingChannel( clientSocket, channels[ i ] ) == false )
				return ;
			// std::map<std::string, Channel>::iterator it;
			// for ( it = _channels.begin(); it != _channels.end(); ++it )
			// {
			// 	std::cout << "nom du channel '"<< it->first << "'";
			// 	std::cout << " et nom dans le channel '" << it->second.getChannelName() << "'" << std::endl;
			// }
		}

		// j'ajoute le nom du channel dans le vecteur 'clientChanels' du client
		std::string	channelName = channels[ i ].substr( 1 );
		client.addChannel( channelName );

		// if client has been added to the channel
		replyMsg( clientSocket, RPL_JOIN( client.getSource(), client.getNickname(), channels[ i ] ) );
		// display RPL_TOPIC (if existing) and NAMES
		if ( 1 )// the channel has a topic *** TODO  ***
			replyMsg( clientSocket, RPL_TOPIC( client.getSource(), client.getNickname(), channels[ i ], "topic to be provided" ) );
		handleNames( clientSocket, channelName);

		// prevoir un broadcast pour les users du channel
	}
}

/* ************************************ ***
** CHANNEL DOES NOT EXIST YET           ***
** ************************************ ***
**	check if the client is already on too many channels
**	- if so --> ERR_TOOMANYCHANNELS
**	- if not --> follow process:
**		1. create a new Channel with current Client as operator
**			add potential other parameters (TO BE DEFINED)
**		2. add this new channel to the map in the Server '_channels'
**		3. add this new channel to the list of the Client 'vector'
**
*/
bool	Server::joinNonExistingChannel( int clientSocket, std::string sharpChannelName )
// bool	Server::joinNonExistingChannel( Client& requestor, std::string chanName )
{
	Client&		client = _clients.at( clientSocket );
	std::string	channelName = sharpChannelName.substr( 1 );

	// check if the client is already on too many channels
	if ( client.getClientChannels().size() >= CHANLIMIT )
	{
		replyMsg( clientSocket, ERR_TOOMANYCHANNELS( client.getSource(), client.getNickname(), sharpChannelName ) );
		return false ;
	}

	// j'ajoute un nouveau channel à _channels
	_channels.insert( std::make_pair( channelName, Channel ( channelName ) ) );
	// j'ajoute le client à ce nouveau channel et je le mets en tant que chanOp
	_channels[ channelName ].addChannelMember( &client );
	_channels[ channelName ].addChannelOps( &client );

	std::cout << MSGINFO << "a new channel <" << _channels[ channelName ].getChannelName() << "> is created\n";// << std::endl;
	std::cout << ZZ_MSGTEST << client.getNickname() << " has created the channel <" << sharpChannelName << "> and is chan op\n" << std::endl;

	return true ;
}

/* ************************************ ***
** CHANNEL DOES EXIST                   ***
** ************************************ ***
**	check if the client has already joined this channel
**	- if so --> return ; NO ACTION ??? (to check)
**	- if not --> continue
**
**	check if the client is already on too many channels
**	- if so --> ERR_TOOMANYCHANNELS
**	- if not --> continue
**
**	check -----> ERR_NOSUCHCHANNEL ? ? ? ? ? ? ? ? ?
**
**	check if the channel is available upon invitation
**	- if so --> check if client has received an invitation for this channel
**		- if client was not invited --> ERR_INVITEONLYCHAN
**		- if client was invited : continue
**	- if not : continue
**
**	check if client is banned from this channel --> not handled (not compulsory)
**	- if so --> ERR_BANNEDFROMCHAN
**	- if not : continue
**
**	check if the channel required password
**	- if so --> check if client has provided the password
**		- if no: ERR_BADCHANNELKEY
**		- if yes: continue
**	- it not : continue
**
**	check if channel is full
**	- if so : ERR_CHANNELISFULL
**	- if not : follow process:
**		1. add Client to the existing Channel
**			+ provide spec parameters (update ??)
**		2. add Channel to the list of the Client 'vector'
**
*/
bool	Server::joinExistingChannel( int clientSocket, std::string sharpChannelName )
{
	std::string	channelName = sharpChannelName.substr( 1 );
	Client&		client = _clients.at( clientSocket );
	Channel&	channel = _channels.at( channelName );

	// check if the client has already joined this channel
	if ( channel.getChannelMembers().find( client.getNickname() ) != channel.getChannelMembers().end() ) 
	{
		std::cout << MSGINFO << client.getNickname() << " is alreay on channel " << sharpChannelName << "'\n" << std::endl;
		return false ;
	}

	// check if the client is already on too many channels
	if ( client.getClientChannels().size() >= CHANLIMIT ) 
	{
		replyMsg( clientSocket, ERR_TOOMANYCHANNELS( client.getSource(), client.getNickname(), sharpChannelName ) );
		return false ;
	}

	// check if channel is full --> ***** REVOIR LES CONDITIONS AVEC LENA *****
	if ( channel.getChannelMembers().size() >= MAXMEMBERS || channel.getLimitStatus() == true ) 
	{
		replyMsg( clientSocket, ERR_CHANNELISFULL( client.getSource(), sharpChannelName ) );
		return false ;
	}
	
	// check if the channel is available upon invitation
	if ( channel.getInviteOnlyStatus() == true && !channel.isInvited( client.getNickname() ) ) 
	{
		replyMsg( clientSocket, ERR_INVITEONLYCHAN( client.getSource(), sharpChannelName ) );
		return false ;
	}

	// check if the channel required password
	if ( channel.getKeyStatus() == true )
	{
		// si je n'ai pas de mot de passe correspondant... TODO
		// if ( )
		replyMsg( clientSocket, ERR_BADCHANNELKEY( client.getSource(), sharpChannelName ) );
		return false ;
	}

	_channels[ channelName ].addChannelMember( &client );

	return true ;
}

// 	/* ****************************** ***
// 	** CHECK IF CHANNEL NAME IS VALID ***
// 	** ****************************** ***
// 	*/
bool	Server::validChannelNames( int clientSocket, std::vector<std::string>& channelNames )
{
	Client&	client = _clients.at( clientSocket );
	// specific case when there is no channel name (only spaces)
	if ( !channelNames.size() )
	{
		replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), "<empty>" ) );
		return false;
	}

	// check each channel if there is at least one channel
	for ( size_t i = 0; i < channelNames.size(); ++i )
	{
		std::string chanName = "<empty>";
		if ( !channelNames[ i ].empty() && channelNames[ i ].find( CHANTYPES ) != 0 )
		{
			chanName = channelNames[ i ];
		}
		else if ( channelNames[ i ].length() > 1 )
		{
			chanName = channelNames[ i ].substr(1);
		}

		if ( chanName.compare( "<empty>" ) == 0 || channelNames[ i ].find( CHANTYPES ) != 0
			|| ( channelNames[ i ].find( CHANTYPES ) == 0 && channelNames[ i ].length() == 1 )
			|| ( channelNames[ i ].find( CHANTYPES ) == 0 && !isValidToken( chanName ) ) )
		{
			replyMsg( clientSocket, ERR_BADCHANMASK( client.getSource(), client.getNickname(), chanName ) );
			return false ;
		}

		// channelNames[ i ] = chanName;// // ne pas garder la valeur ??
		std::cout << ZZ_MSGTEST << "val de channelName = " << chanName << std::endl;
	}
	return true ;
}
