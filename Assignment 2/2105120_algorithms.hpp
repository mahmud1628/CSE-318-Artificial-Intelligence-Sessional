#include <vector>
#include <cstdlib>
#include <ctime>
#include <unordered_set>
#include <limits>
#include <algorithm>
#include <random>

using namespace std;

int calculateCutWeight(const unordered_set<int> & partition_x, const unordered_set<int> & partition_y, const vector<vector<int>> & weights) {
    int cutWeight = 0;

    for (auto u : partition_x) {
        for (auto v : partition_y) {
            cutWeight += weights[u-1][v-1];
        }
    }

    return cutWeight;
}


double RandomizedHeuristicMaxCut(int n, const vector<pair<pair<int,int>, int>> & edges, int iterations) {
    double totalCutWeight = 0; // Total weight of the cut

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, n);


    for (int i= 0; i < iterations ; i++ ) {
        unordered_set<int> x, y;

        for (int j = 1; j <= n; j++) {
            // Randomly assign vertices to partition x or y
            int random_partition = dis(gen) % 2;
            if(random_partition == 0) x.insert(j);
            else y.insert(j);
        }

        double cut_weight = 0; // Weight of the current cut

        for (auto & edge : edges) {
            int u = edge.first.first;
            int v = edge.first.second;
            int weight = edge.second;

            bool u_in_x = x.find(u) != x.end();
            bool v_in_x = x.find(v) != x.end();
            bool u_in_y = y.find(u) != y.end();
            bool v_in_y = y.find(v) != y.end();
            bool is_cut = u_in_x && v_in_y || u_in_y && v_in_x; // Check if the edge is cut

            if (is_cut) cut_weight += weight;
        }
        totalCutWeight = totalCutWeight + cut_weight;
    }

    return totalCutWeight / iterations; // Return the average cut weight
}


pair<unordered_set<int>, unordered_set<int>> GreedyMaxCut(int n, const vector<vector<int>> & weights) {
    unordered_set<int> partition_x, partition_y;

    // get the maximum weight of the edges
    int max_weight = 0, max_u = -1, max_v = -1;
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= n; j++) {
            if (weights[i-1][j-1] > max_weight) {
                max_weight = weights[i-1][j-1];
                max_u = i;
                max_v = j;
            }
        }
    }

    partition_x.insert(max_u);
    partition_y.insert(max_v);


    for (int i = 1; i <= n; i++) {
        if(i == max_u || i == max_v) continue; // Skip the vertices already in the partitions
        int weight_x = 0, weight_y = 0;

        for(auto u : partition_y) weight_x += weights[i-1][u-1];
        for(auto v : partition_x) weight_y += weights[i-1][v-1];

        if(weight_x > weight_y) partition_x.insert(i);
        else partition_y.insert(i);
    }

    return {partition_x, partition_y};
}



pair<unordered_set<int>, unordered_set<int>> SemiGreedyMaxCut(int n, const vector<vector<int>> & weights, double alpha) {
    unordered_set<int> partition_x, partition_y;
    vector<bool> assigned(n + 1, false); // Track assigned vertices, 1 indexed

    // get the maximum weight of the edges
    int max_weight = 0, max_u = -1, max_v = -1;
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= n; j++) {
            if (weights[i-1][j-1] > max_weight) {
                max_weight = weights[i-1][j-1];
                max_u = i;
                max_v = j;
            }
        }
    }

    partition_x.insert(max_u);
    partition_y.insert(max_v);
    assigned[max_u] = true;
    assigned[max_v] = true;

    int remaining_vertices = n - 2; // Remaining vertices to assign

    random_device rd;
    mt19937 gen(rd());

    while(remaining_vertices > 0) {
        vector<int> candidates;
        vector<int> greedy_values;

        int wmin = numeric_limits<int>::max();
        int wmax = numeric_limits<int>::min();

        for (int v = 1; v <= n; v++) {
            if(assigned[v]) continue; // Skip already assigned vertices

            int sigma_x = 0, sigma_y = 0;

            for (auto u : partition_x) sigma_x += weights[v-1][u-1];
            for (auto u : partition_y) sigma_y += weights[v-1][u-1];

            int greedy_value = max(sigma_x, sigma_y);
            greedy_values.push_back(greedy_value);
            candidates.push_back(v);
            wmin = min(wmin, min(sigma_x, sigma_y));
            wmax = max(wmax, max(sigma_x, sigma_y));
        }

        double mu = wmin + alpha * (wmax - wmin);

        vector<int> RCL; // Restricted Candidate List

        for (int i =0 ; i < candidates.size(); i++) {
            if(greedy_values[i] >= mu) RCL.push_back(candidates[i]);
        }

        if(RCL.empty()) {
            int best_index = max_element(greedy_values.begin(), greedy_values.end()) - greedy_values.begin();
            RCL.push_back(candidates[best_index]);
        }

        // Randomly select a vertex from RCL

        uniform_int_distribution<> dis(0, RCL.size() - 1);
        int random_index = dis(gen);
        int selected_vertex = RCL[random_index];
        assigned[selected_vertex] = true;
        remaining_vertices--;
        int weight_x = 0, weight_y = 0;

        for (auto u : partition_x) weight_x += weights[selected_vertex-1][u-1];
        for (auto u : partition_y) weight_y += weights[selected_vertex-1][u-1];

        if (weight_x > weight_y) partition_x.insert(selected_vertex);
        else partition_y.insert(selected_vertex);

    }

    return {partition_x, partition_y};
}



void LocalSearchMaxCut(int n, const vector<vector<int>> & weights, unordered_set<int> & partition_x, unordered_set<int> & partition_y) {
    bool improved = true;

    while(improved) {
        improved = false;

        int delta_max = 0;
        int best_vertex = -1;
        bool best_partition = false; // true for x, false for y

        for (int v = 1; v <=n ; v++) {
            bool vertex_in_x = partition_x.find(v) != partition_x.end();
            
            int sigma_same = 0, sigma_other = 0;

            if(vertex_in_x) {
                for (auto u : partition_x) sigma_same += weights[v-1][u-1];
                for (auto u : partition_y) sigma_other += weights[v-1][u-1];
            } else {
                for (auto u : partition_y) sigma_same += weights[v-1][u-1];
                for (auto u : partition_x) sigma_other += weights[v-1][u-1];
            }

            int delta = sigma_other - sigma_same;

            if ( delta > delta_max) {
                delta_max = delta;
                best_vertex = v;
                best_partition = !vertex_in_x; // Swap the partition
            }
            
        }

        if(best_vertex != -1) {
            if (best_partition) {
                partition_x.insert(best_vertex);
                partition_y.erase(best_vertex);
            } else {
                partition_y.insert(best_vertex);
                partition_x.erase(best_vertex);
            }
            improved = true; // Improvement found
        } else {
            improved = false; // No improvement found
        }
    }
}


pair<unordered_set<int>, unordered_set<int>> GRASP(int n, const vector<vector<int>> & weights, int iterations, double alpha) {
    pair<unordered_set<int>, unordered_set<int>> best_partition = SemiGreedyMaxCut(n, weights, alpha);
    LocalSearchMaxCut(n, weights, best_partition.first, best_partition.second);
    int best_cut_weight = calculateCutWeight(best_partition.first, best_partition.second, weights);

    for(int i = 1 ; i < iterations; i++) {
        auto partition = SemiGreedyMaxCut(n, weights, alpha);
        LocalSearchMaxCut(n, weights, partition.first, partition.second);
        int cut_weight = calculateCutWeight(partition.first, partition.second, weights);

        if (cut_weight > best_cut_weight) {
            best_cut_weight = cut_weight;
            best_partition = partition;
        }
    }
    // Return the best partition found
    return best_partition;
}