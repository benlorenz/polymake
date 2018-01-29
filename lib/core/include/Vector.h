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

/** @file Vector.h
    @brief Implementation of pm::Vector class
 */

#ifndef POLYMAKE_VECTOR_H
#define POLYMAKE_VECTOR_H

#include "polymake/GenericVector.h"
#include "polymake/internal/shared_object.h"

namespace pm {

/// @ref vector_sec "Vector type" class which holds the elements in a contiguous array
template <typename E>
class Vector
   : public GenericVector<Vector<E>, E>
   , public plain_array<Vector<E>, E> {
protected:
   typedef shared_array<E, AliasHandlerTag<shared_alias_handler>> shared_array_type;
   shared_array_type data;

   friend Vector& make_mutable_alias(Vector& alias, Vector& owner)
   {
      alias.data.make_mutable_alias(owner.data);
      return alias;
   }

   friend class plain_array<Vector<E>, E>;

   E* get_data() { return *data; }
   const E* get_data() const { return *data; }

public:
   using typename GenericVector<Vector>::generic_type;

   /// create as empty
   Vector() {}

   /// create vector of length n
   explicit Vector(int n)
      : data(n) {}

   template <typename E2>
   Vector(int n, const E2& init,
          typename std::enable_if<can_initialize<E2, E>::value, void**>::type=nullptr)
      : data(n, init) {}

   template <typename Iterator>
   Vector(int n, Iterator&& src,
          typename std::enable_if<assess_iterator_value<Iterator, can_initialize, E>::value, void**>::type=nullptr)
      : data(n, ensure_private_mutable(std::forward<Iterator>(src))) {}

   template <typename Iterator>
   Vector(Iterator&& src, Iterator&& src_end,
          typename std::enable_if<assess_iterator_value<Iterator, can_initialize, E>::value, void**>::type=nullptr)
      : data(std::distance(src, src_end), ensure_private_mutable(std::forward<Iterator>(src))) {}

   template <typename Container>
   explicit Vector(const Container& src,
                   typename std::enable_if<isomorphic_to_container_of<Container, E, is_vector>::value, void**>::type=nullptr)
      : data(src.size(), ensure(src, (dense*)0).begin()) {}

   Vector(const GenericVector<Vector>& v) : data(v.top().data) {}

   template <typename Vector2>
   Vector(const GenericVector<Vector2, E>& v)
      : data(v.dim(), ensure(v.top(), (dense*)0).begin()) {}

   template <typename Vector2, typename E2>
   explicit Vector(const GenericVector<Vector2, E2>& v,
                   typename std::enable_if<can_initialize<E2, E>::value, void**>::type=nullptr)
      : data(v.dim(), ensure(v.top(), (dense*)0).begin()) {}

   template <typename E2,
             typename=typename std::enable_if<can_initialize<E2, E>::value>::type>
   Vector(std::initializer_list<E2> l)
      : data(l.size(), l.begin()) {}

protected:
   template <typename... TArgs>
   Vector(const shared_array_placement& place, int n, TArgs&&... args)
      : data(place, n, std::forward<TArgs>(args)...) {}

   void resize(const shared_array_placement& place, int n) { data.resize(place, n); }

public:
   /// number of elements
   int size() const { return data.size(); }

   /// truncate to zero size
   void clear() { data.clear(); }

   /// change the size, initialize appended elements with default constructor
   void resize(int n) { data.resize(n); }

   template <typename E2,
             typename=typename std::enable_if<can_initialize<E2, E>::value>::type>
   void assign(int n, const E2& x)
   {
      data.assign(n, x);
   }

   Vector& operator= (const Vector& other) { assign(other); return *this; }
   using generic_type::operator=;

   void swap(Vector& v) { data.swap(v.data); }

   friend void relocate(Vector* from, Vector* to)
   {
      relocate(&from->data, &to->data);
   }

   /// append a GenericVector
   template <typename Vector2, typename E2,
             typename=typename std::enable_if<can_initialize<E2, E>::value>::type>
   Vector& operator|= (const GenericVector<Vector2, E2>& v)
   {
      data.append(v.dim(), ensure(v.top(), (dense*)0).begin());
      return *this;
   }

   /// append an element
   template <typename E2,
             typename=typename std::enable_if<can_initialize<E2, E>::value>::type>
   Vector& operator|= (E2&& r)
   {
      data.append(1, std::forward<E2>(r));
      return *this;
   }

   template <typename E2,
             typename=typename std::enable_if<can_initialize<E2, E>::value>::type>
   Vector& operator|= (std::initializer_list<E2> l)
   {
      data.append(l.size(), l.begin());
   }

protected:
   template <typename, typename> friend class GenericVector;

   void assign(const Vector& v) { data=v.data; }

   template <typename Container>
   void assign(const Container& c)
   {
      data.assign(get_dim(c), ensure(c, (dense*)0).begin());
   }

   template <typename Operation>
   void assign_op(const Operation& op)
   {
      data.assign_op(op);
   }

   template <typename Container, typename Operation>
   void assign_op(const Container& c, const Operation& op)
   {
      data.assign_op(ensure(c, (dense*)0).begin(), op);
   }

   template <typename E2>
   void fill_impl(const E2& x, dense)
   {
      data.assign(data.size(), x);
   }
};

template <typename TVector, typename E, typename Permutation> inline
typename std::enable_if<!TVector::is_sparse, Vector<E>>::type
permuted(const GenericVector<TVector, E>& v, const Permutation& perm)
{
   if (POLYMAKE_DEBUG || !Unwary<TVector>::value) {
      if (v.dim() != int(perm.size()))
         throw std::runtime_error("permuted - dimension mismatch");
   }
   return Vector<E>(v.dim(), select(v.top(), perm).begin());
}

template <typename TVector, typename E, typename Permutation> inline
typename std::enable_if<!TVector::is_sparse, Vector<E>>::type
permuted_inv(const GenericVector<TVector, E>& v, const Permutation& perm)
{
   if (POLYMAKE_DEBUG || !Unwary<TVector>::value) {
      if (v.dim() != int(perm.size()))
         throw std::runtime_error("permuted_inv - dimension mismatch");
   }
   Vector<E> result(v.dim());
   copy_range(entire(v.top()), select(result, perm).begin());
   return result;
}

} // end namespace pm

namespace polymake {
   using pm::Vector;
}

namespace std {
   template <typename E> inline
   void swap(pm::Vector<E>& v1, pm::Vector<E>& v2) { v1.swap(v2); }
}

#endif // POLYMAKE_VECTOR_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
