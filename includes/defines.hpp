#ifndef DEFINES_HPP
# define DEFINES_HPP

/* **************************************** */
/*                  PARSING                 */
/* **************************************** */

/*	End of client message with "\r\n" sequence
**	The acronym CRLF means Carriage Return Line Feed
*/
# define CRLF std::string("\r\n")

// Maximum length of username (WITH OU WITHOUT CRLF ????? )
// The USERLEN parameter indicates the maximum length that a username may be on the server.
// Networks SHOULD be consistent with this value across different servers.
// As noted in the USER message, the tilde prefix ("~"), if it exists, contributes to the length of the username and would be included in this parameter.
// The value MUST be specified and MUST be a positive integer.
// # define USERLEN 18

// Password maximun length (incl. '\r\n', CR-LN) --> TO BE CONFIRMED
# define PASSMAXLEN 100

// Buffer maximun length (incl. '\r\n', CR-LN)
# define BUFMAXLEN 512

// Max number of clients connected (such as max fds 'openable')
# define MAXCONNECTION 128// ou CLIMAXCONNECTION

// Max number of clients on a channel --> TO BE CONFIRMED
# define MAXONCHANNEL 10// ou CLIMAXONCHANNEL

// # define CHANNAMELEN 100

/***************************************/
/*       RPL_ISUPPORT Parameters       */
/***************************************/

/* The CASEMAPPING parameter indicates what method the server uses to compare equality of case-insensitive strings
(such as channel names and nicks).*/
# define CASEMAPPING "ascii"

/* The CHANLIMIT parameter indicates the number of channels a client may join. */
# define CHANLIMIT 10

/* The CHANMODES parameter specifies the channel modes available and which types of arguments
they do or do not take when using them with the MODE command. */
# define CHANMODES ",k,l,it"

/* The CHANNELLEN parameter specifies the maximum length of a channel name that a client may join. */
# define CHANNELLEN 32

/* The CHANTYPES parameter indicates the channel prefix characters that are available on the current server. */
# define CHANTYPES "#"

/* The HOSTLEN parameter indicates the maximum length that a hostname may be on the server
(whether cloaked, spoofed, or a looked-up domain name) */
# define HOSTLEN 63

/* The KICKLEN parameter indicates the maximum length for the <reason> of a KICK command.*/
# define KICKLEN 255

/* The NICKLEN parameter indicates the maximum length of a nickname that a client may set.
Clients on the network MAY have longer nicks than this.*/
# define NICKLEN 9

/* Within channels, clients can have different statuses, denoted by single-character prefixes.
The PREFIX parameter specifies these prefixes and the channel mode characters that they are mapped to. */
# define PREFIX "(o)@"

/* The STATUSMSG parameter indicates that the server supports a method for clients to send a message via
the PRIVMSG / NOTICE commands to those people on a channel with (one of) the specified channel membership prefixes.*/
# define STATUSMSG "@"

/* The TOPICLEN parameter indicates the maximum length of a topic that a client may set on a channel.
Channels on the network MAY have topics with longer lengths than this.
The value MUST be specified and MUST be a positive integer.
307 is the typical value for this parameter advertised by servers today. */
# define TOPICLEN 307

/* The USERLEN parameter indicates the maximum length that a username may be on the server.
Networks SHOULD be consistent with this value across different servers.
The value MUST be specified and MUST be a positive integer. */
# define USERLEN 15

# define USERMODES "+i"
# define CHANNELMODES "+it"
# define CHANNELMODESPARAM "+kl"
# define SERVERNAME "localhost"
# define VERSION "1.69"

#endif /* __DEFINES_HPP__ */ 
