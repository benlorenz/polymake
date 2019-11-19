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

#ifndef POLYMAKE_GRAPH_LATTICE_H
#define POLYMAKE_GRAPH_LATTICE_H

#include "polymake/client.h"
#include "polymake/Graph.h"
#include "polymake/Set.h"
#include "polymake/Array.h"
#include "polymake/vector"
#include "polymake/graph/Decoration.h"
#include <algorithm>

namespace polymake { namespace graph {

using lattice::InverseRankMap;

/*
 * A Lattice is a decorated lattice of subsets of a finite set E ={0,..,n-1}, which is realized
 * as a directed graph. Here i -> j means i is covered by j.
 * It is templated by two paramters:
 * 1) Decoration: This is arbitrary data attached to each node. It is assumed that every node has two
 *    decorations: Its face, which is the corresponding subset of E, and its rank, which is a strictly
 *    monotone (with respect to the graph) integer function
 * 2) If it is known that all nodes of the same rank always form a sequence and that nodes of subsequent ranks
 *    also appear in subsequent lists, the second template
 *    parameter can be set to lattice::Sequential.
 *    In this case, the inverse rank map is serialized in a more efficient manner.
 */
template <typename Decoration, typename SeqType = lattice::Nonsequential>
class Lattice {

protected:
  Graph<Directed> G;
  NodeMap<Directed, Decoration> D;
  InverseRankMap<SeqType> rank_map;

  int top_node_index;
  int bottom_node_index;

public:

  using nodes_of_rank_ref_type = typename SeqType::nodes_of_rank_ref_type;
  using nodes_of_rank_type = typename SeqType::nodes_of_rank_type;

  Lattice() : D(G) {}

  Lattice(const Lattice<Decoration, SeqType>& l)
    : G(l.graph())
    , D(G, entire(l.D))
    , rank_map(l.rank_map)
    , top_node_index(l.top_node())
    , bottom_node_index(l.bottom_node()) {}

  // Copies all but the top node
  friend Lattice<Decoration, SeqType> copy_all_but_top_node(const Lattice<Decoration, SeqType>& me)
  {
    Lattice<Decoration, SeqType> l(me);
    if (l.nodes() > 1)
      l.top_node_index = *(l.in_adjacent_nodes(l.top_node_index).begin());
    l.G.delete_node(me.top_node_index);
    l.G.squeeze();
    l.rank_map.delete_node_and_squeeze(me.top_node_index, me.rank());
    return l;
  }

  Graph<Directed>& graph() { return G; }
  const Graph<Directed>& graph() const { return G; }
  const NodeMap<Directed,Decoration>& decoration() const { return D; }
  const InverseRankMap<SeqType>& inverse_rank_map() const { return rank_map; }
  const Decoration& decoration(int n) const { return D[n]; }
  const Set<int>& face(int n) const { return D[n].face;}

  int top_node() const { return top_node_index;}
  int bottom_node() const { return bottom_node_index;}

  Array<Set<int>> dual_faces() const
  {
    Array<Set<int>> df(nodes());
    int i = 0;
    int top_rank = rank();
    int bottom_rank = lowest_rank();
    for (auto f = entire(nodes_of_rank(top_rank-1)); !f.at_end(); ++f, ++i)
      df[*f] = scalar2set(i);
    for (int d = top_rank -2; d >= bottom_rank; --d)
      for (auto f = entire(nodes_of_rank(d)); !f.at_end(); ++f)
        for (auto nb = entire(out_adjacent_nodes(*f)); !nb.at_end(); ++nb)
          df[*f] += df[*nb];
    return df;
  }

  // Applies a single permutation to all faces.
  template <typename Permutation>
  void permute_faces(const Permutation &perm)
  {
    for (auto& dec : D) {
      dec.face = permuted(dec.face, perm);
    }
  }

  // Applies a node permutation. It is assumed that the permutation only
  // moves around nodes within a rank level, not between two distinct rank levels.
  template <typename Permutation>
  void permute_nodes_in_levels(const Permutation& node_perm)
  {
    G.permute_nodes(node_perm);
  }

  int nodes() const { return G.nodes();}
  int edges() const { return G.edges();}

  friend const Nodes<Graph<Directed>>& nodes(const Lattice<Decoration, SeqType>& me) { return pm::nodes(me.G); }
  friend const Edges<Graph<Directed>>& edges(const Lattice<Decoration, SeqType>& me) { return pm::edges(me.G); }
  friend const AdjacencyMatrix<Graph<Directed>>& adjacency_matrix(const Lattice<Decoration, SeqType>& me) { return pm::adjacency_matrix(me.G); }

  bool node_exists(int n) const { return G.node_exists(n); }
  bool edge_exists(int n1, int n2) const { return G.edge_exists(n1,n2); }

  decltype(auto) out_edges(int n) const { return G.out_edges(n); }
  decltype(auto) in_edges(int n) const { return G.in_edges(n); }

  decltype(auto) out_adjacent_nodes(int n) const { return G.out_adjacent_nodes(n); }
  decltype(auto) in_adjacent_nodes(int n) const { return G.in_adjacent_nodes(n); }

  int out_degree(int n) const { return G.out_degree(n); }
  int in_degree(int n) const { return G.in_degree(n); }
  int degree(int n) const { return G.degree(n); }

  int rank(int n) const
  {
    return D[n].rank;
  }

  int rank() const
  {
    return rank(top_node());
  }

  int lowest_rank() const
  {
    return rank(bottom_node());
  }

  decltype(auto) nodes_of_rank(int d) const
  {
    return rank_map.nodes_of_rank(d);
  }

  decltype(auto) nodes_of_rank_range(int d1, int d2) const
  {
    return rank_map.nodes_of_rank_range(d1,d2);
  }

  // Building methods

  void set_decoration(int n, const Decoration& data)
  {
    D[n] = data;
    rank_map.set_rank(n, data.rank);
  }

  int add_node(const Decoration& data)
  {
    int n = G.nodes();
    G.resize(n+1);
    set_decoration(n,data);
    if (n == 0) {
      bottom_node_index = 0; top_node_index = 0;
    }
    return n;
  }

  template <typename Iterator>
  int add_nodes(int n, Iterator data_list)
  {
    int n_old = G.nodes();
    G.resize(n_old + n);
    for (auto nd = entire(sequence(n_old, n)); !nd.at_end(); ++nd, ++data_list) {
      set_decoration(*nd, *data_list);
    }
    if (n_old == 0) {
      bottom_node_index = 0; top_node_index = 0;
    }
    return n_old;
  }

  void add_edge(int n_from, int n_to)
  {
    G.edge(n_from, n_to);
    if (n_from == top_node_index) top_node_index = n_to;
    if (n_to == bottom_node_index) bottom_node_index = n_from;
  }

  // TODO: introduce operator ... && moving all members
  explicit operator perl::Object () const
  {
    perl::Object result("Lattice", mlist<Decoration, SeqType>());
    result.take("ADJACENCY") << graph();
    result.take("DECORATION") << decoration();
    result.take("INVERSE_RANK_MAP") << rank_map;
    result.take("TOP_NODE") << top_node();
    result.take("BOTTOM_NODE") << bottom_node();
    return result;
  }

  explicit Lattice(const perl::Object& obj)
    : D(G)
  {
    *this = obj;
  }

  Lattice& operator= (const perl::Object& obj)
  {
    // TODO: include is_trusted flag in Object?
    // if (obj.get_flags() * pm::perl::ValueFlags::not_trusted && !obj.isa("Lattice"))
    //   throw std::runtime_error("wrong object type for Lattice");
    obj.give("ADJACENCY") >> G;
    obj.give("DECORATION") >> D;
    obj.give("INVERSE_RANK_MAP") >> rank_map;
    obj.give("TOP_NODE") >> top_node_index;
    obj.give("BOTTOM_NODE") >> bottom_node_index;
    return *this;
  }
};

} }

namespace pm { namespace perl {

template <typename Decoration, typename SeqType>
struct represents_big_Object<polymake::graph::Lattice<Decoration, SeqType>> : std::true_type {};

} }

#endif

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
