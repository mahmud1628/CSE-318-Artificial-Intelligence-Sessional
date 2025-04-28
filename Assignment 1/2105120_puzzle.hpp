#include<iostream>
#include<vector>
#include<queue>
#include<unordered_set>
#include<set>
#include<stack>
#include"2105120_heuristic.hpp"
#include"2105120_node.hpp"


using namespace std;

struct CompareByPriority {
    bool operator()(Node * a, Node * b) {
        return a->getPriority() > b->getPriority();
    }
};




class Puzzle {
    private:
        int k;
        vector<vector<int>> initialBoard;
        Heuristic * heuristic;
        void printResult(Node * node) {
            cout << "Minimum number of moves = " << node->getCost() << endl << endl;
            stack<Node *> st;
            Node * temp = node;
            while(temp != nullptr) {
                st.push(temp);
                temp = temp->getParent();
            }
        
            while(!st.empty()) {
                temp = st.top();
                st.pop();
                temp->printBoard();
            }
        }

        pair<int, int> getBlankPositionOfInitialNode() {
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    if (initialBoard[i][j] == 0) {
                        return make_pair(i, j);
                    }
                }
            }
            return make_pair(-1, -1); // should not reach here
        }

    public :
        Puzzle(int k, vector<vector<int>> initialBoard, Heuristic * heuristic) {
            this->k = k;
            this->initialBoard = initialBoard;
            this->heuristic = heuristic;
        }

        ~Puzzle() {
            delete heuristic;
        }

        bool isSolvable() {
            vector<int> rowMajorOrder;
            for(int i = 0; i < k; i++) {
                for(int j=0; j < k; j++) {
                    if(initialBoard[i][j] == 0 ) continue;
                    rowMajorOrder.push_back(initialBoard[i][j]);
                }
            }
            int inversions = 0;
        
            set<int> st;
            st.insert(rowMajorOrder[0]);
            set<int>::iterator it;
            for(int i = 1; i < rowMajorOrder.size(); i++) {
                it = st.upper_bound(rowMajorOrder[i]);
                inversions += distance(it, st.end());
                st.insert(rowMajorOrder[i]);
            }
        
            // for(int i = 0 ; i< rowMajorOrder.size() ; i++) {
            //     for(int j = i + 1; j < rowMajorOrder.size() ;j++) {
            //         if(rowMajorOrder[i] > rowMajorOrder[j]) inversions++;
            //     }
            // }
        
        
            // cout << inversions << endl;
            // cout << k << endl;
        
            if(k % 2 != 0) {
                if(inversions % 2 == 0)
                    return true;
            } else {
                auto [blankRow, blankCol] = this->getBlankPositionOfInitialNode();
                blankRow = k - blankRow;
                if(blankRow % 2 == 0 && inversions % 2 != 0) return true;
                if(blankRow % 2 != 0 && inversions % 2 == 0) return true;
            }
            return false;
        }
        

        int getMinimumMoves(bool print = false) {

            if(!this->isSolvable()) {
                // cout << "Unsolvable puzzle" << endl;
                return -1;
            }

            int explored = 0, expanded = 0;
            Node * initialNode = new Node(k, initialBoard);
        
            initialNode->setPriority(heuristic->calculateHeuristic(initialBoard));
        
            priority_queue<Node *, vector<Node *>, CompareByPriority> openList;
            set<string> closedList;
            vector<Node * > allNodes;
            allNodes.push_back(initialNode);
        
            openList.push(initialNode);
            explored++;
        
            while(!openList.empty()) {
                Node * currentNode = openList.top();
                openList.pop();
                expanded++;
        
                if(currentNode->isGoalState()) {
                    // write logic for printing the values now
                    if(print) {
                        this->printResult(currentNode);
                        cout << "Number of nodes explored : " << explored << endl;
                        cout << "Number of nodes expanded : " << expanded << endl;
                    }

                    int moves = currentNode->getCost();
                    for(auto node : allNodes) {
                        delete node;
                    }
                    return moves;
                }
        
                vector<Node *> childs = currentNode->getChildsOfCurrentNode();
        
                for(auto child : childs) {
                    if(child == nullptr) continue;
        
                    child->setPriority(child->getCost() + heuristic->calculateHeuristic(child->getBoard()));
        
                    if(closedList.find(child->boardToString()) == closedList.end()) {
                        openList.push(child);
                        explored++;
                        allNodes.push_back(child);
                    } else {
                        delete child;
                    }
                }
        
                // if(expanded % 10000 == 0) { // for debugging
                //     cout << "Expanded nodes: " << expanded << endl;
                // }
        
                closedList.insert(currentNode->boardToString());
            }
            return -1;
        }
};