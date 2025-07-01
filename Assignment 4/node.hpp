#ifndef node_hpp
#define node_hpp

#include <string>
#include <unordered_map>
using namespace std;

struct Node 
{
    bool is_leaf; // true if the current node is a leaf node
    string attribute_name; // attribute name selected for splitting
    string predicted_class; // class label if this is a leaf node
    int current_depth;
    unordered_map<string, Node*> children; // map of attribute values to child nodes

    Node(bool leaf = false, const string& attr_name = "", const string& class_label = "", int depth = 0)
    {
        is_leaf = leaf;
        attribute_name = attr_name;
        predicted_class = class_label;
        current_depth = depth;
    }

    ~Node()
    {
        for (auto& child : children)
        {
            delete child.second; // recursively delete child nodes
        }
    }

    Node (const Node&) = delete; // disable copy constructor
    Node& operator=(const Node&) = delete; // disable copy assignment operator
};

#endif // node_hpp