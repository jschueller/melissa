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
 * @file melissa_data.c
 * @brief Routines related to the melissa_data structure.
 * @author Terraz Théophile
 * @date 2016-24-05
 *
 * @defgroup melissa_data Melissa data
 *
 **/

//#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "melissa_data.h"
#include "melissa_utils.h"
#include "sobol.h"

static void melissa_alloc_data (melissa_data_t *data)
{
    int      i, j;
//    int32_t* items_ptr;

    if (data->is_valid != 1)
    {
        melissa_print (VERBOSE_ERROR, "Data structure not valid (malloc_data)\n");
        exit (1);
    }

    if (data->steps_init == 0)
    {
        alloc_vector (&data->step_simu, data->options->sampling_size);
        for (i=0; i<data->options->sampling_size; i++)
        {
            vector_add (&data->step_simu, melissa_calloc((data->options->nb_time_steps+31)/32, sizeof(int32_t)));
        }
        data->steps_init = 1;
    }

    if (data->vect_size <= 0)
    {
        return;
    }

    data->moments = melissa_malloc (data->options->nb_time_steps * sizeof(moments_t));

    if (data->options->kurtosis_op == 1)
    {
        j = 4;
    }
    else if (data->options->skewness_op == 1)
    {
        j = 3;
    }
    else if (data->options->variance_op == 1)
    {
        j = 2;
    }
    else if (data->options->mean_op == 1)
    {
        j = 1;
    }
    else
    {
        j = 0;
    }

    for (i=0; i<data->options->nb_time_steps; i++)
        init_moments (&(data->moments[i]), data->vect_size, j);

//    if (data->options->mean_op == 1 && data->options->variance_op == 0)
//    {
//        data->means = melissa_malloc (data->options->nb_time_steps * sizeof(mean_t));
//        for (i=0; i<data->options->nb_time_steps; i++)
//            init_mean (&(data->means[i]), data->vect_size);
//    }

//    if (data->options->variance_op == 1)
//    {
//        data->variances = melissa_malloc (data->options->nb_time_steps * sizeof(variance_t));
//        for (i=0; i<data->options->nb_time_steps; i++)
//            init_variance (&(data->variances[i]), data->vect_size);
//    }

    if (data->options->min_and_max_op == 1)
    {
        data->min_max = melissa_malloc (data->options->nb_time_steps * sizeof(min_max_t));
        for (i=0; i<data->options->nb_time_steps; i++)
            init_min_max (&(data->min_max[i]), data->vect_size);
    }

    if (data->options->threshold_op == 1)
    {
        data->thresholds = melissa_malloc (data->options->nb_time_steps * sizeof(threshold_t*));
        for (i=0; i<data->options->nb_time_steps; i++)
        {
            data->thresholds[i] = melissa_calloc (data->options->nb_thresholds, sizeof(threshold_t));
            for (j=0; j<data->options->nb_thresholds; j++)
            {
                init_threshold (&(data->thresholds[i][j]), data->vect_size, data->options->threshold[j]);
            }
        }
    }

    if (data->options->quantile_op == 1)
    {
        data->quantiles = melissa_malloc (data->options->nb_time_steps * sizeof(quantile_t*));
        for (i=0; i<data->options->nb_time_steps; i++)
        {
            data->quantiles[i] = melissa_malloc (data->options->nb_quantiles * sizeof(quantile_t));
            for (j=0; j<data->options->nb_quantiles; j++)
            {
                init_quantile (&(data->quantiles[i][j]), data->vect_size, data->options->quantile_order[j]);
            }
        }
    }

    if (data->options->sobol_op == 1)
    {
        data->sobol_indices = melissa_malloc (data->options->nb_time_steps * sizeof(sobol_array_t));
        data->init_sobol = init_sobol_martinez;
        data->read_sobol = read_sobol_martinez;
        data->save_sobol = save_sobol_martinez;
        data->increment_sobol = increment_sobol_martinez;
        data->free_sobol = free_sobol_martinez;
        for (i=0; i<data->options->nb_time_steps; i++)
        {
            data->init_sobol (&data->sobol_indices[i], data->options->nb_parameters, data->vect_size);
        }
    }
    data->stats_init = 1;
}

/**
 *******************************************************************************
 *
 * @ingroup melissa_data
 *
 * This function initializes the data structure
 *
 *******************************************************************************
 *
 * @param[out] *data
 * pointer to the structure containing global parameters
 *
 * @param[in] *options
 * pointer to the structure containing options parsed from command line
 *
 * @param[in] vect_size
 * sise of the local input vector
 *
 *******************************************************************************/

void melissa_init_data (melissa_data_t    *data,
                        melissa_options_t *options,
                        int                vect_size)
{
    data->vect_size       = vect_size;
    data->options         = options;
//    data->means           = NULL;
//    data->variances       = NULL;
    data->moments         = NULL;
    data->min_max         = NULL;
    data->thresholds      = NULL;
    data->quantiles       = NULL;
    data->sobol_indices   = NULL;
    melissa_check_data (data);
    melissa_alloc_data (data);
}

/**
 *******************************************************************************
 *
 * @ingroup stats_data
 *
 * This function checks the data structure. It terminates the program if one of
 * the mandatory option is missing or invalid, and correct mistakes for other options
 *
 *******************************************************************************
 *
 * @param[in,out] *data
 * pointer to the structure containing global parameters
 *
 *******************************************************************************/

void melissa_check_data (melissa_data_t *data)
{
    if (data->is_valid != 1)
    {
        // check options
        melissa_check_options(data->options);

        // every parameter is now validated
        data->is_valid = 1;
    }
}

/**
 *******************************************************************************
 *
 * @ingroup melissa_data
 *
 * This function frees the memory in the data structure
 *
 *******************************************************************************
 *
 * @param[in] *data
 * pointer to the structure containing global parameters
 *
 *******************************************************************************/

void melissa_free_data (melissa_data_t *data)
{
    int i, j;

    if (data->is_valid != 1)
    {
        melissa_print (VERBOSE_ERROR, "Data structure not valid (free_data)\n");
        exit (1);
    }

    for (i=0; i<data->options->nb_time_steps; i++)
        free_moments (&(data->moments[i]));
    melissa_free (data->moments);

//    if (data->options->mean_op == 1 && data->options->variance_op == 0)
//    {
//        for (i=0; i<data->options->nb_time_steps; i++)
//            free_mean (&(data->means[i]));
//        melissa_free (data->means);
//    }

//    if (data->options->variance_op == 1)
//    {
//        for (i=0; i<data->options->nb_time_steps; i++)
//            free_variance (&(data->variances[i]));
//        melissa_free (data->variances);
//    }

    if (data->options->min_and_max_op == 1)
    {
        for (i=0; i<data->options->nb_time_steps; i++)
            free_min_max (&(data->min_max[i]));
        melissa_free (data->min_max);
    }

    if (data->options->threshold_op == 1)
    {
        for (i=0; i<data->options->nb_time_steps; i++)
        {
            for (j=0; j<data->options->nb_thresholds; j++)
            {
                free_threshold (&(data->thresholds[i][j]));
            }
            melissa_free (data->thresholds[i]);
        }
        melissa_free (data->thresholds);
    }

    if (data->options->quantile_op == 1)
    {
        for (i=0; i<data->options->nb_time_steps; i++)
        {
            for (j=0; j<data->options->nb_quantiles; j++)
            {
                free_quantile (&(data->quantiles[i][j]));
            }
            melissa_free (data->quantiles[i]);
        }
        melissa_free (data->quantiles);
    }

    if (data->options->sobol_op == 1)
    {
        for (i=0; i<data->options->nb_time_steps; i++)
        {
            data->free_sobol (&data->sobol_indices[i], data->options->nb_parameters);
        }
        melissa_free (data->sobol_indices);
    }

    for (i=0; i<data->step_simu.size; i++)
    {
        melissa_free(vector_get(&data->step_simu, i));
    }
    free_vector (&data->step_simu);

    data->options = NULL;

    data->is_valid = 0;
}

///**
// *******************************************************************************
// *
// * @ingroup stats_data
// *
// * This function computes and displays the memory consumption of the library
// *
// *******************************************************************************
// *
// * @param[in] *options
// * pointer to the structure containing global options
// *
// *******************************************************************************/

//long int mem_conso (melissa_options_t *options)
//{
//    long int memory = 0;
//    long int temp_mem = 0;

//    if (options->mean_op != 0)
//    {
//        temp_mem = options->global_vect_size * options->nb_time_steps * sizeof(double)/* + sizeof(int)*/ / 1000000;
//        fprintf(stdout, " --- Mean memory usage:               %ld MB\n", temp_mem);
//        memory += temp_mem;
//    }
//    if (options->variance_op != 0 && options->mean_op == 0)
//    {
//        temp_mem = 2 * options->global_vect_size * options->nb_time_steps * sizeof(double)/* + sizeof(int)*/ / 1000000;
//        fprintf(stdout, " --- Variance memory usage:           %ld MB\n", temp_mem);
//        memory += temp_mem;
//    }
//    if (options->variance_op != 0 && options->mean_op != 0)
//    {
//        temp_mem = options->global_vect_size * options->nb_time_steps * sizeof(double)/* + sizeof(int)*/ / 1000000;
//        fprintf(stdout, " --- Variance memory usage:           %ld MB\n", temp_mem);
//        memory += temp_mem;
//    }
//    if (options->min_and_max_op != 0)
//    {
//        temp_mem = 2 * options->global_vect_size * options->nb_time_steps * sizeof(double)/* + sizeof(int)*/ / 1000000;
//        fprintf(stdout, " --- Min and max memory usage:        %ld MB\n", temp_mem);
//        memory += temp_mem;
//    }
//    if (options->threshold_op != 0)
//    {
//        temp_mem = options->global_vect_size * options->nb_time_steps * sizeof(int) / 1000000;
//        fprintf(stdout, " --- Threshold memory usage:          %ld MB\n", temp_mem);
//        memory += temp_mem;
//    }
//    if (options->sobol_op != 0)
//    {
//        // sobol indices
//        temp_mem  = options->nb_parameters * 2 * options->global_vect_size * options->nb_time_steps * sizeof(double) / 1000000;
//        // variances
//        temp_mem += options->nb_parameters * 2 * options->global_vect_size * options->nb_time_steps * sizeof(double) / 1000000;
//        // covariances
//        temp_mem += options->nb_parameters * 6 * options->global_vect_size * options->nb_time_steps * sizeof(double) / 1000000;
//        // glob variances
//        temp_mem += 2 * options->global_vect_size * options->nb_time_steps * sizeof(double) / 1000000;
//        fprintf(stdout, " --- Sobol indices memory usage:      %ld MB\n", temp_mem);
//        memory += temp_mem;
//    }
//    fprintf(stdout, " --- Total memory usage:      %d MB\n", memory);
//    return memory;
//}
