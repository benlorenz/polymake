/* Copyright (c) 1997-2019
   Ewgenij Gawrilow, Michael Joswig, and the polymake team
   Technische Universität Berlin, Germany
   https://polymake.org

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version: http://www.gnu.org/licenses/gpl.txt.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
--------------------------------------------------------------------------------
*/

#ifndef POLYMAKE_INTERNAL_GMPXX_TRAITS_H
#define POLYMAKE_INTERNAL_GMPXX_TRAITS_H

#include "polymake/internal/iterators.h"

namespace pm {

// __gmp_expr::value_type is private, can't be tested with normal SFINAE tricks
template <typename T1, typename T2>
struct has_value_type<__gmp_expr<T1, T2>> : std::false_type {};

} // end namespace pm

#endif // POLYMAKE_INTERNAL_GMPXX_TRAITS_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
