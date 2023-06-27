#ifndef DEFINES_HPP
# define DEFINES_HPP

/* **************************************** */
/*                  PARSING                 */
/* **************************************** */

/*	End of client message with "\r\n" sequence
**	The acronym CRLF means Carriage Return Line Feed
*/
# define CRLF std::string("\r\n")



// Password maximun length (incl. '\r\n', CR-LN) --> TO BE CONFIRMED
# define MAXPASSLEN 100

// Buffer maximun length (incl. '\r\n', CR-LN)
# define MAXBUFLEN 512

// Max number of clients connected (such as max fds 'openable')
# define MAXCONNECTION 128

// Max number of clients on a channel --> TO BE CONFIRMED
# define MAXCHANCONNECTION 10


// # define CHANNAMELEN 100


#endif /* __DEFINES_HPP__ */ 