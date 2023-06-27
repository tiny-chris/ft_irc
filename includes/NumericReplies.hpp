/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   NumericReplies.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lmelard <lmelard@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/23 17:11:00 by lmelard           #+#    #+#             */
/*   Updated: 2023/06/27 11:14:54 by lmelard          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef NUMERIC_REPLIES_HPP
# define NUMERIC_REPLIES_HPP

/******	  ERROR REPLIES	  ******/

/* No message delivered error messages */

# define ERR_NOSUCHNICK(nickname) (std::string nickname + " :No such nick/channel\r\n") 
// 401 Utilisé pour indiquer que le pseudonyme passé en paramètre à la commande n'est pas actuellement utilisé.

# define ERR_NOSUCHSERVER 402
//"<nom de serveur> :No such server"
//Utilisé pour indiquer que le nom du serveur donné n'existe pas actuellement.

# define ERR_NOSUCHCHANNEL 403
//"<nom de canal> :No such channel"
//Utilisé pour indiquer que le nom de canal donné est invalide.

# define ERR_CANNOTSENDTOCHAN 404
//"<nom de canal> :Cannot send to channel"
//Envoyé à un utilisateur qui (a) soit n'est pas dans un canal en mode +n ou (b) n'est pas opérateur (ou mode +v) sur un canal en mode +m ; et essaie d'envoyer un PRIVMSG à ce canal.

# define ERR_TOOMANYCHANNELS 405
//"<nom de canal> :You have joined too many channels"
//Envoyé à un utilisateur quand il a atteint le nombre maximal de canaux qu'il est autorisé à accéder simultanément, s'il essaie d'en rejoindre un autre.

# define  ERR_WASNOSUCHNICK 406
//"<nom de canal> :There was no such nickname"
//Renvoyé par WHOWAS pour indiquer qu'il n'y a pas d'information dans l'historique concernant ce pseudonyme.

# define  ERR_TOOMANYTARGETS 407
//"<destination> :Duplicate recipients. No message delivered"
//Renvoyé à un client qui essaie d'envoyer un PRIVMSG/NOTICE utilisant le format de destination utilisateur@hôte pour lequel utilisateur@hôte a plusieurs occurrences.

# define  ERR_NOORIGIN 409
//":No origin specified"
//Message PING ou PONG sans le paramètre origine qui est obligatoire puisque ces commandes doivent marcher sans préfixe.

# define  ERR_NORECIPIENT 411
//":No recipient given (<commande>)"
//Pas de destinataire.

# define  ERR_NOTEXTTOSEND 412
//":No text to send"
//Pas de texte à envoyer.

# define  ERR_NOTOPLEVEL 413
//"<masque> :No toplevel domain specified"
//Domaine principal non spécifié.

# define  ERR_WILDTOPLEVEL 414
//"<masque> :Wildcard in toplevel domain"
//Joker dans le domaine principal

/*********************************************/

# define  ERR_UNKNOWNCOMMAND 421
//"<commande> :Unknown command"
//Renvoyé à un client enregistré pour indiquer que la commande envoyée est inconnue du serveur.

# define  ERR_NOMOTD 422
//":MOTD File is missing"
//Le fichier MOTD du serveur n'a pas pu être ouvert.

# define  ERR_NOADMININFO 423
//"<serveur> :No administrative info available"
//Renvoyé par un serveur en réponse à un message ADMIN quand il y a une erreur lors de la recherche des informations appropriées.

# define  ERR_FILEERROR 424
//":File error doing <opération> on <fichier>"
//Message d'erreur générique utilisé pour rapporter un échec d'opération de fichier durant le traitement d'un message.

# define  ERR_NONICKNAMEGIVEN 431
//":No nickname given"
//Renvoyé quand un paramètre pseudonyme attendu pour une commande n'est pas fourni.

# define  ERR_ERRONEUSNICKNAME 432
// "<pseudo> :Erroneus nickname"
// Renvoyé après la réception d'un message NICK qui contient des caractères qui ne font pas partie du jeu autorisé. Voir les sections 1 et 2.2 pour les détails des pseudonymes valides.

# define  ERR_NICKNAMEINUSE 433
// "<nick> :Nickname is already in use"
// Renvoyé quand le traitement d'un message NICK résulte en une tentative de changer de pseudonyme en un déjà existant.

# define  ERR_NICKCOLLISION 436
// "<nick> :Nickname collision KILL"
// Renvoyé par un serveur à un client lorsqu'il détecte une collision de pseudonymes (enregistrement d'un pseudonyme qui existe déjà sur un autre serveur).

# define  ERR_USERNOTINCHANNEL 441
// "<pseudo> <canal> :They aren't on that channel"
// Renvoyé par un serveur pour indiquer que l'utilisateur donné n'est pas dans le canal spécifié.

# define  ERR_NOTONCHANNEL 442
// "<canal> :You're not on that channel"
// Renvoyé par le serveur quand un client essaie une commande affectant un canal dont il ne fait pas partie.

# define  ERR_USERONCHANNEL 443
// "<utilisateur> <channel> :is already on channel"
// Renvoyé quand un client essaie d'inviter un utilisateur sur un canal où il est déjà.

# define  ERR_NOLOGIN 444
// "<utilisateur> :User not logged in"
// Renvoyé par un SUMMON si la commande n'a pas pu être accomplie, car l'utilisateur n'est pas connecté.

# define  ERR_SUMMONDISABLED 445
// ":SUMMON has been disabled"
// Renvoyé en réponse à une commande SUMMON si le SUMMON est désactivé. Tout serveur qui ne gère pas les SUMMON doit retourner cette valeur.

# define  ERR_USERSDISABLED 446
// ":USERS has been disabled"
// Retourné en réponse à une commande USERS si la commande est désactivée. Tout serveur qui ne gère pas les USERS doit retourner cette valeur.

# define  ERR_NOTREGISTERED 451
// ":You have not registered"
// Retourné par le serveur pour indiquer à un client qu'il doit être enregistré avant que ses commandes soient traitées.

# define  ERR_NEEDMOREPARAMS(command) (std::string(command) + ": Not enough parameters\r\n")
// 461
// "<commande> :Not enough parameters"
// Renvoyé par un serveur par de nombreuses commandes, afin d'indiquer que le client n'a pas fourni assez de paramètres.

# define  ERR_ALREADYREGISTRED ":You may not reregister\r\n"
// 462
// ":You may not reregister"
// Retourné par le serveur à tout lien qui tente de changer les détails enregistrés (tels que mot de passe et détails utilisateur du second message USER)

# define  ERR_NOPERMFORHOST 463
// ":Your host isn't among the privileged"
// Renvoyé à un client qui essaie de s'enregistrer sur un serveur qui n'accepte pas les connexions depuis cet hôte.

# define  ERR_PASSWDMISMATCH ":Password incorrect\r\n"
// 464
// ":Password incorrect"
// Retourné pour indiquer l'échec d'une tentative d'enregistrement d'une connexion dû à un mot de passe incorrect ou manquant.

# define  ERR_YOUREBANNEDCREEP 465
// ":You are banned from this server"
// Retourné après une tentative de connexion et d'enregistrement sur un serveur configuré explicitement pour vous refuser les connexions.

# define  ERR_KEYSET 467
// "<canal> :Channel key already set"
// Clé de canal déjà définie.

# define  ERR_CHANNELISFULL 471
// "<canal> :Cannot join channel (+l)"
// Impossible de joindre le canal (+l)

# define  ERR_UNKNOWNMODE 472
// "<caractère> :is unknown mode char to me"
// Mode inconnu.

# define  ERR_INVITEONLYCHAN 473
// "<canal> :Cannot join channel (+i)"
// Impossible de joindre le canal (+i).

# define  ERR_BANNEDFROMCHAN 474
// "<canal> :Cannot join channel (+b)"
// Impossible de joindre le canal (+b).

# define  ERR_BADCHANNELKEY 475
// "<canal> :Cannot join channel (+k)"
// Impossible de joindre le canal (+k).

# define  ERR_NOPRIVILEGES 481
// ":Permission Denied- You're not an IRC operator"
// Toute commande qui requiert le privilège d'opérateur pour opérer doit retourner cette erreur pour indiquer son échec.

# define ERR_CHANOPRIVSNEEDED 482
// "<canal> :You're not channel operator"
// Toute commande qui requiert les privilèges 'chanop' (tels les messages MODE) doit retourner ce message à un client qui l'utilise sans être chanop sur le canal spécifié.

# define ERR_CANTKILLSERVER 483
// ":You cant kill a server!"
// Toute tentative d'utiliser la commande KILL sur un serveur doit être refusée et cette erreur renvoyée directement au client.

# define ERR_NOOPERHOST 491
// ":No O-lines for your host"
// Si un client envoie un message OPER et que le serveur n'a pas été configuré pour autoriser les connexions d'opérateurs de cet hôte, cette erreur doit être retournée.

# define ERR_UMODEUNKNOWNFLAG 501
// ":Unknown MODE flag"
// Renvoyé par un serveur pour indiquer que le message MODE a été envoyé avec un pseudonyme et que le mode spécifié n'a pas été identifié.

# define ERR_USERSDONTMATCH 502
// ":Cant change mode for other users"
// Erreur envoyée à tout utilisateur qui essaie de voir ou de modifier le mode utilisateur d'un autre client.


/******		COMMAND REPLIES		******/

# define RPL_NONE 300
// Numéro de réponse bidon. Inutilisé.

# define RPL_USERHOST 302
// ":[<réponse>{<espace><réponse>}]"
// Format de réponse utilisé par USERHOST pour lister les réponses à la liste des requêtes. La chaîne de réponse est composée ainsi :
// <réponse> ::= <pseudo>['*'] '=' <'+'|'-'><hôte>
// Le '*' indique si le client est enregistré comme opérateur. Les caractères '-' ou '+' indiquent respectivement si le client a défini un message AWAY ou non.

# define RPL_ISON 303
// ":[<pseudo> {<espace><pseudo>}]"
// Format de réponse utilisé par ISON pour lister les réponses à la liste de requête.

# define RPL_AWAY 301
// "<pseudo> :<message d'absence>"

# define RPL_UNAWAY 305
// ":You are no longer marked as being away"

# define RPL_NOWAWAY 306
// ":You have been marked as being away"
// Ces trois réponses sont utilisées avec la commande AWAY (si elle est autorisée). RPL_AWAY est envoyé à tout client qui envoie un PRIVMSG à un client absent. RPL_AWAY n'est envoyé que par le serveur sur lequel le client est connecté. Les réponses RPL_UNAWAY et RPL_NOWAWAY sont envoyées quand un client retire et définit un message AWAY.

# define RPL_WHOISUSER 311
// "<pseudo> <utilisateur> <hôte> * :<vrai nom>"

# define RPL_WHOISSERVER 312 
// "<pseudo> <serveur> :<info serveur>"

# define RPL_WHOISOPERATOR 313
// "<pseudo> :is an IRC operator"

# define  RPL_WHOISIDLE 317
// "<pseudo> <integer> :seconds idle"

# define RPL_ENDOFWHOIS 318
// "<pseudo> :End of /WHOIS list"

# define RPL_WHOISCHANNELS 319
// "<pseudo> :{[@|+]<canal><espace>}"
// Les réponses 311 à 313 et 317 à 319 sont toutes générées en réponse à un message WHOIS. S'il y a assez de paramètres, le serveur répondant doit soit formuler une réponse parmi les numéros ci-dessus (si le pseudo recherché a été trouvé) ou renvoyer un message d'erreur. Le '*' dans RPL_WHOISUSER est là en tant que littéral et non en tant que joker. Pour chaque jeu de réponses, seul RPL_WHOISCHANNELS peut apparaître plusieurs fois (pour les longues listes de noms de canaux). Les caractères '@' et '+' à côté du nom de canal indiquent si un client est opérateur de canal, ou si on l'a autorisé à parler dans un canal modéré. La réponse RPL_ENDOFWHOIS est utilisée pour marquer la fin de la réponse WHOIS.

# define RPL_WHOWASUSER 314
// "<pseudo> <utilisateur> <hôte> * :<vrai nom>"

# define  RPL_ENDOFWHOWAS 369
// "<pseudo> :End of WHOWAS"
// Lorsqu'il répond à un message WHOWAS, un serveur doit utiliser RPL_WHOWASUSER, RPL_WHOISSERVER ou ERR_WASNOSUCHNICK pour chacun des pseudonymes de la liste fournie. A la fin de toutes les réponses, il doit y avoir un RPL_ENDOFWHOWAS (même s'il n'y a eu qu'une réponse, et que c'était une erreur).

# define RPL_LISTSTART 321
// "Channel :Users Name"

# define RPL_LIST 322
// "<canal> <# visible> :<sujet>"

# define  RPL_LISTEND 323
// ":End of /LIST"
// Les réponses RPL_LISTSTART, RPL_LIST, RPL_LISTEND marquent le début, les réponses proprement dites, et la fin du traitement d'une commande LIST. S'il n'y a aucun canal disponible, seules les réponses de début et de fin sont envoyées.

# define  RPL_CHANNELMODEIS 324
// "<canal> <mode> <paramètres de mode>"
# define RPL_NOTOPIC 331
// "<canal> :No topic is set"
# define RPL_TOPIC 332
// "<canal> :<sujet>"
// Lors de l'envoi d'un message TOPIC pour déterminer le sujet d'un canal, une de ces deux réponses est envoyée. Si le sujet est défini, RPL_TOPIC est renvoyée, sinon c'est RPL_NOTOPIC.
# define RPL_INVITING 341
// "<canal> <pseudo>"
// Renvoyé par un serveur pour indiquer que le message INVITE a été enregistré, et est en cours de transmission au client final.

# define RPL_VERSION 351
// "<version>.<debuglevel> <serveur> :<commentaires>"
// Réponse du serveur indiquant les détails de sa version. <version> est la version actuelle du programme utilisé (comprenant le numéro de mise à jour) et <debuglevel> est utilisé pour indiquer si le serveur fonctionne en mode débugage.
// Le champ <commentaire> peut contenir n'importe quel commentaire au sujet de la version, ou des détails supplémentaires sur la version.
# define RPL_WHOREPLY 352
// "<canal> <utilisateur> <hôte> <serveur> <pseudo> <H|G>[*][@|+] :<compteur de distance> <vrai nom>"
# define RPL_ENDOFWHO 315
// "<nom> :End of /WHO list"
// La paire RPL_WHOREPLY et RPL_ENDOFWHO est utilisée en réponse à un message WHO. Le RPL_WHOREPLY n'est envoyé que s'il y a une correspondance à la requête WHO. S'il y a une liste de paramètres fournie avec le message WHO, un RPL_ENDOFWHO doit être envoyé après le traitement de chaque élément de la liste, <nom> étant l'élément.

# define RPL_BANLIST 367
// "<canal> <identification de bannissement>"
# define RPL_ENDOFBANLIST 368
// "<canal> :End of channel ban list"
// Quand il liste les bannissements actifs pour un canal donné, un serveur doit renvoyer la liste en utilisant les messages RPL_BANLIST et RPL_ENDOFBANLIST. Un RPL_BANLIST différent doit être utilisé pour chaque identification de bannissement. Après avoir listé les identifications de bannissement (s'il y en a), un RPL_ENDOFBANLIST doit être renvoyé.

# define RPL_INFO 371
// ":<chaîne>"
# define RPL_ENDOFINFO 374
// ":End of /INFO list"
// Un serveur répondant à un message INFO doit envoyer toute sa série d'info en une suite de réponses RPL_INFO, avec un RPL_ENDOFINFO pour indiquer la fin des réponses.

# define RPL_MOTDSTART 375
// ":- <serveur> Message of the day - "
# define RPL_MOTD 372
// ":- <texte>"
# define RPL_ENDOFMOTD 376
// ":End of /MOTD command"
// Lorsqu'il répond à un message MOTD et que le fichier MOTD est trouvé, le fichier est affiché ligne par ligne, chaque ligne ne devant pas dépasser 80 caractères, en utilisant des réponses au format RPL_MOTD. Celles-ci doivent être encadrées par un RPL_MOTDSTART (avant les RPL_MOTDs) et un RPL_ENDOFMOTD (après).

# define RPL_YOUREOPER 381
// ":You are now an IRC operator"
// RPL_YOUREOPER est renvoyé à un client qui vient d'émettre un message OPER et a obtenu le statut d'opérateur.

# define RPL_USERSSTART 392
// ":UserID Terminal Hôte"
# define RPL_USERS 393
// ":%-8s %-9s %-8s"
# define RPL_ENDOFUSERS 394
// ":End of users"
# define RPL_NOUSERS 395
// ":Nobody logged in"
// Si le message USERS est géré par un serveur, les réponses RPL_USERSTART, RPL_USERS, RPL_ENDOFUSERS et RPL_NOUSERS sont utilisées. RPL_USERSSTART doit être envoyé en premier, suivi par soit une séquence de RPL_USERS soit un unique RPL_NOUSER. Enfin, vient un RPL_ENDOFUSERS.

#endif