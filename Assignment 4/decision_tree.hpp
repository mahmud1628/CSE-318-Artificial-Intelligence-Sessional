#ifndef decision_tree_hpp
#define decision_tree_hpp

#include "node.hpp"
#include <iostream>
#include <vector>
#include <cmath>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <limits>
using namespace std;

enum AttributeType {
    NUMERICAL, 
    CATEGORICAL 
};

extern unordered_map<string, AttributeType> attribute_value_types; // declared in main.cpp
struct DataPoint
{
    string class_label;
    unordered_map<string, string> attribute_values; // map of attribute names to their values
};

class DecisionTree
{
    Node * root;
    int max_depth;
    bool pruning_enabled = true; // whether to enable pruning or not
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

    // for numeric attributes
    double partition_entropy(const unordered_map<string, int> & partition, double total_size);
    pair<double, double> get_best_split_value_and_gain(const vector<DataPoint> & data, const string & attribute);
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
        if(max_depth == 0) 
        {
            pruning_enabled = false;
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
    if(data.empty())
    {
        return nullptr; // No data to build the tree
    }
    if(attributes.empty())
    {
        string majority_class_label = get_majority_class(data);
        return new Node(true, "", majority_class_label, depth);
    }
    if(pruning_enabled && depth == this->max_depth) // maximum depth reached
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

    string best_attribute = get_best_attribute(data, attributes);
    if(best_attribute.empty()) // no valid attribute found, return majority class
    {
        string majority_class_label = get_majority_class(data);
        return new Node(true, "", majority_class_label, depth);
    }

    Node * node = new Node(false, best_attribute, "", depth);

    if (attribute_value_types[best_attribute] == NUMERICAL)
    {
        auto [split_value, split_gain] = get_best_split_value_and_gain(data, best_attribute);

        if (split_gain <= 1e-10) // if gain is too small, negligible, return majority class
        {
            string majority_class_label = get_majority_class(data);
            return new Node(true, "", majority_class_label, depth);
        }

        vector<DataPoint> left, right;
        for (const auto & point : data)
        {
            try
            {
                double attribute_value = stod(point.attribute_values.at(best_attribute));
                (attribute_value < split_value ? left : right).push_back(point);
            }
            catch (...) {} // skip points with invalid numeric values
        }

        if (left.empty() || right.empty())
        {
            string majority_class = get_majority_class(data);
            return new Node(true, "", majority_class, depth);
        }

        vector<string> remaining_attributes;
        for(const auto & attr : attributes)
        {
            if(attr != best_attribute)
            {
                remaining_attributes.push_back(attr);
            }
        }

        string left_child = "<"  + to_string(split_value);
        string right_child = ">=" + to_string(split_value);

        node->children[left_child]  = build_tree(left , remaining_attributes, depth + 1);
        node->children[right_child]  = build_tree(right, remaining_attributes, depth + 1);
        return node;
    }

    // categorical attribute
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
        const string & attribute_name = current_node->attribute_name;

        if (attribute_value_types[attribute_name] == NUMERICAL)
        {
            // get split threshold from the node's children
            double split_value = numeric_limits<double>::quiet_NaN();
            for (const auto & value_node_pair : current_node->children)
            {
                size_t pos = value_node_pair.first.find_first_of("0123456789");
                if (pos != string::npos) 
                { 
                    split_value = stod(value_node_pair.first.substr(pos)); 
                    break; 
                }
            }
            if (isnan(split_value)) // not a number
            {
                return "";
            }

            double attribute_value = 0.0;
            try { 
                attribute_value = stod(point.attribute_values.at(attribute_name)); 
            }
            catch (...) { 
                return ""; 
            }

            string child_indicator; // indicates whether to go to left or right child
            if(attribute_value < split_value)
            {
                child_indicator = "<"  + to_string(split_value);
            }
            else 
            {
                child_indicator = ">=" + to_string(split_value);
            }
            unordered_map<string, Node*>::const_iterator it = current_node->children.find(child_indicator);
            if (it == current_node->children.end()) 
            {
                return "";
            }
            current_node = it->second;
        }
        else // categorical attribute, previous code only wrapped in a else block
        {
            string attribute_value = point.attribute_values.at(attribute_name);
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

    if (attribute_value_types[attribute] == NUMERICAL)
    {
        auto [value, gain] = get_best_split_value_and_gain(data, attribute);
        return gain;
    }

    // categorical, previous code   
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
    if(attribute_value_types[attribute] == NUMERICAL)
    {
        auto [value, gain] = get_best_split_value_and_gain(data, attribute);
        int left_subset_size = 0, right_subset_size = 0;
        for(const auto & point : data)
        {
            try
            {
                double attribute_value = stod(point.attribute_values.at(attribute));
                if(attribute_value < value) left_subset_size++;
                else right_subset_size++;
            }
            catch (...) {} // skip points with invalid numeric values
        }
        if(left_subset_size == 0 || right_subset_size == 0) return 0.0; // Avoid division by zero if no split is possible
        double left_size_ratio = static_cast<double>(left_subset_size) / data.size();
        double right_size_ratio = static_cast<double>(right_subset_size) / data.size();
        double intrinsic_value = - (left_size_ratio * log2(left_size_ratio) + right_size_ratio * log2(right_size_ratio));
        if(intrinsic_value == 0) return 0.0; // Avoid division by zero
        return gain / intrinsic_value; // Return information gain ratio
    }
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
    if(attribute_value_types[attribute] == NUMERICAL)
    {
        auto [value, gain] = get_best_split_value_and_gain(data, attribute);
        unordered_set<string> unique_values;
        for(const auto & point : data)
        {
            unique_values.insert(point.attribute_values.at(attribute));
        }
        int k = unique_values.size();
        double normalized_gain = 1 - (k - 1) / static_cast<double>(data.size());
        normalized_gain = gain * normalized_gain;
        normalized_gain = normalized_gain / log2(k + 1);
        return normalized_gain;
    }
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

// for numeric attributes

double DecisionTree::partition_entropy(const unordered_map<string, int> & class_counts, double total_size)
{
    if (total_size == 0) // avoid division by zero
    {
        return 0.0;
    }
    double entropy_value = 0.0;
    for (const auto & pair : class_counts)
    {
        double probability = pair.second / total_size;
        entropy_value -= probability * log2(probability);
    }
    return entropy_value;   
}

pair<double, double> DecisionTree::get_best_split_value_and_gain(const vector<DataPoint> & data, const string & attribute)
{
    vector<pair<double, string>> rows; // value - class label pairs
    for (const auto & point : data)
    {
        try { 
            rows.push_back({stod(point.attribute_values.at(attribute)), point.class_label}); 
        }
        catch (...) {}
    }

    if (rows.size() < 2) 
    { 
        return {0.0, 0.0}; // Not enough data to split
    }

    sort(rows.begin(), rows.end(), [](const pair<double, string> & a, const pair<double, string> & b){ return a.first < b.first; });


    unordered_map<string, int> right_class_counts; // counts of class labels in the right partition
    unordered_map<string, int> left_class_counts; // counts of class labels in the left partition
    for (const auto & row : rows) // initially all records are in the right partition
    {
        right_class_counts[row.second]++;
    }

    const double total_entropy = partition_entropy(right_class_counts, rows.size());

    double best_gain = -1.0, best_split_value = rows.front().first;

    for (size_t i = 0; i < rows.size() - 1; i++)
    {
        /* move record i → left */
        left_class_counts[rows[i].second]++;
        right_class_counts[rows[i].second]--;
        if (right_class_counts[rows[i].second] == 0)
        {
            right_class_counts.erase(rows[i].second);
        }

        if (rows[i].second == rows[i+1].second) continue;  // no class change

        double split_value = (rows[i].first + rows[i+1].first) / 2.0;
        double number_of_left_classes = i + 1, number_of_right_classes = rows.size() - number_of_left_classes;
        double gain = total_entropy - (number_of_left_classes / rows.size()) * partition_entropy(left_class_counts , number_of_left_classes) - (number_of_right_classes / rows.size()) * partition_entropy(right_class_counts, number_of_right_classes);

        if (gain > best_gain)
        {
            best_split_value = split_value;
            best_gain = gain;
        }
    }

    best_gain = max(0.0, best_gain);
    return {best_split_value, best_gain};
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