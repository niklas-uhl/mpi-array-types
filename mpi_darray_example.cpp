#include <mpi.h>
#include <numeric>
#include <print>
#include <ranges>
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

  // // construct the subarray type for the inner 2x2 square
  // std::array<int, 2> arr_size = {4, 4};
  // std::array<int, 2> arr_subsize = {2, 2};
  // std::array<int, 2> arr_substart = {1, 1};
  // MPI_Datatype arr_type = MPI_DATATYPE_NULL;
  // MPI_Type_create_subarray(2, arr_size.data(), arr_subsize.data(),
  //                          arr_substart.data(), MPI_ORDER_C, MPI_FLOAT,
  //                          &arr_type);
  // MPI_Type_commit(&arr_type);
  std::array<int, 2> gsize_array = {4, 4};
  std::array<int, 2> distrib_array = {MPI_DISTRIBUTE_BLOCK,
                                      MPI_DISTRIBUTE_BLOCK};
  std::array<int, 2> darg_array = {MPI_DISTRIBUTE_DFLT_DARG,
                                   MPI_DISTRIBUTE_DFLT_DARG};
  std::array<int, 2> psize_array = {2, 2};

  std::array<MPI_Datatype, 4> chunks;
  // define type corresponding to disjoint subarrays of size 2x2
  for (auto [idx, arr_type] : std::views::enumerate(chunks)) {
    arr_type = MPI_DATATYPE_NULL;

    MPI_Type_create_darray(4, idx, 2, gsize_array.data(), distrib_array.data(),
                           darg_array.data(), psize_array.data(), MPI_ORDER_C,
                           MPI_FLOAT, &arr_type);
    MPI_Type_commit(&arr_type);
  }

  // output:
  // result: [0, 1, 4, 5]
  // result: [2, 3, 6, 7]
  // result: [8, 9, 12, 13]
  // result: [10, 11, 14, 15]
  std::vector<float> result(16, 0.0);
  MPI_Status status;
  for (auto const &arr_type : chunks) {
    // send to self
    MPI_Sendrecv(v.data(), 1, arr_type, rank, 0, result.data(), 4, MPI_FLOAT,
                 rank, 0, MPI_COMM_WORLD, &status);
    int recv_size = 0;
    MPI_Get_count(&status, MPI_FLOAT, &recv_size);
    std::println("result: {}", result | std::views::take(recv_size));
  }

  for (auto &arr_type : chunks) {
    MPI_Type_free(&arr_type);
  }
  return MPI_Finalize();
}
