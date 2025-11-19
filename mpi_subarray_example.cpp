#include <mpi.h>
#include <numeric>
#include <print>
#include <vector>

int main(int argc, char *argv[]) {
  MPI_Init(&argc, &argv);
  int rank = 0;
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  int size = 0;
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  // fill array with values [1.0, 2.0, ..., 16.0] in row major order
  std::vector<float> v(16, 0.0);
  std::iota(v.begin(), v.end(), 0.0);
  std::println("v: {}", v);

  // construct the subarray type for the inner 2x2 square
  std::array<int, 2> arr_size = {4, 4};
  std::array<int, 2> arr_subsize = {2, 2};
  std::array<int, 2> arr_substart = {1, 1};
  MPI_Datatype arr_type = MPI_DATATYPE_NULL;
  MPI_Type_create_subarray(2, arr_size.data(), arr_subsize.data(),
                           arr_substart.data(), MPI_ORDER_C, MPI_FLOAT,
                           &arr_type);
  MPI_Type_commit(&arr_type);

  // recv into contiguous vector
  std::vector<float> result(4, 0.0);
  // send to self
  MPI_Sendrecv(v.data(), 1, arr_type, rank, 0, result.data(), 4, MPI_FLOAT,
               rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

  // prints values of the inner square: [5, 6, 9, 10]
  std::println("result: {}", result);
  MPI_Type_free(&arr_type);
  return MPI_Finalize();
}
