#ifndef DEFINES_HPP
# define DEFINES_HPP

/* **************************************** */
/*                  PARSING                 */
/* **************************************** */

/*	End of client message with "\r\n" sequence
**	The acronym CRLF means Carriage Return Line Feed
*/
# define CRLF std::string("\r\n")



// Password maximun length (incl. '\r\n', CR-LN)
# define PASSMAXLEN 100

// Buffer maximun length (incl. '\r\n', CR-LN)
# define BUFMAXLEN 512

// Max number of clients connected
# define CONNECTEDMAXNB 128

// Max number of clients on a channel ??
# define CONNECTEDCHANMAX 10


// # define CHANNAMELEN 100


#endif /* __DEFINES_HPP__ */ 