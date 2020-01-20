#include "mpi.h"
#include "omp.h"
#include <iostream>
#include <queue>


using namespace std;

int main(int argc, char * argv[]) {
  if (argc < 2) {
    cerr << "Format: <exe> <N>\n";
    return 1;
  }
  int p, id, provided;
  // MPI_INIT(&argc, &argv);
  MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
  MPI_Comm_size(MPI_COMM_WORLD, &p);
  MPI_Comm_rank(MPI_COMM_WORLD, &id);
  // initialization: tasks by default
  queue<> local_tasks;
  // a thread dedicated to listening to:
  // (1) "STOP" -- have found the solution
  // (2) "Give me more tasks" -- use mutex
  // (3) "FIN" -- no solution is found
  // a thread dedicated to update:
  // (1) update the local queue -- use mutex
  // (2) if there are no tasks left, send "Give me more tasks"
  //     to each of the other processes and wait for responses,
  //     then go to (1) or (4)
  // (3) if have found the solution, send "STOP" to all the 
  //     other processes and print the output
  // (4) if all the processes respond with no more tasks, return
  //     and if this is process 0, print "No solution found"
  omp_set_num_threads(2);
  int ithread;
  bool stop = false, fin = false;
  omp_lock_t mutex;
  omp_init_lock(&mutex);
  #pragma omp parallel private(ithread)
  {
    ithread = omp_get_thread_num();
    if (!ithread) {
      while (!stop && !fin) {
      
      }   
    } else {
      while (!stop) {
        omp_set_lock(&mutex);
        
        omp_unset_lock(&mutex);
      }
    }
  }
  return 0;
}
