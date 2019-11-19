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

/** @file Set.h
    @brief Implementation of pm::ApproximateSet class
*/



#ifndef POLYMAKE_APPROXIMATE_SET_H
#define POLYMAKE_APPROXIMATE_SET_H

#include "polymake/Set.h"

namespace pm {

template <typename E>
using ApproximateSet = Set<E, operations::cmp_with_leeway>;

} // end namespace pm

namespace polymake {
   using pm::ApproximateSet;
}

#endif // POLYMAKE_APPROXIMATE_SET_H


// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
