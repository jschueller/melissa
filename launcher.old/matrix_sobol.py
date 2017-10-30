###################################################################
#                            Melissa                              #
#-----------------------------------------------------------------#
#   COPYRIGHT (C) 2017  by INRIA and EDF. ALL RIGHTS RESERVED.    #
#                                                                 #
# This source is covered by the BSD 3-Clause License.             #
# Refer to the  LICENCE file for further information.             #
#                                                                 #
#-----------------------------------------------------------------#
#  Original Contributors:                                         #
#    Theophile Terraz,                                            #
#    Bruno Raffin,                                                #
#    Alejandro Ribes,                                             #
#    Bertrand Iooss,                                              #
###################################################################

import numpy as np
import numpy.random as rd

def create_matrix(nb_parameters, nb_groups, range_min, range_max):
    A = np.zeros((nb_groups, nb_parameters))
    for i in range(nb_parameters):
        A[:,i] = rd.uniform(range_min[i], range_max[i], nb_groups)
    return A

def create_matrix_k(A, B, k):
    Ck = np.copy(A)
    Ck[:,k] = B[:,k]
    return Ck
