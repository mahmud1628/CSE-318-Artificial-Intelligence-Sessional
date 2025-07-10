#include "decision_tree.hpp"
#include "data_loader.hpp"

using namespace std;

unordered_map<string, AttributeType> attribute_value_types;

int main(int argc, char *argv[])
{
    int max_depth = 5;
    string selection_strategy = "ig";
    if(argc == 3)
    {
        max_depth = stoi(argv[2]);
        selection_strategy = argv[1];
    }

    vector<DataPoint> data;
    vector<string> attributes;
    string class_label_name;

    const string file_name = "iris"; // Change this to "adult" for the adult dataset

    if(file_name == "iris")
    {
        read_iris_file(data, attributes, class_label_name, attribute_value_types);
    }
    else if(file_name == "adult")
    {
        read_adult_file(data, attributes, class_label_name, attribute_value_types);
    }

    DecisionTree tree(max_depth, selection_strategy);

    vector<DataPoint> training_data, test_data;
    split_data(data, training_data, test_data);

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

    return 0;
}