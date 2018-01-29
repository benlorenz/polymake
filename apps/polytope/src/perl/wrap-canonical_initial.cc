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

#include "polymake/IncidenceMatrix.h"
#include "polymake/Integer.h"
#include "polymake/Matrix.h"
#include "polymake/PuiseuxFraction.h"
#include "polymake/QuadraticExtension.h"
#include "polymake/Rational.h"
#include "polymake/SparseMatrix.h"
#include "polymake/SparseVector.h"
#include "polymake/TropicalNumber.h"
#include "polymake/Vector.h"

namespace polymake { namespace polytope { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0>
   FunctionInterface4perl( add_extra_polytope_ineq_X2_f16, T0 ) {
      perl::Value arg0(stack[0]);
      WrapperReturnVoid( (add_extra_polytope_ineq(arg0.get<T0>())) );
   };

   template <typename T0>
   FunctionInterface4perl( canonicalize_polytope_generators_X2_f16, T0 ) {
      perl::Value arg0(stack[0]);
      WrapperReturnVoid( canonicalize_polytope_generators(arg0.get<T0>()) );
   };

   FunctionInstance4perl(canonicalize_polytope_generators_X2_f16, perl::Canned< Matrix< double > >);
   FunctionInstance4perl(canonicalize_polytope_generators_X2_f16, perl::Canned< Matrix< Rational > >);
   FunctionInstance4perl(canonicalize_polytope_generators_X2_f16, perl::Canned< SparseMatrix< Rational, NonSymmetric > >);
   FunctionInstance4perl(canonicalize_polytope_generators_X2_f16, perl::Canned< SparseMatrix< QuadraticExtension< Rational >, NonSymmetric > >);
   FunctionInstance4perl(canonicalize_polytope_generators_X2_f16, perl::Canned< Matrix< QuadraticExtension< Rational > > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< Matrix< double > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< Matrix< Rational > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< SparseMatrix< Rational, NonSymmetric > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< Matrix< QuadraticExtension< Rational > > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< SparseMatrix< double, NonSymmetric > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< SparseMatrix< QuadraticExtension< Rational >, NonSymmetric > >);
   FunctionInstance4perl(add_extra_polytope_ineq_X2_f16, perl::Canned< Matrix< PuiseuxFraction< Min, Rational, Rational > > >);
   FunctionInstance4perl(canonicalize_polytope_generators_X2_f16, perl::Canned< Matrix< PuiseuxFraction< Min, Rational, Rational > > >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
