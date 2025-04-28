#include <iostream>
#include "2105120_puzzle.hpp"
#include "2105120_heuristic.hpp"

using namespace std;


int main(int argc, char * argv[]) {
    int k;
    cin >> k;
    vector<vector<int>> initialBoard(k, vector<int>(k));
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            cin >> initialBoard[i][j];
        }
    }

    Heuristic * heuristic;

    if(argc >= 2) {
        string heuristicType = argv[1];
        if(heuristicType == "manhattan") {
            heuristic = new ManhattanDistance();
        } else if(heuristicType == "euclidean") {
            heuristic = new EuclideanDistance();
        } else if(heuristicType == "linear") {
            heuristic = new LinearConflict();
        } else if(heuristicType == "hamming") {
            heuristic = new HammingDistance();
        } else {
            cout << "Invalid Heuristic Type" << endl;
            return 0;
        }
    } else {
        heuristic = new ManhattanDistance(); // default heuristic if not provided from command line
    }

    Puzzle * puzzle = new Puzzle(k, initialBoard, heuristic);

    if(!puzzle->isSolvable()) {
        cout << "Unsolvable puzzle" << endl;
        delete puzzle;
        return 0;
    }

    puzzle->getMinimumMoves(true);

    delete puzzle;

    return 0;

}