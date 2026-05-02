#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;

int rlf_graph_coloring(int n, const vector<vector<bool>>& adj, vector<int>& colors) {
    int color = 1;                          
    vector<bool> U(n, true);           

    for (int v = 0; v < n; v++) {
        colors[v] = 0;
    }

    while (true) {
        bool U_empty = true;
        for (int i = 0; i < n; i++) {
            if (U[i]) { U_empty = false; break; }
        }
        if (U_empty) break;

        int v = -1;
        int max_deg = -1;
        for (int i = 0; i < n; i++) {
            if (U[i]) {
                int deg = 0;
                for (int j = 0; j < n; j++) {
                    if (U[j] && adj[i][j]) deg++;
                }
                if (deg > max_deg) {
                    max_deg = deg;
                    v = i;
                }
            }
        }

        vector<int> S;
        S.push_back(v);

        vector<int> A;
        for (int i = 0; i < n; i++) {
            if (U[i] && adj[v][i]) {
                A.push_back(i);
            }
        }


        vector<int> W;
        for (int i = 0; i < n; i++) {
            if (!U[i]) continue;
            bool in_S = (i == v);
            bool in_A = false;
            for (int a : A) {
                if (a == i) { in_A = true; break; }
            }
            if (!in_S && !in_A) {
                W.push_back(i);
            }
        }


        while (!W.empty()) {
            vector<int> d_A(W.size(), 0);
            int max_dA = -1;
            for (int idx = 0; idx < (int)W.size(); idx++) {
                int w = W[idx];
                int count = 0;
                for (int a : A) {
                    if (adj[w][a]) count++;
                }
                d_A[idx] = count;
                if (count > max_dA) max_dA = count;
            }


            int w_star = -1;
            int w_star_idx = -1;
            vector<int> candidates;
            for (int idx = 0; idx < (int)W.size(); idx++) {
                if (d_A[idx] == max_dA) {
                    candidates.push_back(idx);
                }
            }

            if (candidates.size() > 1) {
                int best_idx = -1;
                int best_deg = -1;
                for (int idx : candidates) {
                    int w = W[idx];
                    int deg = 0;
                    for (int j = 0; j < n; j++) {
                        if (U[j] && adj[w][j]) deg++;
                    }
                    if (deg > best_deg) {
                        best_deg = deg;
                        best_idx = idx;
                    }
                }
                w_star_idx = best_idx;
                w_star = W[best_idx];
            }
            else {
                w_star_idx = candidates[0];
                w_star = W[candidates[0]];
            }

            S.push_back(w_star);

            for (int x : W) {
                if (adj[w_star][x]) {
                    bool already_in_A = false;
                    for (int a : A) {
                        if (a == x) { already_in_A = true; break; }
                    }
                    if (!already_in_A) {
                        A.push_back(x);
                    }
                }
            }

            vector<int> new_W;
            for (int x : W) {
                if (x == w_star) continue;  
                if (adj[w_star][x]) continue; 
                new_W.push_back(x);
            }
            W = new_W;
        }

        for (int s : S) {
            colors[s] = color;
        }

        for (int s : S) {
            U[s] = false;
        }

        color++;
    }

    return color - 1;
}


vector<vector<bool>> generate_random_graph(int n, int m) {
    vector<vector<bool>> adj(n, vector<bool>(n, false));

    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> vert(0, n - 1);

    int edges_added = 0;
    while (edges_added < m) {
        int u = vert(gen);
        int v = vert(gen);
        if (u != v && !adj[u][v]) {
            adj[u][v] = adj[v][u] = true;
            edges_added++;
        }
    }

    return adj;
}


int main() {
    setlocale(LC_ALL, "Russian");

    int n_fixed = 200;
    vector<double> densities = { 0.05, 0.1, 0.15, 0.2, 0.25, 0.3, 0.35, 0.4 };

    for (double density : densities) {
        int m = (int)(density * n_fixed * (n_fixed - 1) / 2);

        auto adj = generate_random_graph(n_fixed, m);
        vector<int> colors(n_fixed);

        auto start = chrono::high_resolution_clock::now();
        int num_colors = rlf_graph_coloring(n_fixed, adj, colors);
        auto end = chrono::high_resolution_clock::now();

        double time_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        cout << density << "\t\t" << m << "\t" << num_colors << "\t"
             << "\t\t" << time_ms << "\n";
    }


    double fixed_density = 0.1;
    vector<int> vertices_sizes = { 50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800, 850, 900, 950, 1000 };

    for (int n = 10; n <= 1000;n++) {
        int m = (int)(fixed_density * n * (n - 1) / 2);

        auto adj = generate_random_graph(n, m);
        vector<int> colors(n);

        auto start = chrono::high_resolution_clock::now();
        int num_colors = rlf_graph_coloring(n, adj, colors);
        auto end = chrono::high_resolution_clock::now();

        double time_ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();

        double ratio = time_ms / (num_colors * n * n);

        cout << n << "\t" << m << "\t" << num_colors << "\t"
            << "\t\t" << time_ms << "\t" << ratio << "\n";
    }
}
