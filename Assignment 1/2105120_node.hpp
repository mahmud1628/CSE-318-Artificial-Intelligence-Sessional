#ifndef NODE_HPP
#define NODE_HPP

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

        Node * getChildByMovingBlankUp() {
            auto [blankRow, blankCol] = this->getBlankPosition();
            if(blankRow == 0) return nullptr;
        
            vector<vector<int>> newBoard = this->getBoard();
            swap(newBoard[blankRow][blankCol], newBoard[blankRow-1][blankCol]);
            Node * newNode = new Node(this->getK(),newBoard, this);
            return newNode;
        }
        
        Node * getChildByMovingBlankDown() {
            auto [blankRow, blankCol] = this->getBlankPosition();
            if(blankRow == this->getK() - 1) return nullptr;
        
            vector<vector<int>> newBoard = this->getBoard();
            swap(newBoard[blankRow][blankCol], newBoard[blankRow+1][blankCol]);
            Node * newNode = new Node(this->getK(),newBoard, this);
            return newNode;
        }
        
        Node * getChildByMovingBlankLeft() {
            auto [blankRow, blankCol] = this->getBlankPosition();
            if(blankCol == 0) return nullptr;
        
            vector<vector<int>> newBoard = this->getBoard();
            swap(newBoard[blankRow][blankCol], newBoard[blankRow][blankCol-1]);
            Node * newNode = new Node(this->getK(),newBoard, this);
            return newNode;
        }
        
        Node * getChildByMovingBlankRight() {
            auto [blankRow, blankCol] = this->getBlankPosition();
            if(blankCol == this->getK() - 1) return nullptr;
        
            vector<vector<int>> newBoard = this->getBoard();
            swap(newBoard[blankRow][blankCol], newBoard[blankRow][blankCol + 1]);
            Node * newNode = new Node(this->getK(),newBoard, this);
            return newNode;
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
        
        vector<Node *> getChildsOfCurrentNode() {
            vector<Node *> childs;
        
            childs.push_back(this->getChildByMovingBlankUp());
            childs.push_back(this->getChildByMovingBlankDown());
            childs.push_back(this->getChildByMovingBlankLeft());
            childs.push_back(this->getChildByMovingBlankRight());
        
            return childs;
        }

        string boardToString() {
            string result = "";
            for (const auto & row : board) {
                for (const auto & value : row) {
                    result += to_string(value) + ",";
                }
            }
            return result;
        }

        bool isGoalState() {
            for(int i=0;i<k;i++) {
                for(int j=0;j<k;j++) {
                    if(board[i][j] == 0) continue;
                    if(board[i][j] != i * k + j + 1 ) return false;
                }
            }
            return true;
        }
};

#endif 