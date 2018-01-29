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
#include "polymake/group/orbit.h"

namespace polymake { namespace group { 

template<typename Scalar>
auto
orbit(const Array<Matrix<Scalar>>& gens, const Vector<Scalar>& v)
{
  return orbit<on_elements, Matrix<Scalar>, Vector<Scalar>, hash_set<Vector<Scalar>>, pm::is_vector, pm::is_matrix>(gens, v);
}

UserFunctionTemplate4perl("# @category Utilities\n"
                         "# The image of an object //O// under a group element //g//."
                         "# @param Any g Group element"
                         "# @param Object O"
			 "# @tparam action_type one of: [[on_container]], [[on_elements]], [[on_rows]], [[on_cols]], [[on_nonhomog_cols]]"
                         "# @return Object",
                         "action<action_type=on_container>(*, *)");

UserFunctionTemplate4perl("# @category Utilities\n"
                         "# The image of an object //O// under the inverse of a"
                         "# permutation //p//."
                         "# @param Array<Int> p permutation"
                         "# @param Object O"
			 "# @tparam action_type one of: [[on_container]], [[on_elements]], [[on_rows]], [[on_cols]], [[on_nonhomog_cols]]"
                         "# @return Object",
                         "action_inv<action_type=on_container>(Array<Int>, *)");

UserFunctionTemplate4perl("# @category Orbits\n"
                         "# The orbit of an object //O// under a group generated by //G//."
                         "# @param Array G Group generators"
                         "# @param Object O"
			 "# @tparam action_type one of: [[on_container]], [[on_elements]], [[on_rows]], [[on_cols]], [[on_nonhomog_cols]]"
                         "# @return Set<Object>",
                         "orbit<action_type=on_container>(Array, *)");

InsertEmbeddedRule("# @category Orbits\n"
                  "# The orbit of an container //C// under a group//G//.\n"
                  "# @param Group G\n"
                  "# @param Any C\n"
                  "# @return Set<container>\n"
                   "user_function orbit<action_type=on_container>(PermutationAction, $) {\n"
                   "   orbit<action_type>($_[0]->GENERATORS, $_[1]);\n"
                   "}\n");

UserFunctionTemplate4perl("# @category Orbits\n"
			  "# The indices of one representative for each orbit under the group generated by //G//."
			  "# @param Array<GeneratorType> G Group generators"
			  "# @return Array<Int>",
			  "orbit_representatives<GeneratorType>(Array<GeneratorType>)");


UserFunctionTemplate4perl("# @category Orbits\n"
			  "# The orbit of a vector //V// under a group generated by //G//."
			  "# @param Array<Matrix<Scalar>> G Generators of the group"
			  "# @param Vector<Scalar> V"
			  "# @tparam Scalar S the number type"
			  "# @return Set",
			  "orbit<Scalar>(Array<Matrix<Scalar>>, Vector<Scalar>)");

} } // end namespaces
