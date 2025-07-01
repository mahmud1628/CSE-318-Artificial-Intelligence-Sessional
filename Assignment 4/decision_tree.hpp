#ifndef decision_tree_hpp
#define decision_tree_hpp

#include "node.hpp"

class DecisionTree
{
    Node * root; 
    int max_depth;

    public:
    DecisionTree(int max_depth = 5) : root(nullptr), max_depth(max_depth) {}
    ~DecisionTree() 
    {
        delete root; // delete the root node, which recursively deletes all child nodes
    }
};


#endif // decision_tree_hpp