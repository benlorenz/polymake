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

#ifndef POLYMAKE_TOPAZ_HOMOLOGY_COMPLEX_H
#define POLYMAKE_TOPAZ_HOMOLOGY_COMPLEX_H

#include "polymake/list"
#include "polymake/Array.h"
#include "polymake/Set.h"
#include "polymake/Smith_normal_form.h"
#include "polymake/GenericStruct.h"
#include "polymake/topaz/SimplicialComplex_as_FaceMap.h"
#if POLYMAKE_DEBUG
#  include "polymake/client.h"
#  include "polymake/Rational.h"
#  include "polymake/linalg.h"
#endif

namespace polymake { namespace topaz {

template <typename E>
class HomologyGroup : public GenericStruct< HomologyGroup<E> > {
public:
   typedef std::list< std::pair<E,int> > torsion_list;
   DeclSTRUCT( DeclTemplFIELD(torsion, torsion_list)
               DeclTemplFIELD(betti_number, int) );
};

//saves bases and corresponding coefficients (of type E) for a complex
template <typename E>
class CycleGroup : public GenericStruct< CycleGroup<E> > {
public:
   typedef SparseMatrix<E> coeff_matrix;
   typedef Array< Set<int> > face_list;
   DeclSTRUCT( DeclTemplFIELD(coeffs, coeff_matrix)
               DeclTemplFIELD(faces, face_list) );
};

//dummy logger
class nothing_logger : public pm::dummy_companion_logger {
public:
   explicit nothing_logger(const nothing*, const nothing*, const nothing* =0, const nothing* =0) {}
};

//log operations that do not require gcd calculation??
template <typename E>
class elimination_logger : public pm::SNF_companion_logger<E,false> {
   // R is inverted
   typedef pm::SNF_companion_logger<E,false> super;
public:
   elimination_logger(SparseMatrix<E> *L, SparseMatrix<E> *Rinv) : super(L,Rinv) {}

   template <typename Matrix>
   void from_right(const Matrix& U) const { this->R->multiply_from_left(this->inv(U)); }
};

//logs companion matrices and elimination matrices
template <typename E>
class Smith_normal_form_logger
   : public pm::SNF_companion_logger<E,false> {
protected:
   typedef pm::SNF_companion_logger<E,false> super;
   elimination_logger<E> elim2;
public:
   Smith_normal_form_logger(SparseMatrix<E> *L, SparseMatrix<E> *L2, SparseMatrix<E> *R, SparseMatrix<E> *Rinv)
      : super(L,R), elim2(L2, Rinv) {}

   template <typename Matrix>
   void from_left(const Matrix& U) const  { super::from_left(U);  elim2.from_left(U); }
   template <typename Matrix>
   void from_right(const Matrix& U) const { if (super::R) super::from_right(U), elim2.from_right(U); }
};

template <typename E, typename MatrixType, typename BaseComplex, bool with_cycles, bool dual>
class Complex_iterator {
public:
   typedef std::forward_iterator_tag iterator_category;
   typedef HomologyGroup<E> value_type;
   typedef const value_type& reference;
   typedef const value_type* pointer;
   typedef ptrdiff_t difference_type;
   typedef Complex_iterator iterator;
   typedef Complex_iterator const_iterator;

   typedef SparseMatrix<E> matrix;
   typedef typename std::conditional<with_cycles, matrix, nothing>::type companion_type;

   Complex_iterator() {}

   //for cohomology start at the other end
   Complex_iterator(const BaseComplex& complex_arg, int d_start_arg, int d_end_arg)
      : complex(&complex_arg),
        d_cur(dual ? d_end_arg : d_start_arg+1),
        d_end(dual ? d_start_arg+1 : d_end_arg)
   {
#if POLYMAKE_DEBUG
      debug_print = perl::get_debug_level() > 1;
#endif
      if (!at_end()) {
         first_step(); operator++();
      }
   }

   reference operator* () const { return hom_cur; }
   pointer operator-> () const { return &hom_cur; }

   iterator& operator++ ()
   {
      dual ? ++d_cur : --d_cur;
      if (!at_end()) {
         hom_cur=hom_next; step();
      }
      return *this;
   }
   const iterator operator++ (int) { iterator copy=*this;  operator++(); return copy; }

   bool operator== (const iterator& it) const { return d_cur==it.d_cur; }
   bool operator!= (const iterator& it) const { return !operator==(it); }

   bool at_end() const
   {
      return dual ? d_cur>d_end : d_cur<d_end;
   }

   int dim() const { return d_cur-dual; }
   const companion_type& cycle_coeffs() const { return Cycles; }

protected:
   const BaseComplex *complex;
   int d_cur, d_end;
   HomologyGroup<E> hom_cur, hom_next;
   int rank_cur=0, rank_next=0;
   Bitset elim_rows, elim_cols;
   typename MatrixType::persistent_type delta;

   static const int R_inv_prev=0, L=1, LxR_prev=2, R_inv=3, companion_set=4;
   //array of SparseMatrices to hold the companion matrices
   typedef typename MatrixType::persistent_type matrix_array[companion_set]; //matrix_array is an array of 4 matrices
   struct nothing_array : nothing {
      const nothing& operator[] (int) const { return *this; }
      nothing* operator+ (int) { return this; }
   };
   typedef typename std::conditional<with_cycles, matrix_array, nothing_array>::type companion_array;
   companion_array companions;
   //SparseMatrix supposed to hold the generators of the homology group
   companion_type Cycles;

   typedef typename std::conditional<with_cycles, Smith_normal_form_logger<E>, nothing_logger>::type snf_logger_type;
   typedef typename std::conditional<with_cycles, elimination_logger<E>, nothing_logger>::type elim_logger_type;

   void first_step();
   void step(bool first=false);

   void init_companion(const nothing*, int) {}
   void init_companion(GenericMatrix<MatrixType,E>* M, int n) { *M=unit_matrix<E>(n); }
   void calculate_cycles(const nothing&) {}
   void calculate_cycles(GenericMatrix<MatrixType,E>&);
   void prepare_LxR_prev(const nothing*) {}
   void prepare_LxR_prev(GenericMatrix<MatrixType,E>*);

#if POLYMAKE_DEBUG
   SparseMatrix<Rational> r_delta, r_delta_next;
   bool debug_print;

   void debug1(int d, const GenericMatrix<MatrixType,E>& _delta, const SparseMatrix<Rational>& _r_delta, const GenericMatrix<MatrixType,E>* _companions) const
   {
      const SparseMatrix<Rational> r_L(_companions[L]), r_R_inv(_companions[R_inv]);
      cout << "elim[" << d << "]:\n" << std::setw(3) << _delta;
      if (r_L * _r_delta * inv(r_R_inv) != _delta) cout << "WRONG!\n";
      cout << "L:\n" << std::setw(3) << _companions[L];
      if (! abs_equal(det(r_L), 1)) cout << "NOT UNIMODULAR!\n";
      cout << "R_inv:\n" << std::setw(3) << _companions[R_inv];
      if (! abs_equal(det(r_R_inv), 1)) cout << "NOT UNIMODULAR!\n";
      cout << endl;
   }
   void debug2(const GenericMatrix<MatrixType,E>*) const
   {
      cout << "cancel cols[" << d_cur << "]: " << elim_rows << endl;
      const SparseMatrix<Rational> r_L(companions[L]), r_R_inv(companions[R_inv]);
      if (r_L * r_delta * inv(r_R_inv) != delta) cout << "WRONG!\n";
      cout << "R_inv:\n" << std::setw(3) << companions[R_inv];
      if (! abs_equal(det(r_R_inv), 1)) cout << "NOT UNIMODULAR!\n";
   }
   void debug3(const GenericMatrix<MatrixType,E>*) const
   {
      const SparseMatrix<Rational> r_L(companions[L]), r_R_inv(companions[R_inv]);
      if (r_L * r_delta * inv(r_R_inv) != delta) cout << "WRONG!\n";
      cout << "L:\n" << std::setw(3) << companions[L];
      if (! abs_equal(det(r_L), 1)) cout << "NOT UNIMODULAR!\n";
      cout << "R_inv:\n" << std::setw(3) << companions[R_inv];
      if (! abs_equal(det(r_R_inv), 1)) cout << "NOT UNIMODULAR!\n";
      cout << "LxR_prev:\n" << std::setw(3) << companions[LxR_prev];
   }
   void debug1(int d, const GenericMatrix<MatrixType,E>& _delta, const SparseMatrix<Rational>& _r_delta, const nothing*) const
   {
      cout << "elim[" << d << "]:\n" << std::setw(3) << _delta << endl;
   }
   void debug2(const nothing*) const
   {
      cout << "cancel cols[" << d_cur << "]: " << elim_rows << endl;
   }
   void debug3(const nothing*) const {}
#endif
};
} }
namespace pm {

template <typename E, typename MatrixType, typename BaseComplex, bool with_cycles, bool dual>
struct check_iterator_feature<polymake::topaz::Complex_iterator<E, MatrixType, BaseComplex, with_cycles, dual>, end_sensitive> : std::true_type {};

}
namespace polymake { namespace topaz {

template <typename E, typename MatrixType, typename BaseComplex, bool with_cycles, bool dual>
void Complex_iterator<E,MatrixType,BaseComplex,with_cycles,dual>::first_step()
{ 
   //get bd matrices for first step
      delta=dual ? T(complex->template boundary_matrix<E>(d_cur)) : complex->template boundary_matrix<E>(d_cur);
#if POLYMAKE_DEBUG
   if (debug_print) {
      cout << (dual ? "dual delta[" : "delta[") << d_cur << "]:\n" << std::setw(3) << delta << endl;
      r_delta=SparseMatrix<Rational>(delta);
   }
#endif
   //initialize with unit matrices
   init_companion(companions+L, delta.rows());
   init_companion(companions+R_inv, delta.cols());

   //elim_ones optimization only works for matrices with entries from {0,+1,-1}
   if(pm::is_derived_from_instance_of<BaseComplex,SimplicialComplex_as_FaceMap>::value)
      rank_cur=eliminate_ones(delta, elim_rows, elim_cols, elim_logger_type(companions+L, companions+R_inv));

   companions[LxR_prev]=companions[L];
#if POLYMAKE_DEBUG
   if (debug_print) debug1(d_cur, delta, r_delta, companions+0); 
#endif
   step(true);
}

//each step calculates (co-)homology (and cycles) for the current dimension
template <typename E, typename MatrixType, typename BaseComplex, bool with_cycles, bool dual>
void Complex_iterator<E,MatrixType,BaseComplex,with_cycles,dual>::step(bool first)
{
   companion_array companions_next;
   typename MatrixType::persistent_type delta_next;

   companion_type *cLxR_prev=0, *cR_inv=0;
   int rank_next=0;
   if (d_cur!=d_end) {
      if (dual) {
         delta_next=T(complex->template boundary_matrix<E>(d_cur+1));
      } else {
         delta_next=complex->template boundary_matrix<E>(d_cur-1);
      }
#if POLYMAKE_DEBUG
      if (debug_print) {
         cout << (dual ? "dual delta[" : "delta[") << (dual ? d_cur+1 : d_cur-1) << "]:\n" << std::setw(3) << delta_next << endl;
         r_delta_next=SparseMatrix<Rational>(delta_next);
         cout << "cancel rows[" << (dual ? d_cur+1 : d_cur-1) << "]: " << elim_cols << endl;
      }
#endif
      delta_next.minor(elim_cols,All).clear();
      //initialize with unit matrices
      init_companion(companions_next+LxR_prev, delta_next.rows());
      init_companion(companions_next+R_inv, delta_next.cols());

//elim_ones optimization only works for matrices with entries from {0,+1,-1}
      if(pm::is_derived_from_instance_of<BaseComplex,SimplicialComplex_as_FaceMap>::value)
         rank_next=eliminate_ones(delta_next, elim_rows, elim_cols, elim_logger_type(companions+R_inv, companions_next+R_inv));

      //represent next bd matrix wrt. to previously computed basis for Z by left-multiplying with inverse transformation
      companions_next[L]=companions[R_inv];
#if POLYMAKE_DEBUG
      if (debug_print) debug1((dual ? d_cur+1 : d_cur-1), delta_next, r_delta_next, companions_next+0);
#endif

      delta.minor(All,elim_rows).clear();
#if POLYMAKE_DEBUG
      if (debug_print) debug2(companions+0); 
#endif
      cLxR_prev=companions_next+LxR_prev;
      cR_inv=companions+R_inv;
   }
   rank_cur += smith_normal_form(delta, hom_next.torsion,
                                 snf_logger_type(companions+L, companions+LxR_prev, cLxR_prev, cR_inv), std::false_type());

#if POLYMAKE_DEBUG
   if (debug_print) {
      cout << "snf[" << d_cur << "]:\n" << std::setw(3) << delta;
      if (cR_inv) debug3(companions+0);
   }
#endif
   hom_next.betti_number=-rank_cur;
   if (!first) {
      //for cols of delta that are empty, delete corresponding cols in cLxR_prev
      prepare_LxR_prev(cLxR_prev);
      hom_cur.betti_number+=delta.rows()-rank_cur;
      calculate_cycles(Cycles);
      pm::compress_torsion(hom_cur.torsion);
   }
   delta=delta_next;
   rank_cur=rank_next;
#if POLYMAKE_DEBUG
   if (debug_print) r_delta=r_delta_next;
#endif
   //save companions for next step (discard previous ones)
   companions[R_inv_prev]=companions[R_inv];
   //comp_next[L]=R_inv of this step.
   companions[L]=companions_next[L];
   companions[LxR_prev]=companions_next[LxR_prev];
   companions[R_inv]=companions_next[R_inv];
}

template <typename E, typename MatrixType, typename BaseComplex, bool with_cycles, bool dual> inline
void Complex_iterator<E,MatrixType,BaseComplex,with_cycles,dual>::prepare_LxR_prev(GenericMatrix<MatrixType,E> *pLxR_prev)
{
   if (pLxR_prev)
      for (typename Entire< Cols<typename MatrixType::persistent_type> >::iterator c=entire(cols(delta)); !c.at_end(); ++c)
         if (!c->empty())
            pLxR_prev->col(c.index()).clear();
}

template <typename E, typename MatrixType, typename BaseComplex, bool with_cycles, bool dual>
void Complex_iterator<E,MatrixType,BaseComplex,with_cycles,dual>::calculate_cycles(GenericMatrix<MatrixType,E>& C)
{
   //number of generators = betti + number of torsion elements. torsion list is non-compressed, thus t->second is not multiplicity but the row of the torsion coefficient
   Cycles.resize(hom_cur.betti_number + hom_cur.torsion.size(), delta.rows());
   typename Entire< Rows<typename MatrixType::persistent_type> >::iterator r=entire(rows(Cycles));
   //torsional part
   for (typename HomologyGroup<E>::torsion_list::iterator t=hom_cur.torsion.begin(), t_end=hom_cur.torsion.end();
        t != t_end; ++t, ++r)
      *r = companions[R_inv_prev].row(t->second);

   //free part
   typename Rows<typename MatrixType::persistent_type>::iterator r_d=rows(delta).begin();
   while (!r.at_end()) {
      while (! r_d->empty()) ++r_d;//skip non-null rows

      if (! companions[LxR_prev].row(r_d.index()).empty()) {//for Z-basis elements that are also in the B-basis, the product of the companions has a zero row. the rows of R_prev are a basis for the previous chain group, and L describes the row operations to get a basis for B, zeroing out all other basis elements.

         *r = companions[L].row(r_d.index());
         ++r;
      }
      ++r_d;
   }
}

//this class manages efficient homology computation of arbitrary chain complexes (e.g. simplicial complexes).
template <typename R, typename MatrixType, typename BaseComplex>
class HomologyComplex {
protected:
   const BaseComplex& complex;
   int dim_high, dim_low;
public:
   explicit HomologyComplex(const BaseComplex& complex_arg,
                         int dim_high_arg=-1, int dim_low_arg=0)
      : complex(complex_arg), dim_high(dim_high_arg), dim_low(dim_low_arg)
   {
      int d=dim();
      if (dim_high<0) dim_high+=d+1;
      if (dim_low<0) dim_low+=d+1;
      if (dim_high<dim_low || dim_high>d || dim_low<0)
         throw std::runtime_error("HomologyComplex - dimensions out of range");
   }

   int dim() const { return complex.dim(); }
   int size() const { return dim_high-dim_low+1; }
   const BaseComplex& get_complex() const { return complex; }

   typedef HomologyGroup<R> homology_type;
   typedef CycleGroup<R> cycle_type;

   template <bool with_cycles, bool dual> class as_container;

   friend const as_container<false,false>& homologies(const HomologyComplex& cc)
   {
      return reinterpret_cast<const as_container<false,false>&>(cc);
   }

   friend const as_container<true,false>& homologies_and_cycles(const HomologyComplex& cc)
   {
      return reinterpret_cast<const as_container<true,false>&>(cc);
   }

   friend const as_container<false,true>& cohomologies(const HomologyComplex& cc)
   {
      return reinterpret_cast<const as_container<false,true>&>(cc);
   }

   friend const as_container<true,true>& cohomologies_and_cocycles(const HomologyComplex& cc)
   {
      return reinterpret_cast<const as_container<true,true>&>(cc);
   }
};

template <typename R, typename MatrixType, typename BaseComplex>
template <bool with_cycles, bool dual>
class HomologyComplex<R,MatrixType,BaseComplex>::as_container : public HomologyComplex<R,MatrixType,BaseComplex> {
protected:
   as_container();
   ~as_container();
public:
   typedef Complex_iterator<R, MatrixType, BaseComplex, with_cycles, dual> iterator;
   typedef iterator const_iterator;
   typedef typename iterator::value_type value_type;
   typedef typename iterator::reference reference;
   typedef reference const_reference;

   iterator begin() const
   {
      return iterator(complex,dim_high,dim_low);
   }
   iterator end() const
   {
      return iterator(complex,dim_low-1,dim_low);
   }
};

template <typename R, typename MatrixType, typename BaseComplex> inline
HomologyComplex<R, MatrixType, BaseComplex>
make_homology_complex(const BaseComplex& complex, int dim_high=-1, int dim_low=1)
{
   return HomologyComplex<R, MatrixType, BaseComplex>(complex,dim_high,dim_low);
}

} }

#endif // POLYMAKE_TOPAZ_HOMOLOGY_COMPLEX_H

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
