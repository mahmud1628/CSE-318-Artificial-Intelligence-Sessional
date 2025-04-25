#include<vector>
#include<iostream>
using namespace std;

class Node {
    private:
        int k, n;
        vector<vector<int>> board;
        int priority;
        int cost;
        Node * parent;
        pair<int, int> blankPosition;
        void setBlankPosition() {
            for (int i = 0; i < k; i++) {
                for (int j = 0; j < k; j++) {
                    if (board[i][j] == 0) {
                        blankPosition = make_pair(i, j);
                        return;
                    }
                }
            }
        }
    public:
        Node(int k, vector<vector<int>> board, Node * parent = nullptr) {
            this->k = k;
            this->n = k * k - 1;
            this->board = board;
            this->parent = parent;
            setBlankPosition();
            if(parent == nullptr) {
                this->cost = 0;
            } else {
                this->cost = parent->getCost() + 1;
            }
        }

        vector<vector<int>> getBoard() const {
            return board;
        }

        int getCost() {
            return cost;
        }

        int getK() {
            return k;
        }

        // void setCost(int cost) {
        //     this->cost = cost;
        // }
        int getPriority() {
            return priority;
        }
        void setPriority(int priority) {
            this->priority = priority;
        }
        Node * getParent() {
            return parent;
        }

        pair<int, int> getBlankPosition() {
            return blankPosition;
        }
        // void setParent(Node * parent) {
        //     this->parent = parent;
        // }

        void printBoard() {
            for(int i=0;i<k;i++) {
                for(int j=0;j<k;j++) {
                    cout << board[i][j] << " ";
                }
                cout << endl;
            }
            cout << endl;
        }
};