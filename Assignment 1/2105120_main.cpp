#include<iostream>
#include<vector>
#include<queue>
#include<unordered_set>
#include<set>
#include<stack>
#include"2105120_heuristic.hpp"
#include"2105120_node.hpp"


using namespace std;

Node * getChildByMovingBlankDown(Node *);
Node * getChildByMovingBlankLeft(Node *);
Node * getChildByMovingBlankRight(Node *);
Node * getChildByMovingBlankUp(Node *);

string boardToString(const vector<vector<int>> & board) {
    string result = "";
    for (const auto & row : board) {
        for (const auto & value : row) {
            result += to_string(value) + ",";
        }
    }
    return result;
}

struct CompareByPriority {
    bool operator()(Node * a, Node * b) {
        return a->getPriority() > b->getPriority();
    }
};

struct ComapreByBoard {
    bool operator()(const Node* a, const Node* b) const {
        return a->getBoard() < b->getBoard();
    }
};


Node * getChildByMovingBlankUp(Node * node) {
    auto [blankRow, blankCol] = node->getBlankPosition();
    if(blankRow == 0) return nullptr;

    vector<vector<int>> newBoard = node->getBoard();
    swap(newBoard[blankRow][blankCol], newBoard[blankRow-1][blankCol]);
    Node * newNode = new Node(node->getK(),newBoard, node);
    return newNode;
}

Node * getChildByMovingBlankDown(Node * node) {
    auto [blankRow, blankCol] = node->getBlankPosition();
    if(blankRow == node->getK() - 1) return nullptr;

    vector<vector<int>> newBoard = node->getBoard();
    swap(newBoard[blankRow][blankCol], newBoard[blankRow+1][blankCol]);
    Node * newNode = new Node(node->getK(),newBoard, node);
    return newNode;
}

Node * getChildByMovingBlankLeft(Node * node) {
    auto [blankRow, blankCol] = node->getBlankPosition();
    if(blankCol == 0) return nullptr;

    vector<vector<int>> newBoard = node->getBoard();
    swap(newBoard[blankRow][blankCol], newBoard[blankRow][blankCol-1]);
    Node * newNode = new Node(node->getK(),newBoard, node);
    return newNode;
}

Node * getChildByMovingBlankRight(Node * node) {
    auto [blankRow, blankCol] = node->getBlankPosition();
    if(blankCol == node->getK() - 1) return nullptr;

    vector<vector<int>> newBoard = node->getBoard();
    swap(newBoard[blankRow][blankCol], newBoard[blankRow][blankCol + 1]);
    Node * newNode = new Node(node->getK(),newBoard, node);
    return newNode;
}

vector<Node *> getChildsOfCurrentNode(Node * node) {
    vector<Node *> childs;

    childs.push_back(getChildByMovingBlankUp(node));
    childs.push_back(getChildByMovingBlankDown(node));
    childs.push_back(getChildByMovingBlankLeft(node));
    childs.push_back(getChildByMovingBlankRight(node));

    return childs;
}

bool isGoalState(Node * node) {
    vector<vector<int>> board = node->getBoard();
    int k = board.size();
    for(int i=0;i<k;i++) {
        for(int j=0;j<k;j++) {
            if(board[i][j] == 0) continue;
            if(board[i][j] != i * k + j + 1 ) return false;
        }
    }
    return true;
}

bool isSolvable(Node * node) {
    vector<int> rowMajorOrder;
    vector<vector<int>> board = node->getBoard();
    int k = node->getK();
    for(int i = 0; i < k; i++) {
        for(int j=0; j < k; j++) {
            if(board[i][j] == 0 ) continue;
            rowMajorOrder.push_back(board[i][j]);
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
        auto [blankRow, blankCol] = node->getBlankPosition();
        blankRow = k - blankRow;
        if(blankRow % 2 == 0 && inversions % 2 != 0) return true;
        if(blankRow % 2 != 0 && inversions % 2 == 0) return true;
    }
    return false;
}

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


int main(int argc, char * argv[]) {
    int k;
    cin >> k;
    int explored = 0, expanded = 0;
    vector<vector<int>> initialBoard(k, vector<int>(k));
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            cin >> initialBoard[i][j];
        }
    }
    Node * initialNode = new Node(k, initialBoard);

    if(!isSolvable(initialNode)) {
        cout << "Unsolvable puzzle" << endl;
        delete initialNode;
        return 0;
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
            delete initialNode;
            return 0;
        }
    } else {
        heuristic = new ManhattanDistance(); // default heuristic if not provided from command line
    }

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

        if(isGoalState(currentNode)) {
            // write logic for printing the values now
            printResult(currentNode);
            cout << "Number of nodes explored : " << explored << endl;
            cout << "Number of nodes expanded : " << expanded << endl;
            for(auto node : allNodes) {
                delete node;
            }
            delete heuristic;
            return 0;
        }

        vector<Node *> childs = getChildsOfCurrentNode(currentNode);

        for(auto child : childs) {
            if(child == nullptr) continue;

            child->setPriority(child->getCost() + heuristic->calculateHeuristic(child->getBoard()));

            if(closedList.find(boardToString(child->getBoard())) == closedList.end()) {
                openList.push(child);
                explored++;
                allNodes.push_back(child);
            } else {
                delete child;
            }
        }

        if(expanded % 10000 == 0) {
            cout << "Expanded nodes: " << expanded << endl;
        }

        closedList.insert(boardToString(currentNode->getBoard()));
    }
    
}