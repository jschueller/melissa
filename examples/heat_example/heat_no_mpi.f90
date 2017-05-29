

program heat_no_mpi

  use heat_utils_no_mpi

  implicit none

  include "melissa_api.f90"

  integer :: nx, ny, n, nmax, nb_op, narg
  real*8 :: lx, ly, dt, dx, dy, d, t, epsilon, t1, t2, temp = 0
  real*8,dimension(:),pointer :: U => null(), F => null()
  real*8,dimension(3) :: A
  real*8,dimension(5) :: param
  character(len=32) :: arg
  integer ::  sample_id = 0, sobol_rank = 0
  character(len=5) :: name = C_CHAR_"heat"//C_NULL_CHAR

  narg = iargc()
  param(:) = 0
  if (narg .lt. 2) then
    print*,"Missing parameter"
    return
  endif
  do n=2, 6
    if(narg .ge. n) then
      call getarg(n, arg)
      read( arg, * ) param(n-1)
    endif
  enddo
  ! initial temperature
  temp = param(1)
  if(narg .gt. 3) then
    call getarg(narg - 1, arg)
    read( arg, * ) sobol_rank ! sobol rank
  endif
  if(narg .gt. 2) then
    call getarg(narg, arg)
    read( arg, * ) sample_id ! sobol group
  endif

  call cpu_time(t1)

  call read_file(nx, ny, lx, ly, d)

  nb_op   = nx*ny
  dt      = 0.01
  nmax    = 100
  dx      = lx/(nx+1)
  dy      = ly/(ny+1)
  epsilon = 0.0001

  allocate(U(nb_op))
  allocate(F(nb_op))
  call init(U, nb_op, temp)
  call filling_A(d, dx, dy, dt, A) ! fill A

  call melissa_init_no_mpi(nb_op, sobol_rank, sample_id)

  do n=1, nmax
    t = t + dt
    call filling_F(nx, ny, U, d, dx, dy, dt, t, F, nb_op, lx, ly, param)
    call conjgrad(A, F, U, nx, ny, epsilon)

  end do

  n = 1
  call melissa_send_no_mpi(n, name, u, sobol_rank, sample_id)
  call finalize(dx, dy, nx, ny, nb_op, u, f, sample_id)

  call melissa_finalize()
  
  call cpu_time(t2)
  print*,'Calcul time:', t2-t1, 'sec'
  print*,'Final time step:', t
  
  deallocate(U, F)

end program heat_no_mpi
