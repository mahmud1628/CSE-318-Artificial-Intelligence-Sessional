#include <iostream>
#include <filesystem>
#include <fstream>
#include "2105120_algorithms.hpp"

using namespace std;
namespace fs = filesystem;


vector<int> get_known_best_solutions() {
    vector<int> known_best_solutions(54,0);
    known_best_solutions[0] = 12078;
    known_best_solutions[1] = 12084;
    known_best_solutions[2] = 12077;
    known_best_solutions[10] = 627;
    known_best_solutions[11] = 621;
    known_best_solutions[12] = 645;
    known_best_solutions[13] = 3187;
    known_best_solutions[14] = 3169;
    known_best_solutions[15] = 3172;
    known_best_solutions[21] = 14123;
    known_best_solutions[22] = 14129;
    known_best_solutions[23] = 14131;
    known_best_solutions[31] = 1560;
    known_best_solutions[32] = 1537;
    known_best_solutions[33] = 1541;
    known_best_solutions[34] = 8000;
    known_best_solutions[35] = 7996;
    known_best_solutions[36] = 8009;
    known_best_solutions[42] = 7027;
    known_best_solutions[43] = 7022;
    known_best_solutions[44] = 7020;
    known_best_solutions[47] = 6000;
    known_best_solutions[48] = 6000;
    known_best_solutions[49] = 5988;

    return known_best_solutions;
}


void write_CSV_header(ofstream & csv, double alpha) {
    // First header row (category headers)
    csv << ",Problem,,,Constructive Algorithm,,Local Search,,GRASP,,Known Best Solution or Upper Bound" << endl;

    // Second header row (column names)
    csv << "Name,|V| or n,|E| or m,"
            << "Simple randomized,Simple greedy,Semi greedy(Alpha = " << alpha << "),"
            << "No. of iterations,Average value,"
            << "No. of iterations,Best value,"
            << "" << endl;

    csv.flush();
    cout << "Header format written to CSV file" << endl;
}

int main(int argc, char *argv[]) {
    vector<int> known_best_solutions = get_known_best_solutions();

    ofstream csv_file;
    string input_dir = "set1/";  // Default directory
    string output_file = "results.csv";  // Default output file

    if(argc == 3) {
        input_dir = argv[1];
        output_file = argv[2];
    }

    // Open CSV file for writing
    csv_file.open(output_file);

    double alpha = 0.5; // Default alpha value

    write_CSV_header(csv_file, alpha);


    vector<fs::directory_entry> files;
    for (const auto& entry : fs::directory_iterator(input_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".rud") {
            files.push_back(entry);
        }
    }

    sort(files.begin(), files.end(), [](const fs::directory_entry &a, const fs::directory_entry &b) {
        return stoi(a.path().stem().string().substr(1)) < stoi(b.path().stem().string().substr(1));
    });



    for (const auto &entry : files) {
            string input_path = entry.path().string();
            string filename = entry.path().stem().string(); // e.g., "g1"

            // Redirect input and output
            ifstream in(input_path);
            cin.rdbuf(in.rdbuf());

            int n, m;
            cin >> n >> m;
            vector<vector<int>> weights(n, vector<int>(n, 0));
            for (int i = 0; i < m; i++) {
                int u, v, w;
                cin >> u >> v >> w;
                weights[u - 1][v - 1] = w;
                weights[v - 1][u - 1] = w;
            }

            vector<pair<pair<int,int>, int>> edges;
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    if (i < j && weights[i][j] != 0) {
                        edges.push_back({{i + 1, j + 1}, weights[i][j]});
                    }
                }
            }

            // Run the algorithms

            cout << "Processing file: " << filename << endl;

            int randomized_average_cut_weight = RandomizedHeuristicMaxCut(n, edges);
            auto greedy_partition = GreedyMaxCut(n, weights);
            int greedy_cut_weight = calculateCutWeight(greedy_partition.first, greedy_partition.second, weights);
            auto semi_greedy_partition = SemiGreedyMaxCut(n, weights, alpha);
            int semi_greedy_cut_weight = calculateCutWeight(semi_greedy_partition.first, semi_greedy_partition.second, weights);
            int local_search_iterations = LocalSearchMaxCut(n, weights, semi_greedy_partition.first, semi_greedy_partition.second);
            int local_search_cut_weight = calculateCutWeight(semi_greedy_partition.first, semi_greedy_partition.second, weights);

            int grasp_iterations= 50;
            if(n > 1000 && m > 10000) {
                grasp_iterations = 20;
            }

            auto grasp_partition = GRASP(n, weights, grasp_iterations, alpha);
            int grasp_cut_weight = calculateCutWeight(grasp_partition.first, grasp_partition.second, weights);

            int known_best_solution_index = stoi(filename.substr(1)) - 1;

            string known_best_solution = "N/A";
            if(known_best_solutions[known_best_solution_index] != 0) {
                known_best_solution = to_string(known_best_solutions[known_best_solution_index]);
            }
            // Write results to CSV

            csv_file << filename << ","
                     << n << "," << m << ","
                     << randomized_average_cut_weight << ","
                     << greedy_cut_weight << ","
                     << semi_greedy_cut_weight << ","
                     << local_search_iterations << "," << local_search_cut_weight << ","
                     << grasp_iterations << "," << grasp_cut_weight << ","
                     << known_best_solution
                     << endl;
            cout << "Processed file: " << filename << endl;
            in.close();
            csv_file.flush();
    }

    csv_file.close();
    cout << "Results written to " << output_file << endl;

    return 0;
}
