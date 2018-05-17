/*
 *
 * Copyright (c) 2016 - 2018
 *   Stony Brook University
 * Copyright (c) 2015 - 2018
 *   Los Alamos National Security, LLC.
 * Copyright (c) 2011 - 2015
 *   University of Houston System and UT-Battelle, LLC.
 * Copyright (c) 2009 - 2015
 *   Silicon Graphics International Corp.  SHMEM is copyrighted
 *   by Silicon Graphics International Corp. (SGI) The OpenSHMEM API
 *   (shmem) is released by Open Source Software Solutions, Inc., under an
 *   agreement with Silicon Graphics International Corp. (SGI).
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * o Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 *
 * o Neither the name of the University of Houston System,
 *   UT-Battelle, LLC. nor the names of its contributors may be used to
 *   endorse or promote products derived from this software without specific
 *   prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*
 * This program shows, one to one communication and synchronization
 */

//#include "mpi.h"
#include <shmem.h>
#include <stdio.h>
int numtasks, rank;

// make the recieve argument global, shmem accessible
int inmsg1 = 9999;
int inmsg2 = 9999;

void
even()
{
    int outmsg1 = 4;
    int dest = 1;
    int source = 1;
    int rc, count, tag = 1;
    // MPI_Status Stat;
    // rc = MPI_Send(&outmsg1, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);
    // rc = MPI_Recv(&inmsg1, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    shmem_int_put(&inmsg2, &outmsg1, 1, dest);

    if (inmsg1 == 9999) {
        shmem_int_wait_until(&inmsg1, SHMEM_CMP_NE, 9999);
    }

    printf("Task %d: Received %d\n", rank, inmsg1);
}

void
odd()
{
    int dest = 0;
    int source = 0;
    int outmsg2 = 7;
    int rc, count, tag = 1;
    int temp2;
    // MPI_Status Stat;
    // rc = MPI_Recv(&inmsg2, 1, MPI_CHAR, source, tag, MPI_COMM_WORLD, &Stat);
    // rc = MPI_Send(&outmsg2, 1, MPI_CHAR, dest, tag, MPI_COMM_WORLD);

    shmem_int_put(&inmsg1, &outmsg2, 1, dest);

    if (inmsg2 == 9999) {
        shmem_int_wait_until(&inmsg2, SHMEM_CMP_NE, 9999);
    }

    printf("Task %d: Received %d\n", rank, inmsg2);
}

int
main(int argc, char *argv[])
{

    // MPI_Init(&argc,&argv);
    // MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    // MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    shmem_init();
    rank = shmem_my_pe();
    numtasks = shmem_n_pes();

    if (rank == 0) {
        even();
    }
    else if (rank == 1) {
        odd();
    }

    // MPI_Finalize();
    shmem_finalize();
    return 0;
}
