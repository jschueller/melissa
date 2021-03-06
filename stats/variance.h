/******************************************************************
*                            Melissa                              *
*-----------------------------------------------------------------*
*   COPYRIGHT (C) 2017  by INRIA and EDF. ALL RIGHTS RESERVED.    *
*                                                                 *
* This source is covered by the BSD 3-Clause License.             *
* Refer to the  LICENCE file for further information.             *
*                                                                 *
*-----------------------------------------------------------------*
*  Original Contributors:                                         *
*    Theophile Terraz,                                            *
*    Bruno Raffin,                                                *
*    Alejandro Ribes,                                             *
*    Bertrand Iooss,                                              *
******************************************************************/

/**
 *
 * @file variance.h
 * @author Terraz Théophile
 * @date 2017-15-01
 *
 **/

#ifndef VARIANCE_H
#define VARIANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_WITH_MPI
#include <mpi.h>
#endif // BUILD_WITH_MPI

/**
 *******************************************************************************
 *
 * @ingroup stats_base
 *
 * @struct variance_s
 *
 * Structure containing an array of variances and the corresponding mean structure
 *
 *******************************************************************************/

struct variance_s
{
    double *variance;       /**< variance[vect_size] */
    mean_t  mean_structure; /**< corresponding mean  */
};

typedef struct variance_s variance_t; /**< type corresponding to variance_s */

void init_variance(variance_t *variance,
                   const int   vect_size);

void increment_mean_and_variance (variance_t *partial_variance,
                                  double      in_vect[],
                                  const int   vect_size);

void increment_variance (variance_t *partial_variance,
                         double      in_vect[],
                         const int   vect_size);

void update_variance (variance_t *variance1,
                      variance_t *variance2,
                      variance_t *updated_variance,
                      const int   vect_size);

#ifdef BUILD_WITH_MPI

void update_global_variance (variance_t *variance,
                             const int   vect_size,
                             const int   rank,
                             const int   comm_size,
                             MPI_Comm    comm);

void update_global_mean_and_variance (variance_t *variance,
                                      const int   vect_size,
                                      const int   rank,
                                      const int   comm_size,
                                      MPI_Comm    comm);

#endif // BUILD_WITH_MPI

void save_variance(variance_t *vars,
                   int         vect_size,
                   int         nb_time_steps,
                   FILE*       f);

void read_variance(variance_t *vars,
                   int         vect_size,
                   int         nb_time_steps,
                   FILE*       f);

void free_variance (variance_t *variance);

#ifdef __cplusplus
}
#endif

#endif // VARIANCE_H
