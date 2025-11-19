cmake_minimum_required(VERSION 3.24)

project(mpi-array-types LANGUAGES CXX)

find_package(MPI REQUIRED)

add_executable(mpi_subarray_example mpi_subarray_example.cpp)
target_link_libraries(mpi_subarray_example PUBLIC MPI::MPI_CXX)
target_compile_features(mpi_subarray_example PUBLIC cxx_std_23)

add_executable(mpi_darray_example mpi_darray_example.cpp)
target_link_libraries(mpi_darray_example PUBLIC MPI::MPI_CXX)
target_compile_features(mpi_darray_example PUBLIC cxx_std_23)
