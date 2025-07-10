#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <random>
#include <unordered_map>
#include "decision_tree.hpp"

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

void read_adult_file(vector<DataPoint> & data, vector<string> & attributes, string & class_label_name, unordered_map<string, AttributeType> & attribute_value_types)
{
    ifstream input_file(ADULT_FILE);
    string line, word;
    for(int i = 0; i < 14; i++) // Read the first line for attributes
    {
        attributes.push_back("Attribute" + to_string(i + 1));
    }
    class_label_name = "Class";
    int attribute_size = attributes.size();
    attribute_value_types["Attribute1"] = NUMERICAL;
    attribute_value_types["Attribute2"] = CATEGORICAL;
    attribute_value_types["Attribute3"] = NUMERICAL;
    attribute_value_types["Attribute4"] = CATEGORICAL;
    attribute_value_types["Attribute5"] = NUMERICAL;
    attribute_value_types["Attribute6"] = CATEGORICAL;
    attribute_value_types["Attribute7"] = CATEGORICAL;
    attribute_value_types["Attribute8"] = CATEGORICAL;
    attribute_value_types["Attribute9"] = CATEGORICAL;
    attribute_value_types["Attribute10"] = CATEGORICAL;
    attribute_value_types["Attribute11"] = NUMERICAL;
    attribute_value_types["Attribute12"] = NUMERICAL;
    attribute_value_types["Attribute13"] = NUMERICAL;
    attribute_value_types["Attribute14"] = CATEGORICAL;
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


void read_iris_file(vector<DataPoint> & data, vector<string> & attributes, string & class_label_name, unordered_map<string, AttributeType> & attribute_value_types)
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
    for(int i = 0; i < attribute_size; i++)
    {
        attribute_value_types[attributes[i]] = NUMERICAL; // Assuming all attributes are of type string for simplicity
    }
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