#include "mpi.h"
#include <iostream>

using namespace std;

int main(int argc, char * argv[]) {
  if (argc < 2) {
    cerr << "Format: <exe> <N>\n";
    return 1;
  }
  int p, id;
  MPI_INIT(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);

  return 0;
}
