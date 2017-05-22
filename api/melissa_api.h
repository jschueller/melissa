/**
 *
 * @file melissa_api.h
 * @author Terraz Théophile
 * @date 2016-09-05
 *
 * @defgroup melissa_api API
 *
 **/

#ifndef MELISSA_API_H
#define MELISSA_API_H


#ifdef BUILD_WITH_MPI
#include <mpi.h>

void melissa_init(const int *local_vect_size,
                  const int *comm_size,
                  const int *rank,
                  const int *sobol_rank,
                  const int *sobol_group,
                  MPI_Comm  *comm,
                  const int *coupling);

void melissa_init_f(int       *local_vect_size,
                    int       *comm_size,
                    int       *rank,
                    const int *sobol_rank,
                    const int *sobol_group,
                    MPI_Fint  *comm_fortran,
                    int       *coupling);

void melissa_send(const int  *time_step,
                  const char *field_name,
                  double     *send_vect,
                  const int  *rank,
                  const int  *sobol_rank,
                  const int  *sobol_group);

void melissa_finalize();
#endif // BUILD_WITH_MPI

#endif // MELISSA_API_H
