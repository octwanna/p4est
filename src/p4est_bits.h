/*
  This file is part of p4est.
  p4est is a C library to manage a collection (a forest) of multiple
  connected adaptive quadtrees or octrees in parallel.

  Copyright (C) 2010 The University of Texas System
  Written by Carsten Burstedde, Lucas C. Wilcox, and Tobin Isaac

  p4est is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  p4est is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with p4est; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*/

/** \file p4est_bits.h
 *
 * Routines for manipulating quadrants (neighbors, parents, children, etc.)
 *
 * \ingroup p4est
 */

#ifndef P4EST_BITS_H
#define P4EST_BITS_H

#include <p4est.h>

SC_EXTERN_C_BEGIN;

/** Prints one line with quadrant's x, y and level.
 * \param [in] log_priority  see \ref logpriorities in sc.h for the meanings
 *                           of numerical priority values
 * \param [in] q             quadrant to print
 */
void                p4est_quadrant_print (int log_priority,
                                          const p4est_quadrant_t * q);

/** Test if two quadrants have equal Morton indices.
 * \return true if \a q1 describes the same quadrant as \a q2.
 */
int                 p4est_quadrant_is_equal (const p4est_quadrant_t * q1,
                                             const p4est_quadrant_t * q2);

/** Test if two quadrants overlap.
 * \return true if \a q1 and \a q2 are equal or one is the ancestor of the
 * other.
 */
int                 p4est_quadrant_overlaps (const p4est_quadrant_t * q1,
                                             const p4est_quadrant_t * q2);

/** Test if two quadrants have equal Morton indices and the same tree id.
 * \return          true if \a q1 describes the same quadrant as \a q2
 *                  and the p.which_tree fields are equal.
 */
int                 p4est_quadrant_is_equal_piggy (const p4est_quadrant_t *
                                                   q1,
                                                   const p4est_quadrant_t *
                                                   q2);

/** Compare two quadrants in their Morton ordering.
 * Both quadrants must be valid.
 * \return Returns < 0 if \a v1 < \a v2,
 *                   0 if \a v1 == \a v2,
 *                 > 0 if \a v1 > \a v2
 */
int                 p4est_quadrant_compare (const void *v1, const void *v2);

/** Compare two quadrants in their Morton ordering, with equivalence if the
 * two quadrants overlap.
 * \return Returns < 0 if \a v1 < \a v2 and \a v1 and \a v2 do not overlap,
 *                   0 if \a v1 and \a v2 overlap,
 *                 > 0 if \a v1 > \a v2 and \a v1 and \a v2 do not overlap.
 */
int                 p4est_quadrant_disjoint (const void *v1, const void *v2);

/** Compare two quadrants in their Morton ordering and the which_tree member.
 * Both quadrants must be extended (superset of valid, see below).
 * \return Returns < 0 if \a v1 < \a v2,
 *                   0 if \a v1 == \a v2,
 *                 > 0 if \a v1 > \a v2
 */
int                 p4est_quadrant_compare_piggy (const void *v1,
                                                  const void *v2);

/** Compare two quadrants with respect to their local_num in the piggy3 member.
 * \return Returns < 0 if \a v1 < \a v2,
 *                   0 if \a v1 == \a v2,
 *                 > 0 if \a v1 > \a v2
 */
int                 p4est_quadrant_compare_local_num (const void *v1,
                                                      const void *v2);

/** Test if two quadrants have equal Morton indices, callback version.
 * \return true if \a v1 describes the same quadrant as \a v2.
 */
int                 p4est_quadrant_equal_fn (const void *v1, const void *v2,
                                             const void *u);

/** Computes a hash value for a quadrant by the lookup3 method.
 */
unsigned            p4est_quadrant_hash_fn (const void *v, const void *u);

/** Test if two nodes are in the same tree and have equal Morton indices.
 * \param [in] v1   Pointer to a clamped or unclamped node, depending on u.
 * \param [in] v2   Pointer to a clamped or unclamped node, depending on u.
 * \param [in] u    User data, points to an int holding the clamped-flag.
 */
int                 p4est_node_equal_piggy_fn (const void *v1,
                                               const void *v2, const void *u);

/** Compute hash value of a node based on its tree and Morton index.
 * \param [in] v    Pointer to a clamped or unclamped node, depending on u.
 * \param [in] u    User data, points to an int holding the clamped-flag.
 */
unsigned            p4est_node_hash_piggy_fn (const void *v, const void *u);

/** Clamp a node inside the unit tree if it sits on a high border.
 * \param [in] n    Node to be clamped. Must not yet be clamped.
 * \param [out] r   Existing node overwritten by the clamped result.
 */
void                p4est_node_clamp_inside (const p4est_quadrant_t * n,
                                             p4est_quadrant_t * r);

/** Move a clamped node out on the border.
 * \param [in] n    Node to be unclamped in-place.
 */
void                p4est_node_unclamp (p4est_quadrant_t * n);

/** Find the enclosing quadrant of a given node at a given level.
 * \param [in] n        Clamped node.
 * \param [in] level    Level of the quadrant to be created.
 * \param [out] q       Output quadrant, n == q is permitted.
 */
void                p4est_node_to_quadrant (const p4est_quadrant_t * n,
                                            int level, p4est_quadrant_t * q);

/** Decide if a node is completely contained within a quadrant.
 * \param [in] q        Valid quadrant.
 * \param [in] n        Clamped node.
 */
int                 p4est_quadrant_contains_node (const p4est_quadrant_t * q,
                                                  const p4est_quadrant_t * n);

/** Compute the position of the ancestor of this child at level \a level within
 * its siblings.
 * \return Returns its child id in 0..3
 */
int                 p4est_quadrant_ancestor_id (const p4est_quadrant_t * q,
                                                int level);

/** Compute the position of this child within its siblings.
 * \return Returns its child id in 0..3
 */
int                 p4est_quadrant_child_id (const p4est_quadrant_t * q);

/** Test if a quadrant is inside the unit tree.
 * \param [in] q Quadrant to be tested.
 * \return Returns true if \a q is inside the unit tree.
 */
int                 p4est_quadrant_is_inside_root (const p4est_quadrant_t *
                                                   q);

/** Test if a quadrant is inside the 3x3 box around the root tree.
 * \param [in] q Quadrant to be tested.
 * \return Returns true if \a q is inside the unit tree.
 */
int                 p4est_quadrant_is_inside_3x3 (const p4est_quadrant_t * q);

/** Test if a quadrant is outside a tree face boundary (no corner).
 * \param [in] q Quadrant to be tested.
 * \return Returns true if \a q is outside across a unit tree face.
 */
int                 p4est_quadrant_is_outside_face (const p4est_quadrant_t *
                                                    q);

/** Test if a quadrant is outside a tree corner boundary.
 * \param [in] q Quadrant to be tested.
 * \return Returns true if \a q is outside across a unit tree corner.
 */
int                 p4est_quadrant_is_outside_corner (const p4est_quadrant_t *
                                                      q);

/** Test if a quadrant is used to represent a mesh node.
 * \param [in] q        Quadrant to be tested.
 * \param [in] inside   If true, boundary nodes must be clamped inside.
 *                      If false, nodes must align with the quadrant grid.
 * \return Returns true if \a q is a node.
 */
int                 p4est_quadrant_is_node (const p4est_quadrant_t * q,
                                            int inside);

/** Test if a quadrant has valid Morton indices and is inside the unit tree.
 * \param [in] q Quadrant to be tested.
 * \return Returns true if \a q is valid.
 */
int                 p4est_quadrant_is_valid (const p4est_quadrant_t * q);

/** Test if a quadrant has valid Morton indices in the 3x3 box around root.
 * \param [in] q Quadrant to be tested.
 * \return Returns true if \a q is extended.
 */
int                 p4est_quadrant_is_extended (const p4est_quadrant_t * q);

/** Test if two quadrants are siblings.
 * \param [in] q1 First quadrant to be tested.
 * \param [in] q2 Second quadrant to be tested.
 * \return true if \a q1 is unequal to and a sibling of \a q2.
 */
int                 p4est_quadrant_is_sibling (const p4est_quadrant_t * q1,
                                               const p4est_quadrant_t * q2);

/** Test if two quadrants are siblings.
 * Descriptive, slower version of \a p4est_quadrant_is_sibling.
 * For debugging and educational purposes only.
 */
int                 p4est_quadrant_is_sibling_D (const p4est_quadrant_t * q1,
                                                 const p4est_quadrant_t * q2);

/** Test if 4 quadrants are siblings in Morton ordering.
 */
int                 p4est_quadrant_is_family (const p4est_quadrant_t * q0,
                                              const p4est_quadrant_t * q1,
                                              const p4est_quadrant_t * q2,
                                              const p4est_quadrant_t * q3);

/** Test if 4 quadrants are siblings in Morton ordering, array version.
 * \param [in] q   Array of 4 quadrants.
 */
int                 p4est_quadrant_is_familyv (const p4est_quadrant_t q[]);

/** Test if 4 quadrants are siblings in Morton ordering, array version.
 * \param [in] q   Array of 4 pointers to quadrants.
 */
int                 p4est_quadrant_is_familypv (p4est_quadrant_t * q[]);

/** Test if a quadrant is the parent of another quadrant.
 * \param [in] q Quadrant to be tested.
 * \param [in] r Possible child quadrant.
 * \return true if \a q is the parent of \a r.
 */
int                 p4est_quadrant_is_parent (const p4est_quadrant_t * q,
                                              const p4est_quadrant_t * r);

/** Test if a quadrant is the parent of another quadrant.
 * Descriptive, slower version of \a p4est_quadrant_is_parent.
 * For debugging and educational purposes only.
 */
int                 p4est_quadrant_is_parent_D (const p4est_quadrant_t * q,
                                                const p4est_quadrant_t * r);

/** Test if a quadrant is an ancestor of another quadrant.
 * \param [in] q Quadrant to be tested.
 * \param [in] r Descendent quadrant.
 * \return true if \a q is unequal to and an ancestor of \a r.
 */
int                 p4est_quadrant_is_ancestor (const p4est_quadrant_t * q,
                                                const p4est_quadrant_t * r);

/** Test if a quadrant is an ancestor of another quadrant.
 * Descriptive, slower version of \a p4est_quadrant_is_ancestor.
 * Contrary to \a p4est_quadrant_is_ancestor, it aborts for inter-tree q, r.
 * For debugging and educational purposes only.
 */
int                 p4est_quadrant_is_ancestor_D (const p4est_quadrant_t * q,
                                                  const p4est_quadrant_t * r);

/** Test if two quadrants follow each other in the tree with no holes.
 * \param [in] q A quadrant
 * \param [in] r Another quadrant
 * \return true if \a q is immediately before \a r in the tree.
 * \note for every \a q there are between 0 and P4EST_MAXLEVEL+1 possible nexts.
 */
int                 p4est_quadrant_is_next (const p4est_quadrant_t * q,
                                            const p4est_quadrant_t * r);

/** Test if two quadrants follow each other in the tree with no holes.
 * Descriptive, slower version of \a p4est_quadrant_is_next.
 * For debugging and educational purposes only.
 */
int                 p4est_quadrant_is_next_D (const p4est_quadrant_t * q,
                                              const p4est_quadrant_t * r);

/** Test if a quadrant has at least partial overlap with a tree.
 */
int                 p4est_quadrant_overlaps_tree (p4est_tree_t * tree,
                                                  const p4est_quadrant_t * q);

/** Test if a quadrant is completely contained within a tree.
 */
int                 p4est_quadrant_is_inside_tree (p4est_tree_t * tree,
                                                   const p4est_quadrant_t *
                                                   q);

/** Compute the ancestor of a quadrant at a given level.
 * \param [in]  q       Input quadrant.
 * \param [in]  level   A smaller level than q.
 * \param [in,out]  r   Existing quadrent whose Morton index will be filled
 *                      with the ancestor of q at the given level.
 * \note The quadrant q may point to the same quadrant as r.
 *       The user_data of r are never modified.
 */
void                p4est_quadrant_ancestor (const p4est_quadrant_t * q,
                                             int level, p4est_quadrant_t * r);

/** Compute the parent of a quadrant.
 * \param [in]  q Input quadrant.
 * \param [in,out] r Existing quadrant whose Morton index will be filled
 *                   with the Morton index of the parent of \a q.
 *                   Its user_data will be untouched.
 * \note \a q may point to the same quadrant as \a r.
         The user_data of \a r is never modified.
 */
void                p4est_quadrant_parent (const p4est_quadrant_t * q,
                                           p4est_quadrant_t * r);

/** Compute a specific sibling of a quadrant.
 * \param [in]     q  Input quadrant.
 * \param [in,out] r  Existing quadrant whose Morton index will be filled
 *                    with the coordinates of sibling no. sibling_id of q.
 * \param [in]     sibling_id The id of the sibling computed, 0..3.
 */
void                p4est_quadrant_sibling (const p4est_quadrant_t * q,
                                            p4est_quadrant_t * r,
                                            int sibling_id);

/** Compute a specific child of a quadrant.
 * \param [in]     q    Input quadrant.
 * \param [in,out] r    Existing quadrant whose Morton index will be filled
 *                      with the coordinates of its child no. \b child_id.
 * \param [in] child_id The id of the child computed, 0..3.
 */
void                p4est_quadrant_child (const p4est_quadrant_t * q,
                                          p4est_quadrant_t * r, int child_id);

/** Compute the face neighbor of a quadrant.
 * \param [in]     q      Input quadrant, must be valid.
 * \param [in]     face   The face across which to generate the neighbor.
 * \param [in,out] r      Existing quadrant whose Morton index will be filled.
 * \note \a q may point to the same quadrant as \a r.
 */
void                p4est_quadrant_face_neighbor (const p4est_quadrant_t * q,
                                                  int face,
                                                  p4est_quadrant_t * r);

/** Compute the face neighbor of a quadrant, transforming across tree
 * boundaries if necessary.
 * \param [in]     q      Input quadrant, must be valid.
 * \param [in]     t      Tree that contains \q.
 * \param [in]     face   The face across which to generate the neighbor.
 * \param [in,out] r      Existing quadrant whose Morton index will be filled.
 *                        By convention, if there is no tree across \face,
 *                        \r has the same Morton index as \q.
 * \param [in,out] nface  if not NULL, set to the face of \r that neighbors
 *                        \q.  nface is encoded with orientation information
 *                        in the same manner as the tree_to_face array in
 *                        the p4est_connectivity_t struct.
 * \param [in]     conn   The connectivity structure for the forest.
 * \return Returns the tree that contains \r.  By convention, if there is no
 * tree across \face, then -1 is returned.
 */
p4est_topidx_t      p4est_quadrant_face_neighbor_extra (const p4est_quadrant_t
                                                        * q, p4est_topidx_t t,
                                                        int face,
                                                        p4est_quadrant_t * r,
                                                        int *nface,
                                                        p4est_connectivity_t *
                                                        conn);

/** Get the smaller face neighbors of \a q.
 *
 * Gets the smaller face neighbors, which are half of the size assuming the
 * 2-1 constant.
 *
 * The order of the \a n[i] is given in the Morton ordering.
 *
 * \param [in]  q      The quadrant whose face neighbors will be constructed.
 * \param [in]  face   The face across which to generate the neighbors.
 * \param [out] n[0]..n[1] Filled with the four smaller face neighbors.
 * \param [out] nur[0]..nur[1] If not NULL, filled with smallest quadrants
 *                     that fit in the upper right corners of \a n.
 */
void                p4est_quadrant_half_face_neighbors (const p4est_quadrant_t
                                                        * q, int face,
                                                        p4est_quadrant_t n[],
                                                        p4est_quadrant_t
                                                        nur[]);

/** Create all possible face neighbors of \a q.
 *
 * Gets the face neighbors, possible assuming the 2-1 constraint.
 * If the larger or smaller quadrants do not exist than they are returned
 * as initialized by P4EST_QUADRANT_INIT.
 *
 * The order of \a n[0] through \a n[3] are given in Morton ordering.
 *
 * \param [in]  q      The quadrant whose face neighbors will be constructed.
 * \param [in]  face   The face across which to generate the neighbors.
 * \param [out] n[0]..n[1] Filled with the smaller possible face neighbors,
 *                     which are half of the size if they exist
 *                     or initialized to P4EST_QUADRANT_INIT.
 * \param [out] n[2]   Filled with the face neighbor, which is the same size.
 * \param [out] n[3]   Filled with the face neighbor, which is twice the size
 *                     if it exists or initialized to P4EST_QUADRANT_INIT.
 */
void                p4est_quadrant_all_face_neighbors (const p4est_quadrant_t
                                                       * q, int face,
                                                       p4est_quadrant_t n[]);

/** Compute the corner neighbor of a quadrant.
 * \param [in]     q      Input quadrant, must be valid.
 * \param [in]     corner The corner across which to generate the neighbor.
 * \param [in,out] r      Existing quadrant whose Morton index will be filled.
 * \note \a q may point to the same quadrant as \a r.
 */
void                p4est_quadrant_corner_neighbor (const p4est_quadrant_t *
                                                    q, int corner,
                                                    p4est_quadrant_t * r);

/** Compute the corner neighbors of a quadrant, transforming across tree
 * boundaries if necessary.  Only computes neighbors that are not face or edge
 * neighbors.
 * \param [in]     q      Input quadrant, must be valid.
 * \param [in]     t      Tree that contains \q.
 * \param [in]     corner The corner across which to generate the neighbor.
 * \param [in,out] quads  An initialized but empty array where the corner
 *                        neighbors will be placed.
 * \param [in,out] treeids An initialized but empty array where the ids of the
 *                        trees containing the corner neighbors will be placed.
 * \param [in,out] ncorners if not NULL, filled with the corners of \a quads
 *                          that neighbor \q.
 * \param [in]     conn   The connectivity structure for the forest.
 */
void                p4est_quadrant_corner_neighbor_extra (const
                                                          p4est_quadrant_t *
                                                          q, p4est_locidx_t t,
                                                          int corner,
                                                          sc_array_t * quads,
                                                          sc_array_t *
                                                          treeids,
                                                          sc_array_t *
                                                          ncorners,
                                                          p4est_connectivity_t
                                                          * conn);

/** Compute the half size corner neighbor of a quadrant.
 *
 * \param [in]  q       The quadrant whose corner neighbor will be constructed.
 * \param [in]  corner  The corner across which to generate the neighbor.
 * \param [out] r       Morton index filled with the half size corner neighbor.
 */
void                p4est_quadrant_half_corner_neighbor (const
                                                         p4est_quadrant_t * q,
                                                         int corner,
                                                         p4est_quadrant_t *
                                                         r);

/** Compute the corner node of a quadrant.
 * \param [in]     q      Input quadrant, must be valid.
 * \param [in]     corner The corner across which to generate the neighbor.
 * \param [in,out] r      Node that will not be clamped inside.
 * \note \a q may point to the same quadrant as \a r.
 */
void                p4est_quadrant_corner_node (const p4est_quadrant_t * q,
                                                int corner,
                                                p4est_quadrant_t * r);

/** Compute the 4 children of a quadrant.
 * \param [in]     q  Input quadrant.
 * \param [in,out] c0 First computed child.
 *                    \a q may point to the same quadrant as \a c0.
 * \note The user_data of \a c0, c1, c2, c3 is never modified.
 */
void                p4est_quadrant_children (const p4est_quadrant_t * q,
                                             p4est_quadrant_t * c0,
                                             p4est_quadrant_t * c1,
                                             p4est_quadrant_t * c2,
                                             p4est_quadrant_t * c3);

/** Compute the 4 children of a quadrant, array version.
 * \param [in]     q  Input quadrant.
 * \param [in,out] c  The 4 computed children in z-order.
 *                    q may point to the same quadrant as c[0].
 * \note The user_data of c[i] is never modified.
 */
void                p4est_quadrant_childrenv (const p4est_quadrant_t * q,
                                              p4est_quadrant_t c[]);

/** Compute the 4 children of a quadrant, array version.
 * \param [in]     q  Input quadrant.
 * \param [in,out] c  Pointers to the 4 computed children in z-order.
 *                    q may point to the same quadrant as c[0].
 * \note The user_data of c[i] is never modified.
 */
void                p4est_quadrant_childrenpv (const p4est_quadrant_t * q,
                                               p4est_quadrant_t * c[]);

/** Compute the first descendant of a quadrant on a given level.
 * \param [in]  q      Input quadrant.
 * \param [out] fd     First descendant of \a q on level \a level.
 * \param [in]  level  Level must be greater equal than q's level.
 */
void                p4est_quadrant_first_descendant (const p4est_quadrant_t *
                                                     q, p4est_quadrant_t * fd,
                                                     int level);

/** Compute the last descendant of a quadrant on a given level.
 * \param [in]  q      Input quadrant.
 * \param [out] ld     Last descendant of \a q on level \a level.
 * \param [in]  level  Level must be greater equal than q's level.
 */
void                p4est_quadrant_last_descendant (const p4est_quadrant_t *
                                                    q, p4est_quadrant_t * ld,
                                                    int level);

/** Compute the descendant of a quadrant touching a given corner.
 * \param [in]     q   Input quadrant.
 * \param [in,out] r   Existing quadrant whose Morton index will be filled.
 *                     Its user_data will be untouched.
 * \param [in]     c   The corner of \a q that \a r touches.
 * \param [in] level   The size of \a r.
 */
void                p4est_quadrant_corner_descendant (const p4est_quadrant_t *
                                                      q, p4est_quadrant_t * r,
                                                      int c, int level);

/** Computes the nearest common ancestor of two quadrants in the same tree.
 * \param [in]     q1 First input quadrant.
 * \param [in]     q2 Second input quadrant.
 * \param [in,out] r Existing quadrant whose Morton index will be filled.
 *                   Its user_data will be untouched.
 * \note \a q1, \a q2, \a r may point to the same quadrant.
 *       The user_data of \a r is never modified.
 */
void                p4est_nearest_common_ancestor (const p4est_quadrant_t *
                                                   q1,
                                                   const p4est_quadrant_t *
                                                   q2, p4est_quadrant_t * r);

/** Computes the nearest common ancestor of two quadrants in the same tree.
 * Descriptive, slower version of \a p4est_nearest_common_ancestor.
 * For debugging and educationial purposes only.
 */
void                p4est_nearest_common_ancestor_D (const p4est_quadrant_t *
                                                     q1,
                                                     const p4est_quadrant_t *
                                                     q2,
                                                     p4est_quadrant_t * r);

/** Transforms a quadrant/node across a face between trees.
 * \param [in]     q        Input quadrant/non-clamped node.
 * \param [in,out] r        Quadrant/node whose Morton index will be filled.
 * \param [in] ftransform   This array holds 9 integers.
 *             [0,2]        The coordinate axis sequence of the origin face.
 *             [3,5]        The coordinate axis sequence of the target face.
 *             [6,8]        Edge reverse flag for axis 0; face code for 1.
 *             [1,4,7]      0 (unused for compatibility with 3D).
 * \note \a q and \q r may NOT point to the same quadrant structure.
 */
void                p4est_quadrant_transform_face (const p4est_quadrant_t * q,
                                                   p4est_quadrant_t * r,
                                                   const int ftransform[]);

/** Checks if a quadrant touches a corner (diagonally inside or outside).
 */
int                 p4est_quadrant_touches_corner (const p4est_quadrant_t * q,
                                                   int corner, int inside);

/** Move a quadrant inside or diagonally outside a corner position.
 * \param [in,out] q        This quadrant only requires a valid level.
 * \param [in]     icorner  Number of the corner in 0..3.
 * \param [int]    inside   Boolean flag for inside or diagonally outside.
 */
void                p4est_quadrant_transform_corner (p4est_quadrant_t * q,
                                                     int icorner, int inside);

/** Shifts a quadrant until it touches the specified corner from the inside.
 * \param [in]     q          Valid input quadrant.
 * \param [in,out] r          Quadrant whose Morton index will be filled.
 * \param [in]     corner     Corner index.
 */
void                p4est_quadrant_shift_corner (const p4est_quadrant_t * q,
                                                 p4est_quadrant_t * r,
                                                 int corner);

/** Computes the linear position of a quadrant in a uniform grid.
 * \param [in] quadrant  Quadrant whose id will be computed.
 * \return Returns the linear position of this quadrant on a grid.
 * \note This is the inverse operation of p4est_quadrant_set_morton.
 *       The user_data of \a quadrant is never modified.
 */
uint64_t            p4est_quadrant_linear_id (const p4est_quadrant_t *
                                              quadrant, int level);

/** Set quadrant Morton indices based on linear position in uniform grid.
 * \param [in,out] quadrant  Quadrant whose Morton indices will be set.
 * \param [in]     id        The linear position of this quadrant on a grid.
 * \note This is the inverse operation of p4est_quadrant_linear_id.
 *       The user_data of \a quadrant is never modified.
 */
void                p4est_quadrant_set_morton (p4est_quadrant_t * quadrant,
                                               int level, uint64_t id);

SC_EXTERN_C_END;

#endif /* !P4EST_BITS_H */
