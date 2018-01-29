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

#include "polymake/Array.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/Integer.h"
#include "polymake/Matrix.h"
#include "polymake/Polynomial.h"
#include "polymake/PuiseuxFraction.h"
#include "polymake/QuadraticExtension.h"
#include "polymake/Rational.h"
#include "polymake/TropicalNumber.h"
#include "polymake/Vector.h"
#include "polymake/client.h"
#include "polymake/linalg.h"

namespace polymake { namespace common { namespace {
///==== Automatically generated contents follow.    Please do not delete this line. ====
   template <typename T0, typename T1>
   FunctionInterface4perl( new_X, T0,T1 ) {
      perl::Value arg0(stack[1]);
      WrapperReturnNew(T0, (arg0.get<T1>()) );
   };

   template <typename T0, typename T1, typename T2>
   FunctionInterface4perl( new_X_X, T0,T1,T2 ) {
      perl::Value arg0(stack[1]), arg1(stack[2]);
      WrapperReturnNew(T0, (arg0.get<T1>(), arg1.get<T2>()) );
   };

   template <typename T0>
   FunctionInterface4perl( new, T0 ) {
      WrapperReturnNew(T0, () );
   };

   FunctionInstance4perl(new, Polynomial< PuiseuxFraction< Min, Rational, Rational >, int >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const Polynomial< PuiseuxFraction< Min, Rational, Rational >, int > >, perl::Canned< const Polynomial< PuiseuxFraction< Min, Rational, Rational >, int > >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, perl::Canned< const Array< int > >, perl::Canned< const Matrix< int > >);
   OperatorInstance4perl(BinaryAssign_mul, perl::Canned< Polynomial< Rational, int > >, perl::Canned< const Polynomial< Rational, int > >);
   OperatorInstance4perl(Unary_neg, perl::Canned< const Polynomial< Rational, int > >);
   OperatorInstance4perl(Binary_add, int, perl::Canned< const Polynomial< Rational, int > >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, int, int);
   FunctionInstance4perl(new_X_X, Polynomial< TropicalNumber< Min, Rational >, int >, perl::Canned< const pm::SameElementVector<pm::TropicalNumber<pm::Min, pm::Rational> const&> >, perl::Canned< const pm::DiagMatrix<pm::SameElementVector<int const&>, true> >);
   FunctionInstance4perl(new_X_X, Polynomial< TropicalNumber< Max, Rational >, int >, perl::Canned< const pm::SameElementVector<pm::TropicalNumber<pm::Max, pm::Rational> const&> >, perl::Canned< const pm::DiagMatrix<pm::SameElementVector<int const&>, true> >);
   FunctionInstance4perl(new_X_X, Polynomial< TropicalNumber< Max, Rational >, int >, perl::Canned< const TropicalNumber< Max, Rational > >, int);
   FunctionInstance4perl(new_X_X, Polynomial< TropicalNumber< Min, Rational >, int >, perl::Canned< const TropicalNumber< Min, Rational > >, int);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, perl::Canned< const pm::SameElementVector<pm::Rational const&> >, perl::Canned< const pm::MatrixMinor<pm::Matrix<int>&, pm::Array<int> const&, pm::all_selector const&> >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Polynomial< TropicalNumber< Min, Rational >, int > >, perl::Canned< const Polynomial< TropicalNumber< Min, Rational >, int > >);
   OperatorInstance4perl(Binary_div, perl::Canned< const Polynomial< Rational, int > >, int);
   OperatorInstance4perl(Binary__ne, perl::Canned< const Polynomial< Rational, int > >, int);
   OperatorInstance4perl(Binary_div, perl::Canned< const Polynomial< Rational, int > >, perl::Canned< const Rational >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, int, perl::Canned< const pm::SameElementVector<int const&> >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, int, perl::Canned< const pm::SameElementSparseVector<pm::SingleElementSetCmp<int, pm::operations::cmp>, int> >);
   Class4perl("Polymake::common::Polynomial_A_QuadraticExtension__Rational_I_Int_Z", Polynomial< QuadraticExtension< Rational >, int >);
   FunctionInstance4perl(new_X_X, Polynomial< QuadraticExtension< Rational >, int >, perl::Canned< const QuadraticExtension< Rational > >, perl::Canned< const pm::SameElementVector<int const&> >);
   OperatorInstance4perl(Binary_xor, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >, int);
   OperatorInstance4perl(BinaryAssign_add, perl::Canned< Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >);
   OperatorInstance4perl(Binary__ne, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >);
   OperatorInstance4perl(Binary_div, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const QuadraticExtension< Rational > >);
   FunctionInstance4perl(new_X_X, Polynomial< QuadraticExtension< Rational >, int >, perl::Canned< const QuadraticExtension< Rational > >, perl::Canned< const pm::SameElementSparseVector<pm::SingleElementSetCmp<int, pm::operations::cmp>, int> >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, perl::Canned< const Rational >, perl::Canned< const pm::SameElementVector<int const&> >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, perl::Canned< const Rational >, perl::Canned< const pm::SameElementSparseVector<pm::SingleElementSetCmp<int, pm::operations::cmp>, int> >);
   OperatorInstance4perl(Binary__ne, perl::Canned< const Polynomial< Rational, int > >, perl::Canned< const Polynomial< Rational, int > >);
   OperatorInstance4perl(BinaryAssign_mul, perl::Canned< Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >);
   OperatorInstance4perl(BinaryAssign_sub, perl::Canned< Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const QuadraticExtension< Rational > >);
   OperatorInstance4perl(BinaryAssign_sub, perl::Canned< Polynomial< Rational, int > >, perl::Canned< const Rational >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, perl::Canned< const Vector< Rational > >, perl::Canned< const Matrix< int > >);
   FunctionInstance4perl(new_X, Polynomial< Rational, int >, perl::Canned< const Polynomial< Rational, int > >);
   FunctionInstance4perl(new_X_X, Polynomial< Rational, int >, perl::Canned< const Vector< Rational > >, perl::Canned< const pm::ColChain<pm::SingleCol<pm::Vector<int> const&>, pm::Matrix<int> const&> >);
   OperatorInstance4perl(Binary_add, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >);
   OperatorInstance4perl(Binary_mul, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >);
   FunctionInstance4perl(new, Polynomial< QuadraticExtension< Rational >, int >);
   OperatorInstance4perl(Binary__eq, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >, perl::Canned< const Polynomial< QuadraticExtension< Rational >, int > >);
   OperatorInstance4perl(Binary_add, perl::Canned< const Polynomial< TropicalNumber< Min, Rational >, int > >, perl::Canned< const Polynomial< TropicalNumber< Min, Rational >, int > >);
///==== Automatically generated contents end here.  Please do not delete this line. ====
} } }
