#ifndef HEURISTIC_HPP
#define HEURISTIC_HPP

#include<vector>
#include<cmath>
using namespace std;


class Heuristic
{   protected:
        pair<int,int> getGoalPosition(int value, int k) {
            int goalRow = (value - 1) / k;
            int goalCol = (value - 1) % k;
            return make_pair(goalRow, goalCol);
        }
    public:
        virtual double calculateHeuristic(const vector<vector<int>> & board) = 0;
};

class HammingDistance : public Heuristic {
    public:
        double calculateHeuristic(const vector<vector<int>> & board) {
            int hammingDistance = 0;
            int k = board.size();
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    if (board[i][j] != 0 && board[i][j] != i * k + j + 1) {
                        hammingDistance++;
                    }
                }
            }
            return hammingDistance;
        }
};

class ManhattanDistance : public Heuristic {
    public:
        double calculateHeuristic(const vector<vector<int>> & board) {
            int manhattanDistance = 0;
            int k = board.size();
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    if (board[i][j] != 0) {
                        int value = board[i][j];
                        auto [goalRow, goalCol] = getGoalPosition(value, k);
                        manhattanDistance += abs(i - goalRow) + abs(j - goalCol);
                    }
                }
            }
            return manhattanDistance;
        }
};


class EuclideanDistance : public Heuristic {
    public:
        double calculateHeuristic(const vector<vector<int>> & board) {
            double euclideanDistance = 0;
            int k = board.size();
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    if (board[i][j] != 0) {
                        int value = board[i][j];
                        auto [goalRow, goalCol] = getGoalPosition(value, k);
                        euclideanDistance += sqrt((i-goalRow) * (i - goalRow) + (j - goalCol) * (j - goalCol));
                    }
                }
            }
            return euclideanDistance;
        }
};

class LinearConflict : public Heuristic {
    private:
        double manhattanDistance( const vector<vector<int>> & board) {
            int manhattanDistance = 0;
            int k = board.size();
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    if (board[i][j] != 0) {
                        int value = board[i][j];
                        auto [goalRow, goalCol] = getGoalPosition(value, k);
                        manhattanDistance += abs(i - goalRow) + abs(j - goalCol);
                    }
                }
            }
            return manhattanDistance;
        }
    public:
        double calculateHeuristic(const vector<vector<int>> & board) {
            int linearConflict = 0;
            int k = board.size();

            for(int row = 0; row < k; row++) {
                for(int col = 0; col < k; col++) {
                    int value = board[row][col];
                    if(value == 0 ) continue;;
                    auto [goalRow, goalCol] = getGoalPosition(value, k);
                    if(goalRow != row) continue;
                    for(int j = col + 1; j < k ; j++) {
                        int nextValue = board[row][j];
                        if(nextValue == 0) continue;
                        auto [nextGoalRow, nextGoalCol] = getGoalPosition(nextValue, k);
                        if(nextGoalRow != row) continue;
                        if(goalCol > nextGoalCol) linearConflict++;
                    }
                }
            }

            for(int col = 0; col < k; col++) {
                for(int row = 0; row < k; row++) {
                    int value = board[row][col];
                    if(value == 0) continue;
                    auto [goalRow, goalCol] = getGoalPosition(value, k);
                    if(goalCol != col) continue;
                    for(int i = row + 1; i < k ; i++) {
                        int nextValue = board[i][col];
                        if(nextValue == 0) continue;
                        auto [nextGoalRow, nextGoalCol] = getGoalPosition(nextValue, k);
                        if(nextGoalCol != col) continue;
                        if(goalRow > nextGoalRow) linearConflict++;
                    }
                }
            }
            linearConflict = manhattanDistance(board) + 2 * linearConflict;
            return linearConflict;
        }
};

#endif 