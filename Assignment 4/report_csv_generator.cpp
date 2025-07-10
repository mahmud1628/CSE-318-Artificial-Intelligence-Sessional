#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "decision_tree.hpp"
#include "data_loader.hpp"
using namespace std;

ofstream node_depth_csv_file("node_depth.csv");



void get_accuracy(const vector<DataPoint> & data, vector<string> & attributes, int max_depth, const string & selection_strategy, double & average_training_accuracy, double & average_test_accuracy, unordered_map<string, AttributeType> & attribute_value_types, int & node_count, int & depth_count)
{
    double test_accuracy = 0.0;
    double training_accuracy = 0.0;
    int nodes = 0;
    int depths = 0;

    for(int i = 0; i < 20; i++)
    {
        DecisionTree tree(max_depth, selection_strategy, attribute_value_types);
        vector<DataPoint> training_data, test_data;
        split_data(data, training_data, test_data);
        tree.train(training_data, attributes);
        if(max_depth == 0)
        {
            nodes += tree.get_tree_size();
            depths += tree.get_tree_depth();
        }
        int correct_predictions_with_training_data = 0;
        for(const auto & point : training_data)
        {
            string predicted_class = tree.predict(point);
            if(predicted_class == point.class_label)
            {
                correct_predictions_with_training_data++;
            }
        }
        training_accuracy += static_cast<double>(correct_predictions_with_training_data) / training_data.size();
        int correct_predictions_with_test_data = 0;
        for(const auto & point : test_data)
        {
            string predicted_class = tree.predict(point);
            if(predicted_class == point.class_label)
            {
                correct_predictions_with_test_data++;
            }
        }
        test_accuracy += static_cast<double>(correct_predictions_with_test_data) / test_data.size();
    }

    average_training_accuracy = training_accuracy / 20.0;
    average_test_accuracy = test_accuracy / 20.0;
    node_count = nodes / 20;
    depth_count = depths / 20;
}


void generate_csv(string selection_strategy, string file_name)
{
    string csv_file_name = file_name + "_" + selection_strategy + ".csv";
    ofstream csv_file(csv_file_name);
    if(!csv_file.is_open())
    {
        cerr << "Error: Could not open file " << csv_file_name << " for writing." << endl;
        return; 
    }

    csv_file << "Max Depth,Training Accuracy,Test Accuracy\n";
    csv_file.flush(); // Ensure header is written to the file immediately
    vector<DataPoint> data;
    vector<string> attributes;
    string class_label_name;
    unordered_map<string, AttributeType> attribute_value_types;
    if(file_name == "iris")
    {
        read_iris_file(data, attributes, class_label_name, attribute_value_types);
    }
    else if(file_name == "adult")
    {
        read_adult_file(data, attributes, class_label_name, attribute_value_types);
    }
    else
    {
        cerr << "Error: Unsupported file name." << endl;
        return;
    }
    int depth = 10;
    for(int max_depth = 0; max_depth <= depth; max_depth++)
    {
        double average_training_accuracy = 0.0;
        double average_test_accuracy = 0.0;
        int node_count = 0;
        int depth_count = 0;
        get_accuracy(data, attributes, max_depth, selection_strategy, average_training_accuracy, average_test_accuracy, attribute_value_types, node_count, depth_count);
        csv_file << max_depth << "," << average_training_accuracy << "," << average_test_accuracy << "\n";
        csv_file.flush(); // Ensure data is written to the file immediately

        if(max_depth == 0)
        {
            node_depth_csv_file << file_name << "," << selection_strategy << "," << node_count << "," << depth_count << "\n"; // For max depth 0, we assume no nodes and depth
            node_depth_csv_file.flush(); // Ensure data is written to the file immediately
        }
    }
    csv_file.close();
    cout << "CSV file generated: " << csv_file_name << endl;
}



int main()
{
    node_depth_csv_file << "File Name,Selection Strategy,Node Count,Depth Count\n";
    node_depth_csv_file.flush(); // Ensure header is written to the file immediately
    generate_csv("ig", "iris");
    generate_csv("igr", "iris");
    generate_csv("nwig", "iris");
    generate_csv("ig", "adult");
    generate_csv("igr", "adult");
    generate_csv("nwig", "adult");
    node_depth_csv_file.close();

    return 0;
}