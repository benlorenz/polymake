/* Copyright (c) 1997-2018
   Ewgenij Gawrilow, Michael Joswig (Technische Universitaet Berlin, Germany)
   http://www.polymake.org

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

#include "polymake/client.h"
#include "polymake/graph/Lattice.h"
#include "polymake/graph/Decoration.h"

namespace polymake { namespace topaz {

using graph::Lattice;
using graph::lattice::BasicDecoration;

Array< Set<int> > facets_from_hasse_diagram(perl::Object p)
{
   const Lattice<BasicDecoration> HD(p);
   Array< Set<int> > F( attach_member_accessor( select(HD.decoration(), HD.in_adjacent_nodes(HD.top_node())),
               ptr2type<BasicDecoration, Set<int>,&BasicDecoration::face>()
               ));
   return F;
}

Function4perl(&facets_from_hasse_diagram, "facets_from_hasse_diagram(Lattice<BasicDecoration>)");

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
