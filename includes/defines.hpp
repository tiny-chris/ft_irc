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
# define USERLEN 18

// Password maximun length (incl. '\r\n', CR-LN) --> TO BE CONFIRMED
# define PASSMAXLEN 100

// Buffer maximun length (incl. '\r\n', CR-LN)
# define BUFMAXLEN 512

// Max number of clients connected (such as max fds 'openable')
# define MAXCONNECTION 128// ou CLIMAXCONNECTION

// Max number of clients on a channel --> TO BE CONFIRMED
# define MAXONCHANNEL 10// ou CLIMAXONCHANNEL


// # define CHANNAMELEN 100


#endif /* __DEFINES_HPP__ */ 