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

///==== this line controls the automatic file splitting: max.instances=40

#include "polymake/IncidenceMatrix.h"
#include "polymake/Integer.h"
#include "polymake/Matrix.h"
#include "polymake/QuadraticExtension.h"
#include "polymake/Rational.h"
#include "polymake/Vector.h"
#include "polymake/client.h"
#include "polymake/linalg.h"

namespace polymake { namespace common { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0>
   FunctionInterface4perl( repeat_row_X32_x, T0 ) {
      perl::Value arg0(stack[0]), arg1(stack[1]);
      WrapperReturn( (repeat_row(arg0.get<T0>(), arg1)), arg0 );
   };

   FunctionInstance4perl(repeat_row_X32_x, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Rational>&>, pm::Series<int, true>, mlist<> > >);
   FunctionInstance4perl(repeat_row_X32_x, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Integer> const&>, pm::Series<int, true>, mlist<> > >);
   FunctionInstance4perl(repeat_row_X32_x, perl::Canned< const Vector< Rational > >);
   FunctionInstance4perl(repeat_row_X32_x, perl::Canned< const Vector< double > >);
   FunctionInstance4perl(repeat_row_X32_x, perl::Canned< const pm::IndexedSlice<pm::masquerade<pm::ConcatRows, pm::Matrix_base<pm::Integer>&>, pm::Series<int, true>, mlist<> > >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
