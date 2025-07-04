#ifndef decision_tree_hpp
#define decision_tree_hpp

#include "node.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_set>
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
    // Function pointer for attribute selection strategy
    double (DecisionTree::*attribute_selection_strategy)(const vector<DataPoint> &, const string &);

    Node * build_tree(const vector<DataPoint> & data, const vector<string> & attributes, int depth);
    string get_best_attribute(const vector<DataPoint> & data, const vector<string> & attributes);
    string get_majority_class(const vector<DataPoint> & data);
    bool is_pure_node(const vector<DataPoint> & data);


    void print_tree(Node * node, int depth);

    // selection criteria
    double entropy(const vector<DataPoint> & data);
    double information_gain(const vector<DataPoint> & data, const string & attribute);
    double information_gain_ratio(const vector<DataPoint> & data, const string & attribute);
    double normalized_weighted_information_gain(const vector<DataPoint> & data, const string & attribute);

    public:
    DecisionTree(int max_depth = 5, string selection_strategy = "ig") : root(nullptr), max_depth(max_depth) {
        if(selection_strategy == "ig") attribute_selection_strategy = &DecisionTree::information_gain;
        else if(selection_strategy == "igr") attribute_selection_strategy = &DecisionTree::information_gain_ratio;
        else if(selection_strategy == "nwig") attribute_selection_strategy = &DecisionTree::normalized_weighted_information_gain;
        else
        {
            cerr << "Error: Unsupported attribute selection strategy." << endl;
            attribute_selection_strategy = nullptr; // default to nullptr if unsupported
        }
    }
    ~DecisionTree() 
    {
        delete root; // delete the root node, which recursively deletes all child nodes
    }

    bool train(const vector<DataPoint> & data, const vector<string> & attrributes);
    string predict(const DataPoint & point);
    void print();
};

bool DecisionTree::train(const vector<DataPoint> & data, const vector<string> & attributes)
{
    if(data.empty() || attributes.empty())
    {
        cerr << "Error: Training data or attributes cannot be empty." << endl;
        return false;
    }

    root = build_tree(data, attributes, 0);
    if(root == nullptr)
    {
        cerr << "Error: Failed to build the decision tree." << endl;
        return false;
    }
    return true; // Successfully built the tree
}

Node * DecisionTree::build_tree(const vector<DataPoint> & data, const vector<string> & attributes, int depth)
{
    if(data.empty() || attributes.empty())
    {
        return nullptr; // No data to build the tree
    }
    if(depth == this->max_depth) // maximum depth reached
    {
        string majority_class_label = get_majority_class(data);
        return new Node(true, "", majority_class_label, depth);
    }
    // Check if all data points belong to the same class,i.e. pure node
    bool is_pure = is_pure_node(data);
    if(is_pure)
    {
        return new Node(true, "", data[0].class_label, depth);
    }

    // find the best attribute to split on
    string best_attribute = get_best_attribute(data, attributes);
    if(best_attribute.empty()) // no valid attribute found, return majority class
    {
        string majority_class_label = get_majority_class(data);
        return new Node(true, "", majority_class_label, depth);
    }

    Node * node = new Node(false, best_attribute, "", depth);
    unordered_set<string> attribute_values;
    for(const auto & point : data)
    {
        attribute_values.insert(point.attribute_values.at(best_attribute));
    }
    vector<string> remaining_attributes;
    for(const auto & attr : attributes)
    {
        if(attr != best_attribute)
        {
            remaining_attributes.push_back(attr);
        }
    }
    for(const auto & value : attribute_values)
    {
        vector<DataPoint> subset;
        for(const auto & point : data)
        {
            if(point.attribute_values.at(best_attribute) == value)
            {
                subset.push_back(point); // Collect data points with the current attribute value
            }
        }
        Node * child = build_tree(subset, remaining_attributes, depth + 1);
        node->children[value] = child;
    }

    return node;
}

string DecisionTree::predict(const DataPoint & point)
{
    if(root == nullptr)
    {
        cerr << "Error: Decision tree has not been trained." << endl;
        return ""; // Return empty string if tree is not trained
    }
    Node * current_node = root;
    while(!current_node->is_leaf)
    {
        string attribute_value = point.attribute_values.at(current_node->attribute_name);
        if(current_node->children.find(attribute_value) != current_node->children.end())
        {
            current_node = current_node->children[attribute_value]; // Move to the child node corresponding to the attribute value
        }
        else
        {
            cerr << "Warning: Attribute value '" << attribute_value << "' not found in the decision tree." << endl;
            return ""; // Return empty string if attribute value is not found
        }
    }
    return current_node->predicted_class; // Return the predicted class label at the leaf node
}

bool DecisionTree::is_pure_node(const vector<DataPoint> & data)
{
    if(data.empty()) return false; // Empty data cannot be pure
    string first_class = data[0].class_label;
    for(const auto & point : data)
    {
        if(point.class_label != first_class)
        {
            return false; // Found a different class label, not pure
        }
    }
    return true; // Didn't find any different class labels, so it's pure
}

string DecisionTree::get_majority_class(const vector<DataPoint> & data)
{
    unordered_map<string, int> class_counts;
    for(const auto & point : data)
    {
        class_counts[point.class_label]++;
    }
    string majority_class;
    int max_count = 0;
    for(const auto & pair : class_counts)
    {
        if(pair.second > max_count)
        {
            max_count = pair.second;
            majority_class = pair.first;
        }
    }
    return majority_class; // Return the class label with the highest count
}

string DecisionTree::get_best_attribute(const vector<DataPoint> & data, const vector<string> & attributes)
{
    double best_value = -1.0;
    string best_attribute;
    for(const auto & attribute : attributes)
    {
        double value = (this->*attribute_selection_strategy)(data, attribute);
        if(value > best_value)
        {
            best_value = value;
            best_attribute = attribute;
        }
    }
    cout << "Best value for attribute '" << best_attribute << "': " << best_value << endl;
    return best_attribute;
}





// selection criteria

double DecisionTree::entropy(const vector<DataPoint> & data)
{
    if(data.empty()) return 0.0;

    unordered_map<string, int> class_counts;

    for(const auto & point : data)
    {
        class_counts[point.class_label]++;
    }

    double entropy_value = 0.0;
    int total_count = data.size();
    for(const auto & pair : class_counts)
    {
        double probability = static_cast<double>(pair.second) / total_count;
        entropy_value -= probability * log2(probability);
    }
    return entropy_value;
}

double DecisionTree::information_gain(const vector<DataPoint> & data, const string & attribute)
{
    double total_entropy = entropy(data);
    unordered_map<string, vector<DataPoint>> subsets;
    for(const auto & point : data)
    {
        subsets[point.attribute_values.at(attribute)].push_back(point);
    }
    double weighted_entropy = 0.0;
    int total_count = data.size();
    for(const auto & pair : subsets)
    {
        double subset_entropy = entropy(pair.second);
        weighted_entropy += (static_cast<double>(pair.second.size()) / total_count) * subset_entropy;
    }
    return total_entropy - weighted_entropy;
}

double DecisionTree::information_gain_ratio(const vector<DataPoint> & data, const string & attribute)
{
    double total_entropy = entropy(data);
    unordered_map<string, vector<DataPoint>> subsets;
    for(const auto & point : data)
    {
        subsets[point.attribute_values.at(attribute)].push_back(point);
    }
    double weighted_entropy = 0.0;
    int total_count = data.size();
    for(const auto & pair : subsets)
    {
        double subset_entropy = entropy(pair.second);
        weighted_entropy += (static_cast<double>(pair.second.size()) / total_count) * subset_entropy;
    }
    double gain = total_entropy - weighted_entropy;
    // Calculate intrinsic value
    double intrinsic_value = 0.0;
    for(const auto & pair : subsets)
    {
        double size_ratio = static_cast<double>(pair.second.size()) / total_count;
        intrinsic_value -= size_ratio * log2(size_ratio);
    }
    if(intrinsic_value == 0) return 0.0; // Avoid division by zero
    return gain / intrinsic_value; // Return information gain ratio
}

double DecisionTree::normalized_weighted_information_gain(const vector<DataPoint> & data, const string & attribute)
{
    double information_gain_value = information_gain(data, attribute);
    int k; // Number of unique values for the attribute
    unordered_set<string> unique_values;
    for(const auto & point : data)
    {
        unique_values.insert(point.attribute_values.at(attribute));
    }
    k = unique_values.size();
    double normalized_gain = 1 - (k - 1) / static_cast<double>(data.size());
    normalized_gain = information_gain_value * normalized_gain;
    normalized_gain = normalized_gain / log2(k + 1);
    return normalized_gain;
}

// print
void DecisionTree::print_tree(Node* node, int tree_depth) 
{
    if (node == nullptr) return;
    
    std::string indent(tree_depth * 4, ' ');
    if (node->is_leaf) {
        std::cout << indent << "-> Predict: " << node->predicted_class << "\n";
    } else {
        std::cout << indent << "Split on: " << node->attribute_name << "?\n";
        for (const auto& pair : node->children) {
            std::cout << indent << "    - " << pair.first << ":\n";
            print_tree(pair.second, tree_depth + 1);
        }
    }
}

void DecisionTree::print()
{
    print_tree(root, 0);
}


#endif // decision_tree_hpp