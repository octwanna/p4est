/*
  This file is part of p4est.
  p4est is a C library to manage a parallel collection of quadtrees and/or
  octrees.

  Copyright (C) 2008 Carsten Burstedde, Lucas Wilcox.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef P8EST_H
#define P8EST_H

/* p8est_connectivity.h includes p4est_base.h sc_containers.h */
#include <p8est_connectivity.h>

/* the spatial dimension */
#define P8EST_DIM 3
#define P8EST_CHILDREN 8
#define P8EST_INSUL 27
#define P8EST_STRING "p8est"

/** Finest level of the octree for representing nodes */
#define P8EST_MAXLEVEL 19

/** Finest level of the octree for representing octants */
#define P8EST_QMAXLEVEL 18

/* the length of a root quadrant */
#define P8EST_ROOT_LEN ((p4est_qcoord_t) 1 << P8EST_MAXLEVEL)

/* the length of a quadrant of level l */
#define P8EST_QUADRANT_LEN(l) ((p4est_qcoord_t) 1 << (P8EST_MAXLEVEL - (l)))

/* the offset of the highest quadrant at level l */
#define P8EST_LAST_OFFSET(l) (P8EST_ROOT_LEN - P8EST_QUADRANT_LEN (l))

/* a negative magic number for consistency checks */
#define P8EST_NEG_MAGIC = -439623173;

typedef struct p8est_quadrant
{
  p4est_qcoord_t      x, y, z;
  int8_t              level, pad8;
  int16_t             pad16;
  union p8est_quadrant_data
  {
    void               *user_data;
    p4est_topidx_t      which_tree;
    struct
    {
      p4est_topidx_t      which_tree;
      int                 owner_rank;
    }
    piggy1;
    struct
    {
      p4est_topidx_t      which_tree;
      p4est_topidx_t      from_tree;
    }
    piggy2;
    struct
    {
      p4est_topidx_t      which_tree;
      p4est_locidx_t      local_num;
    }
    piggy3;
  }
  p;
}
p8est_quadrant_t;

typedef struct p8est_tree
{
  sc_array_t          quadrants;        /* locally stored quadrants */
  p8est_quadrant_t    first_desc, last_desc;    /* first and last descendent */
  p4est_locidx_t      quadrants_offset; /* cumulative sum over earlier tress */
  p4est_locidx_t      quadrants_per_level[P8EST_MAXLEVEL + 1];  /* locals only */
  int8_t              maxlevel; /* highest local quadrant level */
}
p8est_tree_t;

typedef struct p8est
{
  MPI_Comm            mpicomm;
  int                 mpisize, mpirank;

  size_t              data_size;        /* size of per-quadrant user_data */
  void               *user_pointer;     /* convenience pointer for users,
                                           will never be touched by p8est */

  p4est_topidx_t      first_local_tree; /* 0-based index of first local tree,
                                           must be -1 for an empty processor */
  p4est_topidx_t      last_local_tree;  /* 0-based index of last local tree,
                                           must be -2 for an empty processor */
  p4est_locidx_t      local_num_quadrants;      /* number of quadrants
                                                   on all trees on this processor */
  p4est_gloidx_t      global_num_quadrants;     /* number of quadrants
                                                   on all trees on all processors */
  p4est_gloidx_t     *global_last_quad_index;   /* Index in the total ordering
                                                   of all quadrants of the
                                                   last quadrant on each proc.
                                                 */
  p8est_quadrant_t   *global_first_position;    /* first smallest possible quadrant
                                                   for each processor and 1 beyond */
  p8est_connectivity_t *connectivity;   /* connectivity structure */
  sc_array_t         *trees;    /* list of all trees */

  sc_mempool_t       *user_data_pool;   /* memory allocator for user data
                                         * WARNING: This is NULL if data size
                                         *          equals zero.
                                         */
  sc_mempool_t       *quadrant_pool;    /* memory allocator for temporary quadrants */
}
p8est_t;

/** Callback function prototype to initialize the quadrant's user data.
 */
typedef void        (*p8est_init_t) (p8est_t * p8est,
                                     p4est_topidx_t which_tree,
                                     p8est_quadrant_t * quadrant);

/** Callback function prototype to decide for refinement.
 * \return nonzero if the quadrant shall be refined.
 */
typedef int         (*p8est_refine_t) (p8est_t * p8est,
                                       p4est_topidx_t which_tree,
                                       p8est_quadrant_t * quadrant);

/** Callback function prototype to decide for coarsening.
 * \param [in] quadrants   Pointers to 8 siblings in Morton ordering.
 * \return nonzero if the quadrants shall be replaced with their parent.
 */
typedef int         (*p8est_coarsen_t) (p8est_t * p8est,
                                        p4est_topidx_t which_tree,
                                        p8est_quadrant_t * quadrants[]);

/** Callback function prototype to calculate weights for partitioning.
 * \return a 32bit integer >= 0 as the quadrant weight.
 * \note    (global sum of weights * mpisize) must fit into a 64bit integer.
 */
typedef int         (*p8est_weight_t) (p8est_t * p8est,
                                       p4est_topidx_t which_tree,
                                       p8est_quadrant_t * quadrant);

extern void        *P8EST_DATA_UNINITIALIZED;
extern const int    p8est_num_ranges;

/** set statically allocated quadrant to defined values */
#define P8EST_QUADRANT_INIT(q) \
  do { memset ((q), -1, sizeof (p8est_quadrant_t)); } while (0)

/** Create a new p8est.
 *
 * \param [in] mpicomm       A valid MPI_Comm or MPI_COMM_NULL.
 * \param [in] connectivity  This is the connectivity information that
 *                           the forest is built with.  Note the p8est
 *                           does not take ownership of the memory.
 * \param [in] min_quadrants Minimum initial number of quadrants per processor.
 * \param [in] data_size     This is the size of data for each quadrant which
 *                           can be zero.  Then user_data_pool is set to NULL.
 * \param [in] init_fn       Callback function to initialize the user_data
 *                           which is already allocated automatically.
 * \param [in] user_pointer  Assign to the user_pointer member of the p8est
 *                           before init_fn is called the first time.
 *
 * \return This returns a vaild forest.
 *
 * \note The connectivity structure must not be destroyed
 *       during the lifetime of this p8est.
 */
p8est_t            *p8est_new (MPI_Comm mpicomm,
                               p8est_connectivity_t * connectivity,
                               p4est_locidx_t min_quadrants, size_t data_size,
                               p8est_init_t init_fn, void *user_pointer);

/** Destroy a p8est.
 * \note The connectivity structure is not destroyed with the p8est.
 */
void                p8est_destroy (p8est_t * p8est);

/** Make a deep copy of a p8est.  Copying of quadrant user data is optional.
 * \param [in]  copy_data  If true, data are copied.
 *                         If false, data_size is set to 0.
 * \return  Returns a valid p8est that does not depend on the input.
 */
p8est_t            *p8est_copy (p8est_t * input, bool copy_data);

/** Refine a forest.
 * \param [in] refine_fn Callback function that returns true
 *                       if a quadrant shall be refined
 * \param [in] init_fn   Callback function to initialize the user_data
 *                       which is already allocated automatically.
 */
void                p8est_refine (p8est_t * p8est,
                                  bool refine_recursive,
                                  p8est_refine_t refine_fn,
                                  p8est_init_t init_fn);

/** Refine a forest with a bounded maximum refinement level.
 * \param [in] refine_fn Callback function that returns true
 *                       if a quadrant shall be refined
 * \param [in] init_fn   Callback function to initialize the user_data
 *                       which is already allocated automatically.
 * \param [in] maxlevel  Maximum allowed level (inclusive) of quadrants.
 */
void                p8est_refine_level (p8est_t * p8est,
                                        bool refine_recursive,
                                        p8est_refine_t refine_fn,
                                        p8est_init_t init_fn,
                                        int allowed_level);

/** Coarsen a forest.
 * \param [in] coarsen_fn Callback function that returns true if a
 *                        family of quadrants shall be coarsened
 * \param [in] init_fn    Callback function to initialize the user_data
 *                        which is already allocated automatically.
 */
void                p8est_coarsen (p8est_t * p8est,
                                   bool coarsen_recursive,
                                   p8est_coarsen_t coarsen_fn,
                                   p8est_init_t init_fn);

/** Balance a forest. Currently only doing local balance.
 * \param [in] init_fn   Callback function to initialize the user_data
 *                       which is already allocated automatically.
 * \note Balances edges and corners.
 *       Can be easily changed to edges only in p8est_algorithms.c.
 */
void                p8est_balance (p8est_t * p8est, p8est_init_t init_fn);

/** Equally partition the forest.
 *
 * The forest will be partitioned between processors where they each
 * have an approximately equal number of quadrants.
 *
 * \param [in,out] p8est      The forest that will be partitioned.
 * \param [in]     weight_fn  A weighting function or NULL
 *                            for uniform partitioning.
 */
void                p8est_partition (p8est_t * p8est,
                                     p8est_weight_t weight_fn);

/** Compute the checksum for a forest.
 * Based on quadrant arrays only. It is independent of partition and mpisize.
 * \return  Returns the checksum on processor 0 only. 0 on other processors.
 */
unsigned            p8est_checksum (p8est_t * p8est);

#endif /* !P8EST_H */
