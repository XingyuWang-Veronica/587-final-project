#include "omp.h"
#include <iostream>
#include <queue>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

using namespace std;

enum State {Found, Invalid, Continue};

class Board {
public:
  int N;
  deque<int> positions; // positions[row] = col
  deque<unordered_set<int>> impossible_values; // impossible_values[row] = set of impossible columns
  Board(int n) {
    N = n;
    impossible_values.resize(N);
  }
  int update() {
    /*cout << "positions is ";
    for (int i = 0; i < positions.size(); i++) {
      cout << positions[i] << " ";
    }
    cout << '\n';*/
    int row = positions.size() - 1;
    int col = positions[row];
    for (int r = row + 1; r < N; r++) {
      impossible_values[r].insert(col);
      // cout << "(" << r << ", " << col << ") is impossible\n";
    }
    for (int i = 0; i < N; i++) {
      int r = row + i;
      int c = col + i;
      if (r >= N || c >= N) {
        continue;
      }
      impossible_values[r].insert(c);
      // cout << "(" << r << ", " << c << ") is impossible\n";
    }
    for (int i = 0; i < N; i++) {
      int r = row + i;
      int c = col - i;
      if (r >= N || c < 0) {
        continue;
      }
      impossible_values[r].insert(c);
      // cout << "(" << r << ", " << c << ") is impossible\n";
    }
    if (impossible_values[row + 1].size() == N) {
      return Invalid;
    }
    if (row == N - 2) {
      assert(impossible_values[N - 1].size() > 0);
      if (impossible_values[N - 1].size() < N) {
        for (int c = 0; c < N; c++) {
          if (!impossible_values[N - 1].count(c)) {
            // cout << "(" << N - 1 << ", " << c << ") is POSSIBLE\n";
            positions.push_back(c);
          }
        }
        return Found;
      }
      return Invalid;
    }
    return Continue;
  }
};

struct Compare {
  bool operator() (Board & b1, Board & b2) {
    // return b1.positions.size() < b2.positions.size();
    int b1_size = b1.N - b1.positions.size();
    int b2_size = b2.N - b2.positions.size();
    int b1_numImpossible = b1.impossible_values[b1.N - 1].size();
    int b2_numImpossible = b2.impossible_values[b2.N - 1].size();
    int p1 = (b1.N - b1_numImpossible) / b1_size;
    int p2 = (b2.N - b2_numImpossible) / b2_size;
    return p1 < p2;
  }
};

int main(int argc, char * argv[]) {
  if (argc < 2) {
    cerr << "Format: <exe> <N>\n";
    return 1;
  }
  int N = atoi(argv[1]);
  // Board final_board(N);
  int num_solutions = 0;
  priority_queue<Board, vector<Board>, Compare> pq;
  int num_tasks_alive = N;
  // bool found = false;
	// omp_lock_t mutex;
	// omp_init_lock(&mutex);
  double start_time = omp_get_wtime();

  #pragma omp parallel
  {
    // add at least N tasks to pq
    #pragma omp single
    {
      int n = omp_get_num_threads();
      for (int c = 0; c < N; c++) {
        Board new_board(N);
        new_board.positions.push_back(c);
        new_board.update();
        pq.push(new_board);
      }
    }
    #pragma omp barrier
    // int id = omp_get_thread_num();
    // while (num_tasks_alive > 0 && !found) {
    while (num_tasks_alive > 0) {
      Board top(N);
      bool shall_continue = false;
      #pragma omp critical
      {
        if (pq.empty()) {
          shall_continue = true;
        } else {
          top = pq.top();
          pq.pop();
        }
      }
      if (shall_continue) {
        continue;
      }
      // can continue
      int row = top.positions.size() - 1;
      int col = top.positions[row];
      for (int c = 0; c < N; c++) {
        int r = row + 1;
        if (!top.impossible_values[r].count(c)) {
          // cout << "(" << r << ", " << c << ") is POSSIBLE 2\n";
          Board new_board = top;
          new_board.positions.push_back(c);
          // update
          int ret = new_board.update();
          if (ret == Found) {
            // found = true;
            // omp_set_lock(&mutex);
            // final_board = new_board;
            /*for (int r = 0; r < N; r++) {
              for (int c = 0; c < N; c++) {
                if (final_board.positions[r] == c) {
                  cout << "x ";
                } else {
                  cout << "o ";
                }
              }
              cout << '\n';
            }*/
            // omp_unset_lock(&mutex);
            #pragma omp atomic
            num_solutions++;
            // break;
          } else if (ret == Invalid) {
            continue;
          } else if (ret == Continue) {
            // push this new task to pq
            #pragma omp critical
            {
              pq.push(new_board);
            }
            #pragma omp atomic
            num_tasks_alive++;
          }
        }
      }
      #pragma omp atomic
      num_tasks_alive--;
    }
  }
  double end_time = omp_get_wtime();
  double duration = end_time - start_time;
  // if (found) {
  //   cout << "Found a solution: \n";
  //   for (int r = 0; r < N; r++) {
  //     for (int c = 0; c < N; c++) {
  //       if (final_board.positions[r] == c) {
  //         cout << "Q ";
  //       } else {
  //         cout << "- ";
  //       }
  //     }
  //     cout << '\n';
  //   }
  // } else {
  //   cout << "No solution found\n";
  // }
  cout << "Number of solutions on a " << N << " * " << N << " board is " << num_solutions << '\n';
	cout << "Duration is " << duration << " in seconds\n";
  return 0;
}
