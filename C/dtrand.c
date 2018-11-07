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
 * Distributed table: partitioned across PEs, locks protect updates
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <unistd.h>

#include <shmem.h>

const int table_size = 16;

/*
 * our table and the per-index locks
 */
int *table;
long *lock;

/*
 * partition the table
 */
int ip_pe;

/*
 * PE identity/program size
 */
int me, npes;

/*
 * which PE has this index of the array?
 */
#define OWNER(idx) ((idx) / ip_pe)
/*
 * where is this index locally?
 */
#define OFFSET(idx) ((idx) % ip_pe)

/*
 * from a given PE, I want to update index "idx", which might be
 * stored somewhere else
 */
void
table_update(int nv, int idx)
{
    const int q = OWNER(idx);   /* PE that owns this index */
    const int off = OFFSET(idx);    /* local table offset */

    shmem_set_lock(&lock[idx]);

    shmem_int_p(&table[off], nv, q);

    shmem_clear_lock(&lock[idx]);
}

/*
 * just to validate the updates
 */
void
table_dump(void)
{
    int i;

    for (i = 0; i < ip_pe; i += 1) {
        printf("PE %4d: table[local %4d](global %4d) = %d\n",
               me, i, (me * ip_pe) + i, table[i]);
    }
}

int
main(void)
{
    int table_bytes;
    int lock_bytes;
    int i;

    srand(getpid() + getuid());

    shmem_init();
    me = shmem_my_pe();
    npes = shmem_n_pes();

    /*
     * size of the per-PE partition
     */
    ip_pe = table_size / npes;

    /*
     * each PE only stores what it owns
     */
    table_bytes = sizeof(*table) * ip_pe;
    table = shmem_malloc(table_bytes);  /* !!! unchecked !!! */
    /*
     * initialize table
     */
    for (i = 0; i < ip_pe; i += 1) {
        table[i] = 0;
    }

    /*
     * each PE needs to be able to lock everywhere
     */
    lock_bytes = sizeof(*lock) * table_size;
    lock = shmem_malloc(lock_bytes);    /* !!! unchecked !!! */
    /*
     * initialize locks
     */
    for (i = 0; i < table_size; i += 1) {
        lock[i] = 0L;
    }

    /*
     * make sure all PEs have initialized symmetric data
     */
    shmem_barrier_all();

    for (i = 0; i < 4; i += 1) {
        const int updater = rand() % npes;

        if (me == updater) {
            const int i2u = rand() % table_size;
            const int nv = rand() % 100;

            printf("PE %d: About to update index %d with %d...\n", me, i2u,
                   nv);

            table_update(nv, i2u);
        }
    }

    shmem_barrier_all();

    /*
     * everyone shows their part of the table
     */
    table_dump();

    /*
     * clean up allocated memory
     */
    shmem_barrier_all();
    shmem_free(lock);
    shmem_free(table);

    shmem_finalize();

    return 0;
}
