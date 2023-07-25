/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deepCopyUtils.tpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 14:48:25 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/19 15:09:58 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEEP_COPY_UTILS_TPP
# define DEEP_COPY_UTILS_TPP

# include "deepCopyUtils.hpp"

template <typename T>
void	deepCopyVector( const std::vector< T* >& src, std::vector< T* >& dest )
{
	std::vector< T* >	newVec;

	newVec.reserve( src.size() );
	for ( size_t i = 0; i < src.size(); ++i ) {
		T* ptr = src[ i ];
		// check if pointer is null
		if ( ptr == 0 ) {
			continue ;
		}
		//deep copy:
		T* newPtr = new T(*ptr);
		newVec.push_back( newPtr );
	}
	// checker double free...
	for ( size_t i = 0; i < src.size(); ++i ) {
		delete src [ i ];
	}
	dest = newVec;
	return ;
}

/* --------------------------------- OTHERS -------------------------------- */

// // use .tpp and call this file in the .hpp
// // OR
// // use an explicit instantiation part to list all possible format (not choosen as too many)

// template void deepCopyVector( const std::vector< Client::ChannelPtr >& src, std::vector< Client::ChannelPtr * >& dest )
// //...

#endif /* __DEEP_COPY_UTILS_TPP__ */
