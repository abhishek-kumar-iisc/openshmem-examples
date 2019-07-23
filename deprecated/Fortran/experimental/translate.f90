!
! Copyright (c) 2016 - 2018
!   Stony Brook University
! Copyright (c) 2015 - 2018
!   Los Alamos National Security, LLC.
! Copyright (c) 2011 - 2015
!   University of Houston System and UT-Battelle, LLC.
! Copyright (c) 2009 - 2015
!   Silicon Graphics International Corp.  SHMEM is copyrighted
!   by Silicon Graphics International Corp. (SGI) The OpenSHMEM API
!   (shmem) is released by Open Source Software Solutions, Inc., under an
!   agreement with Silicon Graphics International Corp. (SGI).
!
! All rights reserved.
!
! Redistribution and use in source and binary forms, with or without
! modification, are permitted provided that the following conditions
! are met:
!
! o Redistributions of source code must retain the above copyright notice,
!   this list of conditions and the following disclaimer.
!
! o Redistributions in binary form must reproduce the above copyright
!   notice, this list of conditions and the following disclaimer in the
!   documentation and/or other materials provided with the distribution.
!
! o Neither the name of the University of Houston System,
!   UT-Battelle, LLC. nor the names of its contributors may be used to
!   endorse or promote products derived from this software without specific
!   prior written permission.
!
! THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
! "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
! LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
! A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
! HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
! SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
! TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
! PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
! LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
! NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
! SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
!

program translate
  include 'shmem.fh'

  integer :: shmem_my_pe, shmem_n_pes
  integer :: me, npes

  integer, parameter :: target_pe = 1

  integer*8, save :: symvar
  integer*8       :: remote_symvar
  integer*8       :: shmemx_lookup_remote_addr

  call shmem_init ()
  me = shmem_my_pe ()
  npes = shmem_n_pes ()

  write (*, *) 'PE ', me, 'symvar @ ', loc(symvar)
  call sleep(1)

  if (me .eq. 0) then
     remote_symvar = shmemx_lookup_remote_addr (symvar, target_pe);

     write (*, *) remote_symvar
  end if

end program translate