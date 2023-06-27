/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/23 15:56:26 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/27 11:24:22 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Server.hpp"
#include "Socket.hpp"
#include "Utils.hpp"
#include "NumericReplies.hpp"

void	handlePass(Client &client, std::string param, std::string password)
{
	std::string	reply; 
	// checking if the Client is already registered
	// meaning checking if PASS, NICK, USER are already set
	// if not ERR_ALREADYREGISTERED numeric reply is sent
	if (client.getIfResgistered() == true)
	{
		// ERR_ALREADYREGISTERED numeric reply is sent
		reply = ERR_ALREADYREGISTRED;
		std::cout << "print reply: " << reply << std::endl; // to del 
		send(client.getCfd(), reply.c_str(), reply.length(), 0);
	}
	// else if there is no param to the PASS command 
	// ERR_NEEDMOREPARAMS numeric reply is sent
	else if (param.compare("") == 0)
	{
		// ERR_NEEDMOREPARAMS numeric reply is sent
		std::string command = "pass";
		reply = ERR_NEEDMOREPARAMS (command);
		std::cout << "print reply: " << reply << std::endl; // to del
		send(client.getCfd(), reply.c_str(), reply.length(), 0);
	}
	// else if Pass command's param is different from the password set for the Server
	// then ERR_PASSDMISMATCH error is sent and 
	else if (param.compare(password) != 0 || param.size() != password.size())
	{
		// wrong password numeric reply is sent ERR_PASSWRDMISMATCH
		reply = ERR_PASSWDMISMATCH;
		std::cout << "print reply: " << reply << std::endl; // to del
		send(client.getCfd(), reply.c_str(), reply.length(), 0);
		//killClient(); close le fd, afficher le message KILL, supprimer le fd du client des fd poll et supprimer le client de la map
		//REMPLACER LES LIGNES CI DESSOUS -> UTILISER LA FONCTION DE CLEM NOUVELLE VERSION DE SON SERVEUR QUI NE FERME PAS TOUT DE SUITE LES FD.
		
	}
	// else if it's the right password, the client is not yet registered then setPassStatus to true
	else 
		client.setPassStatus(true);
	return ;
}

