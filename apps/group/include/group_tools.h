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

#ifndef __GROUP_TOOLS_H
#define __GROUP_TOOLS_H

#include "polymake/group/action.h"
#include <polymake/QuadraticExtension.h>
#include "polymake/hash_set"

namespace polymake { namespace group {

typedef Array<Array<int>> GroupRightMultiplicationTable;
typedef Array<Array<int>> GroupLeftMultiplicationTable;
typedef Array<int> GroupRightMultiplicationTableRow;
typedef Array<int> GroupLeftMultiplicationTableRow;

template<typename Element=Array<int>>
using GroupIndex = hash_map<Element, int>;

template<typename Element=Array<int>>
using ConjugacyClass = Array<Element>;
      
template<typename Element=Array<int>>
using ConjugacyClasses = Array<ConjugacyClass<Element>>;

template<typename Element=Array<int>>
using ConjugacyClassReps = Array<Element>;

// choose the right parametrization of 'action' for permutation, matrix and set actions

template<typename Element>
struct action_choice;

template<>
struct action_choice<Array<int>> {
   typedef Array<int>& OpRef;
   typedef Array<int> Perm; // Perm and Element are the same because the group acts on itself
   typedef on_container action_type;
   typedef pm::is_container op_tag;
   typedef pm::is_container perm_tag;
   typedef std::false_type stores_ref;
};

template<typename Scalar>
struct action_choice<Matrix<Scalar>> {
   typedef Matrix<Scalar>& OpRef;
   typedef Matrix<Scalar> Perm; // Perm and Element are the same because the group acts on itself
   typedef on_elements action_type;
   typedef pm::is_matrix op_tag;
   typedef pm::is_matrix perm_tag;
   typedef std::false_type stores_ref;
};

template<typename SetType>
struct action_choice<GenericSet<SetType>> {
   typedef GenericSet<SetType>& OpRef;
   typedef Array<int> Perm; 
   typedef on_container action_type;
   typedef pm::is_container op_tag;
   typedef pm::is_container perm_tag;
   typedef std::false_type stores_ref;
};

      
template<typename Element>
using ac = action_choice<typename pm::deref<Element>::type>;

template<typename Element>
using ConjugationAction = pm::operations::group::conjugation_action
   <typename ac<Element>::OpRef, typename ac<Element>::action_type, typename ac<Element>::Perm,
    typename ac<Element>::op_tag, typename ac<Element>::perm_tag, typename ac<Element>::stores_ref>;

template<typename Element>
using LeftAction = pm::operations::group::action
   <typename ac<Element>::OpRef, typename ac<Element>::action_type, typename ac<Element>::Perm,
    typename ac<Element>::op_tag, typename ac<Element>::perm_tag>;
      
template<typename SetType>
using SetAction = pm::operations::group::action
   <typename ac<GenericSet<SetType>>::OpRef, typename ac<GenericSet<SetType>>::action_type, typename ac<GenericSet<SetType>>::Perm,
    typename ac<GenericSet<SetType>>::op_tag, typename ac<GenericSet<SetType>>::perm_tag>;

// more datatypes
      
template<typename SparseSet>
using SparseSimplexVector = hash_map<SparseSet, Rational>;

template<typename SparseSet>
using SparseIsotypicBasis = Array<SparseSimplexVector<SparseSet>>;

template<typename SetType>
using ActionType = pm::operations::group::action<SetType&, group::on_container, Array<int>>;

typedef QuadraticExtension<Rational> CharacterNumberType;


template<typename Element>
GroupIndex<Element>
group_index(const ConjugacyClasses<Element>& cc)
{
   int n(0);
   GroupIndex<Element> index_of;
   for (const auto& c : cc)
      for (const auto& g : c)
         index_of[g] = n++;
   return index_of;
}

// GMT[g][h] = gh
template<typename Element>
GroupRightMultiplicationTable
group_right_multiplication_table_impl(const ConjugacyClasses<Element>& cc,
                                      const GroupIndex<Element>& index_of)
{
   const int n(index_of.size());
   GroupRightMultiplicationTable GMT(n);
   for (int i=0; i<n; ++i)
      GMT[i].resize(n);
   
   int i(0);
   for (const auto& c : cc) {
      for (const auto& g : c) {
         const LeftAction<Element> a(g);
         int j(-1);
         for (const auto& c1 : cc) {
            for (const auto& g1 : c1) {
               // fill the transpose of the table because of the way the action is defined
               GMT[++j][i] = index_of.at(a(g1));
            }
         }
         ++i;
      }
   }
   return GMT;
}

// GMT[g][h] = hg
template<typename Element>
GroupLeftMultiplicationTable
group_left_multiplication_table_impl(const ConjugacyClasses<Element>& cc,
                                     const GroupIndex<Element>& index_of)
{
   const int n(index_of.size());
   GroupLeftMultiplicationTable GMT(n);
   
   int i(-1);
   for (const auto& c : cc) {
      for (const auto& g : c) {
         const LeftAction<Element> a(g);
         int j(-1);
         GroupLeftMultiplicationTableRow r(n);
         for (const auto& c1 : cc)
            for (const auto& g1 : c1)
               r[++j] = index_of.at(a(g1));
         GMT[++i] = r;
      }
   }
   return GMT;
}

template<typename IndexedGroup>
Array<int>
partition_representatives_impl(const IndexedGroup& H,
                               const GroupRightMultiplicationTable& GMT)
{
   const int n(GMT.size()/H.size());
   Array<int> reps(n);
   hash_set<int> remaining(sequence(0, GMT.size()));
   Entire<Array<int>>::iterator rit = entire(reps);
   while (!remaining.empty()) {
      const int r = *remaining.begin();
      *rit = r; ++rit;
      const GroupRightMultiplicationTableRow& GMT_r(GMT[r]);
      for (const auto& h : H) {
#if POLYMAKE_DEBUG
         if (!remaining.contains(GMT_r[h])) throw std::runtime_error("partition_representatives: could not find expected element");
#endif
         remaining -= GMT_r[h];
      }
#if POLYMAKE_DEBUG
      if (!remaining.empty() && rit.at_end()) throw std::runtime_error("partition_representatives: incorrect number of reps");
#endif
   }
   return reps;
}


} }

#endif // __GROUP_TOOLS_H


// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:


