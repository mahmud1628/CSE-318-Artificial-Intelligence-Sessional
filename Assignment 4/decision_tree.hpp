#ifndef decision_tree_hpp
#define decision_tree_hpp

#include "node.hpp"
#include <iostream>
#include <vector>
using namespace std;

struct DataPoint
{
    string class_label;
    unordered_map<string, string> attribute_values; // map of attribute names to their values
};

class DecisionTree
{
    Node * root; 
    int max_depth;

    Node * build_tree(const vector<DataPoint> & data, const vector<string> & attributes, int depth);

    public:
    DecisionTree(int max_depth = 5) : root(nullptr), max_depth(max_depth) {}
    ~DecisionTree() 
    {
        delete root; // delete the root node, which recursively deletes all child nodes
    }

    bool train(const vector<DataPoint> & data, const vector<string> & attrributes);
};

bool DecisionTree::train(const vector<DataPoint> & data, const vector<string> & attributes)
{
    if(data.empty() || attributes.empty())
    {
        cerr << "Error: Training data or attributes cannot be empty." << endl;
        return false;
    }

    this->root = build_tree(data, attributes, 0); // root node is at depth 0
    if(this->root == nullptr)
    {
        cerr << "Error: Failed to build the decision tree." << endl;
        return false;
    }
    return true; // Successfully built the tree
}

Node * DecisionTree::build_tree(const vector<DataPoint> & data, const vector<string> & attributes, int depth)
{
    if(depth == this->max_depth) // maximum depth reached
    {
        string majority_class_label;
        // TODO: Find the majority class label in the data here
        return new Node(true, "", majority_class_label, depth);
    }
}


#endif // decision_tree_hpp