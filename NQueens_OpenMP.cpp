#include "omp.h"
#include <iostream>
#include <queue>
#include <deque>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <cassert>

using namespace std;

struct Pair {
  int row;
  int col;
}

enum State {Found, Invalid, Continue};

class Board {
public:
  int N;
  deque<int> positions; // positions[row] = col
  unordered_map<int, unordered_set<int>> impossible_values;
  int update() {
    int row = positions.size() - 1;
    int col = positions[row];
    for (int r = row + 1; r < N; r++) {
      impossible_values[r].insert(col);
    }
    for (int i = 0; i < N; i++) {
      int r = row + i;
      int c = col + i;
      if (r >= N || c >= N) {
        break;
      }
      impossible_values[r].insert(c);
    }
    for (int i = 0; i < N; i++) {
      int r = row + i;
      int c = col - i;
      if (r >= N || c < 0) {
        break;
      }
    }
    if (row == N - 2) {
      assert(impossible_values[N - 1].size() > 0);
      if (impossible_values[N - 1].size() < N) {
        for (int c = 0; c < N; c++) {
          if (!impossible_values[N - 1].count(c)) {
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
  bool operator() (Board const& b1, Board const& b2) {
    return b1.size() < b2.size();
  }
};

int main(int argc, char * argv[]) {
  if (argc < 2) {
    cerr << "Format: <exe> <N>\n";
    return 1;
  }
  int N = atoi(argv[1]);
  Board final_board;
  priority_queue<Board, vector<Board>, Compare> pq;
  int num_tasks_alive = N;
  bool found = false;
  double start_time = omp_get_wtime();

  #pragma omp parallel
  {
    // add at least N tasks to pq
    #pragma omp single
    {
      int n = omp_get_num_threads();
      for (int c = 0; c < N; c++) {
        Board new_board;
        new_board.N = N;
        new_board.positions.push_back(c);
        new_board.update();
        pq.push(new_board);
      }
    }
    #pragma omp barrier
    int id = omp_get_thread_num();
    while (num_tasks_alive > 0 && !found) {
      Board top;
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
        if (!impossible_values[r].count(c)) {
          Board new_board;
          new_board.positions = top.positions;
          new_board.N = top.N;
          new_board.positions.push_back(c);
          // update
          int ret = new_board.update();
          if (ret == Found) {
            found = true;
            #pragma omp atomic
            final_board = new_board;
            break;
          } else if (ret == Invalid) {
            continue;
          } else if (ret == Continue) {
            // push this new task to pq
            pq.push(new_board);
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
  if (found) {
    cout << "Found a solution: \n";
    for (int r = 0; r < N; r++) {
      for (int c = 0; c < N; c++) {
        if (final_board.positions[r] == c) {
          cout << "x ";
        } else {
          cout << "o ";
        }
      }
      cout << '\n';
    }
  } else {
    cout << "No solution found\n";
  }
	cout << "Duration is " << duration << "\n";
  return 0;
}
