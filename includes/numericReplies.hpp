/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   numericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/23 17:11:00 by lmelard           #+#    #+#             */
/*   Updated: 2023/07/28 21:00:05 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NUMERIC_REPLIES_HPP
# define NUMERIC_REPLIES_HPP

# include "defines.hpp"

/* ***************************************** */
/*         ERROR NUMERIC REPLIES             */
/* ***************************************** */

// Utilisé pour indiquer que le pseudonyme passé en paramètre à la commande n'est pas actuellement utilisé.
# define ERR_NOSUCHNICK(source, nickname) (std::string(":") + source + " 401 " + nickname + " :No such nick/channel\r\n")

//Utilisé pour indiquer que le nom du serveur donné n'existe pas actuellement.
# define ERR_NOSUCHSERVER(source, server) (std::string(":") + source + " 402 " + server + " :No such server" + CRLF)

// Indicates that no channel can be found for the supplied channel name.
# define ERR_NOSUCHCHANNEL(source, nickname, channel) (std::string(":") + source + " 403 " + nickname + " " + channel + " :No such channel\r\n")

//Envoyé à un utilisateur qui (a) soit n'est pas dans un canal en mode +n ou (b) n'est pas opérateur (ou mode +v) sur un canal en mode +m ; et essaie d'envoyer un PRIVMSG à ce canal.
# define ERR_CANNOTSENDTOCHAN(source, nickname, channel) (std::string(":") + source + " 404 " + nickname + " " + channel + " :Cannot send to channel\r\n" )

//Envoyé à un utilisateur quand il a atteint le nombre maximal de canaux qu'il est autorisé à accéder simultanément, s'il essaie d'en rejoindre un autre.
# define ERR_TOOMANYCHANNELS(source, nickname, channel) (std::string(":") + source + " 405 " + nickname + " " + channel + " :You have joined too many channels\r\n")

//Renvoyé à un client qui essaie d'envoyer un PRIVMSG/NOTICE utilisant le format de destination utilisateur@hôte pour lequel utilisateur@hôte a plusieurs occurrences.
# define ERR_TOOMANYTARGETS(source, destination) (std::string(":") + source + " 407 " + destination + " :Duplicate recipients. Ne message delivered" + CRLF)

//Indicates a PING or PONG message missing the originator parameter which is required by old IRC servers. Nowadays, this may be used by some servers when the PING <token> is empty.
# define ERR_NOORIGIN(source, nickname) (std::string(":") + source + " 409 " + nickname + " :No origin specified\r\n")

//Pas de destinataire.
# define ERR_NORECIPIENT(source, nickname, command) (std::string(":") + source + " 411 " + nickname + " :No recipient given (" + command + ")" + CRLF)

//No text to send"
# define ERR_NOTEXTTOSEND(source, nickname) (std::string(":") + source + " 412 " + nickname + " :No text to send" + CRLF)

//Renvoyé à un client enregistré pour indiquer que la commande envoyée est inconnue du serveur.
# define  ERR_UNKNOWNCOMMAND(source, nickname, command) (std::string(":") + source + " 421 " + nickname + " " + command + " :Unknown command\r\n")

//Renvoyé quand un paramètre pseudonyme attendu pour une commande n'est pas fourni.
# define  ERR_NONICKNAMEGIVEN(source, nickname) (std::string(":") + source + " 431 " + nickname + " :No nickname given\r\n")

// Renvoyé après la réception d'un message NICK qui contient des caractères qui ne font pas partie du jeu autorisé. Voir les sections 1 et 2.2 pour les détails des pseudonymes valides.
# define  ERR_ERRONEUSNICKNAME(source, clientNickname, nickname) (std::string(":") + source + " 432 " + clientNickname + " :Erroneus nickname\r\n")

// Renvoyé quand le traitement d'un message NICK résulte en une tentative de changer de pseudonyme en un déjà existant.
# define  ERR_NICKNAMEINUSE(source, clientNickname, nickname)(std::string(":") + source + " 433 " + clientNickname + " " + nickname + " :Nickname is already in use\r\n")

// Renvoyé par un serveur à un client lorsqu'il détecte une collision de pseudonymes (enregistrement d'un pseudonyme qui existe déjà sur un autre serveur).
# define  ERR_NICKCOLLISION(source, clientNickname, nickname) (std::string(":") + source + " 436 " + clientNickname + " :Nickname collision KILL\r\n")

// Renvoyé par un serveur pour indiquer que l'utilisateur donné n'est pas dans le canal spécifié.
# define  ERR_USERNOTINCHANNEL(source, nickname, tokick, channel) (std::string(":") + source + " 441 " + nickname + " " + tokick + " " + channel + " :They aren't on that channel\r\n")

// Renvoyé par le serveur quand un client essaie une commande affectant un canal dont il ne fait pas partie.
# define  ERR_NOTONCHANNEL(source, nickname, channel) (std::string(":") + source + " 442 " + nickname + " " + channel + " :You're not on that channel\r\n")

// Renvoyé quand un client essaie d'inviter un utilisateur sur un canal où il est déjà.
# define  ERR_USERONCHANNEL(source, nickname, toinvite, channel) (std::string(":") + source + " 443 " + nickname + " " + toinvite + " " + channel + " :is already on channel\r\n")

// Retourné par le serveur pour indiquer à un client qu'il doit être enregistré avant que ses commandes soient traitées.
# define  ERR_NOTREGISTERED(source, nickname) (std::string(":") + source + " 451 " + nickname + " :You have not registered\r\n")

// Renvoyé par un serveur par de nombreuses commandes, afin d'indiquer que le client n'a pas fourni assez de paramètres.
# define  ERR_NEEDMOREPARAMS(source, nickname, command) (std::string(":") + source + " 461 " + nickname + " " + command + " :Not enough parameters\r\n")

// Retourné par le serveur à tout lien qui tente de changer les détails enregistrés (tels que mot de passe et détails utilisateur du second message USER)
# define  ERR_ALREADYREGISTRED(source, nickname) (std::string(":") + source + " 462 " + nickname + " :You may not reregister\r\n")

// Retourné pour indiquer l'échec d'une tentative d'enregistrement d'une connexion dû à un mot de passe incorrect ou manquant.
# define  ERR_PASSWDMISMATCH(source, nickname) (std::string(":") + source + " 464 " + nickname + " :Password incorrect\r\n")

// # define  ERR_CHANNELISFULL(source, channel) (std::string(":") + source + " 471 " + channel + " :Cannot join channel (+l)\r\n")
// Impossible de joindre le canal (+l)
# define  ERR_CHANNELISFULL(source, nickname, channel) (std::string(":") + source + " 471 " + nickname + " " + channel + " :Cannot join channel (+l)\r\n")

// Impossible de joindre le canal (+i).
# define  ERR_INVITEONLYCHAN(source, nickname, channel) (std::string(":") + source + " 473 " + nickname + " " + channel + " :Cannot join channel (+i)\r\n")

// Impossible de joindre le canal (+k).
# define  ERR_BADCHANNELKEY(source, nickname, channel) (std::string(":") + source + " 475 " + nickname + " " + channel + " :Cannot join channel (+k)\r\n")

// Indicates the supplied channel name is not a valid.
// This is similar to, but stronger than, ERR_NOSUCHCHANNEL (403), which indicates that the channel does not exist, but that it may be a valid name.
// The text used in the last param of this message may vary
# define ERR_BADCHANMASK(source, channel) (std::string(":") + source + " 476 " + channel + " :Bad Channel Mask\r\n")

// Toute commande qui requiert le privilège d'opérateur pour opérer doit retourner cette erreur pour indiquer son échec.
# define  ERR_NOPRIVILEGES(source) (std::string(":") + source + " 481 " + ":Permission Denied- You're not an IRC operator" + CRLF)

// Toute commande qui requiert les privilèges 'chanop' (tels les messages MODE) doit retourner ce message à un client qui l'utilise sans être chanop sur le canal spécifié.
# define ERR_CHANOPRIVSNEEDED(source, nickname, channel) (std::string(":") + source + " 482 " + nickname + " " + channel + " :You're not channel operator\r\n")

// Toute tentative d'utiliser la commande KILL sur un serveur doit être refusée et cette erreur renvoyée directement au client.
# define ERR_CANTKILLSERVER (std::string(":") + " 483 " + ":You cant kill a server!" + CRLF)

// Renvoyé par un serveur pour indiquer que le message MODE a été envoyé avec un pseudonyme et que le mode spécifié n'a pas été identifié.
# define ERR_UMODEUNKNOWNFLAG(source, nickname) (std::string(":") + source + " 501 " + nickname + " :Unknown MODE flag\r\n")

// Erreur envoyée à tout utilisateur qui essaie de voir ou de modifier le mode utilisateur d'un autre client.
# define ERR_USERSDONTMATCH(source, nickname) (std::string(":") + source + " 502 " + nickname + " :Cant change mode for other users\r\n")


/* ***************************************** */
/*             NUMERIC REPLIES               */
/* ***************************************** */

/******		REGISTRATION REPLIES		******/

// The first message sent after client registration, this message introduces the client to the network. The text used in the last param of this message varies wildly.
# define RPL_WELCOME(source, nickname) (std::string(":") + source + " 001 " + nickname + " :Welcome to the Internet Relay Network, " + source + "\r\n")

// Part of the post-registration greeting, this numeric returns the name and software/version of the server the client is currently connected to. The text used in the last param of this message varies wildly.
# define RPL_YOURHOST(source, nickname) (std::string(":") + source + " 002 " + nickname + " :Your host is " + SERVERNAME + " running version " + VERSION + "\r\n")

// Part of the post-registration greeting, this numeric returns a human-readable date/time that the server was started or created. The text used in the last param of this message varies wildly.
# define RPL_CREATED(source, nickname, date) (std::string(":") + source + " 003 " + nickname + " :This server was created " + date + "\r\n")

// Part of the post-registration greeting. Clients SHOULD discover available features using RPL_ISUPPORT tokens rather than the mode letters listed in this reply.
# define RPL_MYINFO(source, nickname) (std::string(":") + source + " 004 " + nickname + " " + SERVERNAME + " " + VERSION + " " + USERMODES + " " + CHANNELMODES + " " + CHANNELMODESPARAM + "\r\n")

// Part of the post-registration greeting. Clients SHOULD discover available features using RPL_ISUPPORT tokens rather than the mode letters listed in this reply.
# define RPL_ISUPPORT(source, nickname, token) (std::string(":") + source + " 005 " + token + " :are supported by this server\r\n")

/******		  LUSERS REPLIES		  ******/

// <u>, <i>, and <s> are non-negative integers, and represent the number of total users, invisible users, and other servers connected to this server.
# define RPL_LUSERCLIENT(source, nickname, totalusers, invisibleuser) (std::string(":") + source + " 251 " + nickname + " :There are " + totalusers + " users and " + invisibleuser + " invisible on 1 servers\r\n")

// <ops> is a positive integer and represents the number of IRC operators connected to this server. The text used in the last param of this message may vary.
# define RPL_LUSEROP(source, nickname, ops) (std::string(":") + source + " 252 " + nickname + " " + ops + " :operator(s) online\r\n")

// <connections> is a positive integer and represents the number of connections to this server that are currently in an unknown state. The text used in the last param of this message may vary.
# define RPL_LUSERUNKNOWN(source, nickname, connections) (std::string(":") + source + " 253 " + nickname + " " + connections + " :unknown connection(s)\r\n")

// <channels> is a positive integer and represents the number of channels that currently exist on this server. The text used in the last param of this message may vary.
# define RPL_LUSERCHANNELS(source, nickname, channels) (std::string(":") + source + " 254 " + nickname + " " + channels + " :channels formed\r\n")

// <c> and <s> are non-negative integers and represent the number of clients and other servers connected to this server, respectively.
# define RPL_LUSERME(source, nickname, clientsnbr) (std::string(":") + source + " 255 " + nickname + " :I have " + clientsnbr + " clients and 0 servers\r\n")

/******		  MODE REPLIES		  ******/

// Sent to a client to inform that client of their currently-set user modes.
# define RPL_UMODEIS(source, nickname, usermodes) (std::string(":") + source + " 221 " + nickname + " " + usermodes + "\r\n")

// Sent to a client to inform them of the currently-set modes of a channel.
# define  RPL_CHANNELMODEIS(source, nickname, channel, modestring, modesargs) (std::string(":") + source + " 324 " + nickname + " " + channel + " " + modestring + " " + modesargs + "\r\n")

/******		  TOPIC REPLIES		  ******/

# define RPL_NOTOPIC(source, nick, channel) (std::string(":") + source + " 331 " + nick + " " + channel + " :No topic is set\r\n")

// Lors de l'envoi d'un message TOPIC pour déterminer le sujet d'un canal, une de ces deux réponses est envoyée. Si le sujet est défini, RPL_TOPIC est renvoyée, sinon c'est RPL_NOTOPIC.
# define RPL_TOPIC(source, nick, channel, topic) (std::string(":") + source + " 332 " + nick + " " + channel + " :" + topic + "\r\n")

// Sent to a client to let them know who set the topic (<nick>) and when they set it (<setat> is a unix timestamp). Sent after RPL_TOPIC (332).
# define RPL_TOPICWHOTIME(source, client, channel, nick, setat) (std::string(":") + source + " 333 " + client + " " + channel + " " + nick + " " + setat + "\r\n")

/******		  INVITE REPLIES		  ******/

// Renvoyé par un serveur pour indiquer que le message INVITE a été enregistré, et est en cours de transmission au client final.
# define RPL_INVITING(source, nick, toinvite, channel) (std::string(":") + source + " 341 " + nick + " " + toinvite + " " + channel + "\r\n")

/******		  WHO REPLIES		      ******/

# define RPL_WHOREPLY(source, client, channel, username, host, servername, flags, realname) (std::string(":") + source + " 352 " + client + " " + channel + " " + username + " " + host + " " + servername + " " + nick + " " + flags + " :0 " + realname + "\r\n")

// La paire RPL_WHOREPLY et RPL_ENDOFWHO est utilisée en réponse à un message WHO. Le RPL_WHOREPLY n'est envoyé que s'il y a une correspondance à la requête WHO. S'il y a une liste de paramètres fournie avec le message WHO, un RPL_ENDOFWHO doit être envoyé après le traitement de chaque élément de la liste, <nom> étant l'élément.
# define RPL_ENDOFWHO(source, client, mask) (std::string(":") + source + " 315 " + client + " " + mask + " :End of WHO list\r\n")

/******		  NAME REPLIES		      ******/

// Sent as a reply to the NAMES command, this numeric lists the clients that are joined to <channel> and their status in that channel.
// Only public channel so no use of <symbol> (public, secret, private)
# define RPL_NAMREPLY(source, nickname, channel, members) (std::string(":") + source + " 353 " + nickname + " = " + channel + " :" + members + "\r\n")

// Sent as a reply to the NAMES command, this numeric specifies the end of a list of channel member names.
# define RPL_ENDOFNAMES(source, nickname, channel) (std::string(":") + source + " 366 " + nickname + " " + channel + " :End of /NAMES list\r\n")

/******		  BAN REPLIES	     	  ******/

// Quand il liste les bannissements actifs pour un canal donné, un serveur doit renvoyer la liste en utilisant les messages RPL_BANLIST et RPL_ENDOFBANLIST. Un RPL_BANLIST différent doit être utilisé pour chaque identification de bannissement. Après avoir listé les identifications de bannissement (s'il y en a), un RPL_ENDOFBANLIST doit être renvoyé.
# define RPL_ENDOFBANLIST(source, nickname, channel) (std::string(":") + source + " 368 " + nickname + " " + channel + " :End of channel ban list\r\n")

/******		  MOTD REPLIES		      ******/

# define RPL_MOTDSTART(source, nickname, server) (std::string(":") + source + " 375 " + nickname + " :- " + server + " Message of the day - \r\n")

# define RPL_MOTD(source, nickname) (std::string(":") + source + " 372 " + nickname + " :")

// Lorsqu'il répond à un message MOTD et que le fichier MOTD est trouvé, le fichier est affiché ligne par ligne, chaque ligne ne devant pas dépasser 80 caractères, en utilisant des réponses au format RPL_MOTD. Celles-ci doivent être encadrées par un RPL_MOTDSTART (avant les RPL_MOTDs) et un RPL_ENDOFMOTD (après).
# define RPL_ENDOFMOTD(source, nickname) (std::string(":") + source + " 376 " + nickname + " :End of /MOTD command.\r\n")


/* ***************************************** */
/*    CUSTOM MESSAGES NO NUMERIC REPLIES     */
/* ***************************************** */

# define KILL_MSG(source, nickname)							(std::string(source) + " KILL " + nickname + " :" + source + "\r\n")
# define RPL_NICK(oldNickname, newNickname)					(std::string(":") + oldNickname + " NICK " + newNickname + "\r\n")
# define MSG_MODE(source, nickname, modeString, modeargs)	(std::string(":") + source + " MODE " + nickname + " " + modeString + " " + modeargs + "\r\n")
# define MSG_MODE_CUSTOM(source, channel, modes)			(std::string(":") + source + " MODE " + channel + " " + modes + "\r\n")
# define RPL_JOIN(source, nickname, channel)				(std::string(":") + source + " JOIN " + channel + CRLF)
# define KICKER(source, nickname, channel, reason) 			(std::string(":") + source + " KICK " + channel + " "  + nickname + " :" + reason + "\r\n")
# define DEFAULTKICK(nickname, channel, tokick, reason) 	(std::string(":") + nickname + " KICK " + channel + " "  + tokick + " :" + reason + "\r\n")
# define KICK_REASON 										"bye bye looser\r\n"
# define INVITE(inviter, invitee, channel)                  (std::string(":") + inviter + " INVITE " + invitee + " " + channel + "\r\n")
# define RPL_PART(source, nickname, channel, reason)		(std::string(":") + source + " PART " + channel + reason + CRLF)
# define ERR_CANNOTPART(source, channel, reason)			(std::string(":") + source + " " + channel + reason + CRLF)
# define RPL_PRIVMSG(source, nickname, target, message)		(std::string(":") + source + " PRIVMSG " + target + " :" + message + CRLF)
# define RPL_QUIT(source, quitter, reason)					 (std::string(":") + source + " QUIT :" + reason + "\r\n")
# define RPL_SQUIT(source, reason)							(std::string(":") + source + " " + nickname + " SQUIT " + ":" + reason + CRLF)
// # define RPL_KILL(source, tokill, reason)					(std::string(":") + source + " KILL " + tokill + " :" + reason + CRLF)
# define KILL_REASON(killer, reason)						("Killed (" + killer + " (" + reason + "))")
# define ERR_KILL(source, servername, reason) 				("Closing Link: " + servername + " (" + reason + ")" + CRLF)
# define CHANNEL_DELETED(source, tokick, channelName)       (std::string(":") + source + " KICK last chanop " + tokick + " from " + channelName + " :channel was deleted " + CRLF)

#endif /* __NUMERIC_REPLIES_HPP__*/
