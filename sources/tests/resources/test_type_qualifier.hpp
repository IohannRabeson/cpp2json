/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_type_qualifier.hpp                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: irabeson <irabeson@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2017/12/16 01:27:06 by irabeson          #+#    #+#             */
/*   Updated: 2017/12/16 01:31:56 by irabeson         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#if!defined TEST_TYPE_QUALIFIER_HPP
#define TEST_TYPE_QUALIFIER_HPP

struct TestTypeQualifier
{
    int raw;
    int* raw_pointer;
    int& reference;
    int const const_raw;
    int const* const_raw_pointer;
    int const& const_reference;
    int volatile volatile_raw;
    int const volatile const_volatile_raw;
};

#endif
