# ##---------------------------------------------------------------------------##
# ## EXAMPLES
# ##---------------------------------------------------------------------------##

# We require version 1.4.0 or higher but the format used by Google benchmark is
# wrong and thus, we cannot check the version during the configuration step.
FIND_PACKAGE(benchmark REQUIRED)

TRIBITS_ADD_EXECUTABLE(
  bvh
  SOURCES bvh_driver.cpp
  ADDED_EXE_TARGET_NAME_OUT bvh_exe_target_name
  )
TARGET_LINK_LIBRARIES(${bvh_exe_target_name} benchmark::benchmark)

IF(Kokkos_ENABLE_Serial)
  TRIBITS_ADD_TEST(
    bvh
    POSTFIX_AND_ARGS_0 serial --benchmark_filter=Serial --benchmark_color=false
    COMM serial mpi
    NUM_MPI_PROCS 1
    FAIL_REGULAR_EXPRESSION "data race;leak;runtime error"
    )
ENDIF()
IF(Kokkos_ENABLE_Cuda)
  TRIBITS_ADD_TEST(
    bvh
    POSTFIX_AND_ARGS_0 cuda --benchmark_filter=Cuda --benchmark_color=false
    COMM serial mpi
    NUM_MPI_PROCS 1
    FAIL_REGULAR_EXPRESSION "data race;leak;runtime error"
    )
ENDIF()
IF(Kokkos_ENABLE_OpenMP)
  TRIBITS_ADD_TEST(
    bvh
    POSTFIX_AND_ARGS_0 openmp --benchmark_filter=OpenMP --benchmark_color=false
    COMM serial mpi
    NUM_MPI_PROCS 1
    NUM_TOTAL_CORES_USED 4
    ENVIRONMENT OMP_NUM_THREADS=4
    FAIL_REGULAR_EXPRESSION "data race;leak;runtime error"
    )
ENDIF()
