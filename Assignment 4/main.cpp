#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include "decision_tree.hpp"
#include <algorithm>
#include <random>

using namespace std;

const string IRIS_FILE = "Iris.csv";
const string ADULT_FILE = "adult.data";

string trim(const string & str)
{
    int first = 0;
    while(first < str.size() && isspace(str[first])) first++;
    int last =str.size() - 1;
    while(last >= first && isspace(str[last])) last--;
    if(first > last) return ""; // If the string is empty after trimming
    return str.substr(first, last - first + 1);
}

void read_adult_file(vector<DataPoint> & data, vector<string> & attributes, string & class_label_name)
{
    ifstream input_file(ADULT_FILE);
    string line, word;
    for(int i = 0; i < 14; i++) // Read the first line for attributes
    {
        attributes.push_back("Attribute" + to_string(i + 1));
    }
    class_label_name = "Class";
    int attribute_size = attributes.size();
    while(getline(input_file, line))
    {
        if(line.empty()) continue;
        stringstream ss(line);
        DataPoint point;
        int index = 0;
        while(getline(ss, word, ',') && index < attribute_size)
        {
            word = trim(word);
            point.attribute_values[attributes[index]] = word;
            index++;
        }
        getline(ss, word, ','); // Read the class label
        point.class_label = trim(word);

        data.push_back(point);
    }
    input_file.close();
}


void read_iris_file(vector<DataPoint> & data, vector<string> & attributes, string & class_label_name)
{
    ifstream input_file(IRIS_FILE);
    string line, word;
    getline(input_file, line); // Read the first line for attributes
    if(!line.empty())
    {
        stringstream ss(line);
        getline(ss, word, ','); // Skip the first column (ID)
        while(getline(ss, word, ','))
        {
            word = trim(word);
            attributes.push_back(word);
        }
        class_label_name = attributes.back();
        attributes.pop_back();
    }
    int attribute_size = attributes.size();
    while(getline(input_file, line))
    {
        if(line.empty()) continue;
        stringstream ss(line);
        // Skip the first column (ID)
        getline(ss, word, ','); // Skip the first column (ID)
        DataPoint point;
        int index = 0;
        while(getline(ss, word, ',') && index < attribute_size)
        {
            word = trim(word);
            point.attribute_values[attributes[index]] = word;
            index++;
        }
        getline(ss, word, ','); // Read the class label
        point.class_label = trim(word);

        data.push_back(point);
    }
    input_file.close();
}

void print_data(const vector<DataPoint> & data, const vector<string> & attributes)
{
    for(const auto & attr : attributes)
    {
        cout << attr << " ";
    }
    cout << endl;

    for(const auto & point : data)
    {
        for(const auto & attr : attributes)
        {
            cout << point.attribute_values.at(attr) << " ";
        }
        cout << point.class_label << endl;
    }
}

void split_data(const vector<DataPoint> & data, vector<DataPoint> & training_data, vector<DataPoint> & test_data)
{
    vector<DataPoint> shuffled_data = data;
    std::shuffle(shuffled_data.begin(), shuffled_data.end(), std::default_random_engine(std::random_device{}()));
    size_t train_size = static_cast<size_t>(shuffled_data.size() * 0.8);

    training_data.assign(shuffled_data.begin(), shuffled_data.begin() + train_size);
    test_data.assign(shuffled_data.begin() + train_size, shuffled_data.end());
}


int main(int argc, char *argv[])
{
    int max_depth = 5;
    string selection_strategy = "ig";
    if(argc == 3)
    {
        max_depth = stoi(argv[1]);
        selection_strategy = argv[2];
    }
    vector<DataPoint> data;
    vector<string> attributes;
    string class_label_name;

    // Read the Iris dataset
    // read_iris_file(data, attributes, class_label_name);
    read_adult_file(data, attributes, class_label_name);

    DecisionTree tree(max_depth, selection_strategy);

    vector<DataPoint> training_data, test_data;
    split_data(data, training_data, test_data);

    // cout << training_data.size() << " training data points." << endl;
    // cout << test_data.size() << " test data points." << endl;

    tree.train(training_data, attributes);
    tree.print();

    int correct_predictions_with_training_data = 0;
    for(const auto & point : training_data)
    {
        string predicted_class = tree.predict(point);
        if(predicted_class == point.class_label)
        {
            correct_predictions_with_training_data++;   
        }
    }

    double training_accuracy = static_cast<double>(correct_predictions_with_training_data) / training_data.size() * 100.0;
    cout << "Training accuracy: " << training_accuracy << "%" << endl;
    int correct_predictions_with_test_data = 0;
    for(const auto & point : test_data)
    {
        string predicted_class = tree.predict(point);
        if(predicted_class == point.class_label)
        {
            correct_predictions_with_test_data++;
        }
    }
    double test_accuracy = static_cast<double>(correct_predictions_with_test_data) / test_data.size() * 100.0;
    cout << "Test accuracy: " << test_accuracy << "%" << endl;

    ofstream output_file("Iris_results.csv", ios::app);
    if(output_file.is_open())
    {
        output_file << max_depth << ", " << selection_strategy << ", "
                    << training_accuracy << ", " << test_accuracy << endl;
        output_file.close();   
    }
    else
    {
        cerr << "Error: Unable to open output file." << endl;
    }
    return 0;
}