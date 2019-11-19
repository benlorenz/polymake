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

#ifndef POLYMAKE_POLYTOPE_HYPERSIMPLEX_H
#define POLYMAKE_POLYTOPE_HYPERSIMPLEX_H

#include "polymake/client.h"

namespace polymake { namespace polytope {

      perl::Object hypersimplex(int d, int k, perl::OptionSet options);

} }

#endif // POLYMAKE_POLYTOPE_HYPERSIMPLEX_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
