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
# define USERLEN 15

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