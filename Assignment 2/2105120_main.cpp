#include<iostream>
#include "2105120_algorithms.hpp"

using namespace std;


int main() {
    freopen("set1/g1.rud", "r", stdin);
    freopen("output.txt", "w", stdout);

    int n, m;

    cin >> n >> m;

    vector<vector<int>> weights(n, vector<int>(n, 0));

    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        weights[u-1][v-1] = w;
        weights[v-1][u-1] = w;
    }

    vector<pair<pair<int,int>, int>> edges;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i < j && weights[i][j] != 0) {
                edges.push_back({{i+1, j+1}, weights[i][j]});
            }
        }
    }

    // int average_cut_weight = RandomizedHeuristicMaxCut(n, edges, n);
    // pair<unordered_set<int>, unordered_set<int>> partition = SemiGreedyMaxCut(n, weights,0.5);

    // int cut_weight = calculateCutWeight(partition.first, partition.second, weights);
    // cout << "Greedy Max Cut: " << cut_weight << endl;
    // LocalSearchMaxCut(n, weights, partition.first, partition.second);
    // cut_weight = calculateCutWeight(partition.first, partition.second, weights);
    // cout << "Cut weight: " << cut_weight << endl;

    pair<unordered_set<int>, unordered_set<int>> partition = GRASP(n, weights, 100, 0.5);
    int cut_weight = calculateCutWeight(partition.first, partition.second, weights);
    cout << "GRASP Max Cut: " << cut_weight << endl;
}