/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   deepCopyUtils.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cgaillag <cgaillag@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/07/19 14:48:25 by cgaillag          #+#    #+#             */
/*   Updated: 2023/07/19 14:58:20 by cgaillag         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEEP_COPY_UTILS_HPP
# define DEEP_COPY_UTILS_HPP

# include <vector>

template <typename T>
void	deepCopyVector( const std::vector< T* >& src, std::vector< T* >& dest );

// Inclusion du fichier de définition des fonctions membres
# include "deepCopyUtils.tpp"

#endif /* __DEEP_COPY_UTILS_HPP__ */
