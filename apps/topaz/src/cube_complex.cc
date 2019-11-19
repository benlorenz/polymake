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
#include "polymake/Array.h"
#include "polymake/Matrix.h"
#include "polymake/Vector.h"
#include "polymake/Set.h"
#include "polymake/MultiDimCounter.h"
#include "polymake/list"
#include <algorithm>

namespace polymake { namespace topaz {

namespace {
  
// prodces a triangulated cube
std::list< Set<int> > triang_cube(const int lower_corner, const Array<int>& x_diff)
{
  std::list< Set<int> > cube;
  const int dim = x_diff.size();
  
  Array<int> path(dim);
  for (int i=0; i<dim; ++i)
    path[i] = i;
  
  // iterate over all monotonous paths from vertex 0 to vertex 2^dim - 1
  while (true) {
    Set<int> simplex;
    simplex += lower_corner;
    
    // follow path
    int vertex = 0;
    for (int i=0; i<dim; ++i) {
      vertex |= 1<<path[i];
      
      // compute coordinates in the cube complex
      int corner = lower_corner;
      for (int j=0; j<dim; ++j)
        if ((vertex&(1<<j)) != 0)
          corner += x_diff[dim-j-1];
      simplex += corner;
    }
    
    // add simplex
    cube.push_back(simplex);
    
    // generate next path
    if (!std::next_permutation(path.begin(), path.end()))  break;
  }
  
  return cube;
}

}

perl::Object cube_complex(Array<int> x_param)
{
  // adjust x_param
  for (int i=0; i<x_param.size(); ++i)
    ++x_param[i];

  const int dim = x_param.size();
  int n = x_param[dim-1];
  Array<int> x_diff(dim);
  x_diff[dim-1] = 1;
  
  for (int i=dim-2; i>=0; --i) {
    x_diff[i] = x_diff[i+1] * x_param[i+1];
    n *= x_param[i];
  }
  
  perl::Object p("GeometricSimplicialComplex<Rational>");
  std::ostringstream description;
  for (int i=0; i<dim-1; ++i)
    description << x_param[i]-1 << "x";
  description << x_param[dim-1]-1 << " Pile of " << dim << "-dimensional triangulated cubes." << endl;
  p.set_description() << description.str();
  
  Matrix<int> Coordinates(n,dim);
  std::list< Set<int> > Pile;
  int corner=0;
  for (MultiDimCounter<false> counter(x_param); !counter.at_end(); ++corner, ++counter) {     

    // compute coordinates
    copy_range(entire(*counter), Coordinates[corner].begin());

    // compute cube
    bool cube_corner = true;
    for (int i=0; i<dim; ++i)
      if ((*counter)[i] == x_param[i]-1) {
        cube_corner = false;
        break;
      }
    
    if (cube_corner) {
      std::list< Set<int> > cube = triang_cube(corner, x_diff);
      Pile.splice(Pile.end(), cube);
    }
  }
  
  p.take("FACETS") << Pile;
  p.take("DIM") << dim;
  p.take("COORDINATES") << Coordinates;
  p.take("MANIFOLD") << true;
  p.take("ORIENTED_PSEUDO_MANIFOLD") << true;
  p.take("BALL") << true;
  return p;
}

UserFunction4perl("# @category Producing from scratch\n"
                  "# Produces a triangulated pile of hypercubes, arranged in a d-dimensional array."
                  "# Each cube is split into d! tetrahedra, and the tetrahedra are all grouped around"
                  "# one of the diagonal axes of the cube.\n"
                  "# @param Array<Int> x specifies the shape of the pile:"
                  "# d=x.size is the dimension of the cubes to be stacked, and the stack will be"
                  "# x_1 by x_2 by ... by x_d cubes."
                  "# @return GeometricSimplicialComplex<Rational>"
                  "# @example Arrange four triangulated 3-cubes to form a big 2 by 2 cube:"
                  "# > $cc = cube_complex([2,2,2]);"
                  "# > print $cc->description;"
                  "# | 2x2x2 Pile of 3-dimensional triangulated cubes.",
                  &cube_complex, "cube_complex");

} }

// Local Variables:
// mode:C++
// c-basic-offset:3
// indent-tabs-mode:nil
// End:
