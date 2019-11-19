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

#include "polymake/client.h"
#include "polymake/Graph.h"
#include "polymake/Set.h"
#include "polymake/IncidenceMatrix.h"
#include "polymake/Matrix.h"
#include "polymake/linalg.h"
#include "polymake/Rational.h"

namespace polymake { namespace polytope {

template <typename IMatrix>
Graph<> points_graph_from_incidence(const Matrix<Rational>& points,  const GenericIncidenceMatrix<IMatrix>& IM, const Matrix<Rational>& facets, const int& dim)
{

  const int n_vertices=IM.cols();
  Graph<> G(n_vertices);
  if (n_vertices<3) {
    if (n_vertices==2) G.edge(0,1);
    return G;
  }

  EdgeMap<Undirected, Set<int> > intersects(G);

  for (auto n1=entire(nodes(G)); !n1.at_end(); ++n1) {
    const Matrix<Rational> min=facets.minor(IM.col(*n1),All);
    if (rank(min)>=dim-1) { //exclude vertices in the interior of some non-edge face
      auto n2=n1;
      while (!(++n2).at_end()) {
        Set<int> common = IM.col(*n1) * IM.col(*n2);
        if (common.empty()) continue;
        
        Graph<>::out_edge_list::iterator edge=n1.out_edges().begin();
        bool add=true;
        while (!edge.at_end()) {
          const int inc=incl(intersects[*edge],common);
          if (inc==2) {
            ++edge;
          } else {
            if (inc<0) n1.out_edges().erase(edge++);
            else {
              if (inc>0) { add=false; break; }
              int i=1;
              //compute lambda with lambda*points[*n1]+(1-lambda)*points[edge.to_node()]=points[*n2]
              while (points(*n1,i)==points(*n2,i)) ++i;
              if( points(edge.to_node(),i)==points(*n1,i) ){add=false; break;}
              const Rational lambda=(points(edge.to_node(),i)-points(*n2,i))/(points(edge.to_node(),i)-points(*n1,i));
              // cout<<(*n1)<<" "<<(*n2)<<" "<<edge.to_node()<<" "<<lambda<<endl;
              if (lambda>1);
              else if (lambda>0) n1.out_edges().erase(edge++);
              else add=false;
              break;
            }
          }
        }
        if (add) intersects[n1.edge(*n2)]=common;
      }
    }
    //else cout<<(*n1)<<": "<<facets.minor(IM.col(*n1),All)<<endl<<rank(facets.minor(IM.col(*n1),All))<<endl;
  }
  return G;
}

FunctionTemplate4perl("points_graph_from_incidence(Matrix<Rational> IncidenceMatrix Matrix<Rational> Int)");

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
