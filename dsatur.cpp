#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <set>
#include <algorithm>
#include <thread> 
#include <chrono>

using namespace std;

const int INF = 1e9;

vector<int> GreedySequental(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> colors(n, -1);

    for (int vertex = 0; vertex < n; vertex++) {
        vector<bool> available(n, 0);
        for (int j : graph[vertex]) {
            if (colors[j] != -1)
                available[colors[j]] = 1;
        }

        int min_color = 0;
        while (available[min_color] == 1)
            min_color++;

        colors[vertex] = min_color;
    }
    return colors;
}

vector<int> dsatur(vector<vector<int>>& graph) {
    int n = graph.size();
    vector<int> colors(n, -1);

    int vert = 0, mx = graph[0].size();
    for (int i = 0; i < n; i++)
        if (mx < graph[i].size()) {
            mx = graph[i].size();
            vert = i;
        }

    vector<int> pointers(n), cnt(n);
    vector<vector<bool>> saturation(n, vector<bool>(n));
    int colored = 0;
    while (colored < n) {

        vector<bool> available(n, 0);
        for (int i : graph[vert]) {
            if (colors[i] != -1)
                available[colors[i]] = 1;
        }

        int min_color = 0;
        while (available[min_color] == 1)
            min_color++;

        colors[vert] = min_color;
        colored++;

        for (int i : graph[vert]) {
            if (saturation[i][min_color] == 0)
                cnt[i]++;
            saturation[i][min_color] = 1;
        }

        int mx = -1, next_vertex = -1, deg = -1;
        for (int i = 0; i < n; i++) {
            if (colors[i] == -1 && (mx < cnt[i] ||
                mx == cnt[i] && (int)graph[i].size() > deg)) {
                mx = cnt[i];
                next_vertex = i;
                deg = graph[i].size();
            }
        }

        vert = next_vertex;

    }
    return colors;
}

// Функция для подсчета количества использованных цветов
int countColors(vector<int>& colors) {
    return *max_element(colors.begin(), colors.end()) + 1;
}

vector<vector<int>> graph;
vector<vector<bool>> matrix;

// Функция для генерации случайного графа с заданным количеством ребер
void generateRandomGraph(int n, int m) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> vert_dist(0, n - 1);

    int edges_added = 0;
    while (edges_added < m) {
        int u = vert_dist(gen);
        int vert = vert_dist(gen);

        if (u != vert && !matrix[u][vert]) {
            graph[u].push_back(vert);
            graph[vert].push_back(u);
            matrix[u][vert] = true;
            matrix[vert][u] = true;
            edges_added++;
        }
    }

}

vector<vector<int>> rationalization(int n) {

    vector<bool> banned(n);

    for (int i = 0; i < n; i++) {
        for (int j = i + 1; j < n; j++) {
            if (matrix[i][j] == 0) {
                bool absort = 1;
                // i поглащает j
                for (int k = 0; k < n; k++)
                    if (matrix[i][k] < matrix[j][k])
                        absort = 0;

                if (absort) {
                    banned[j] = 1;
                    continue;
                }

                // j поглащает i
                absort = 1;

                for (int k = 0; k < n; k++)
                    if (matrix[j][k] < matrix[i][k])
                        absort = 0;

                if (absort) {
                    banned[i] = 1;
                    continue;
                }
            }
        }
    }

    vector<vector<int>> edges(n);

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            if (banned[i] == 0 && banned[j] == 0 && matrix[i][j] == 1) {
                edges[i].push_back(j);
                edges[j].push_back(i);
            }
        }

    return edges;
}


#include <fstream>

int main() {

    ofstream outfile("experiment_results.csv");

    outfile << "m,Greedy_Colors,DSATUR_Colors,Greedy_Time_ms,DSATUR_Time_ms" << endl;


    cout << "m\tGreedy\tDSATUR" << endl;
    cout << "---------------------" << endl;

    int n = 500;


    for (int m = 1000; m <= n*(n-1)/2; m += 1000) {
        double greedy_time = 0, dsatur_time = 0;
        double greedy_time_rat = 0, dsatur_time_rat = 0;
        double greedy_count = 0, dsatur_count = 0;
        double greedy_count_rat = 0, dsatur_count_rat = 0;
        

        for (int i = 0; i < 20; i++) {
            graph.clear(); matrix.clear();
            graph.resize(n);
            matrix.resize(n);
            for (int i = 0; i < n; i++)
                matrix[i].resize(n);
            generateRandomGraph(n, m);

            auto start = chrono::high_resolution_clock::now();
            auto greedy_colors = GreedySequental(graph);
            auto end = chrono::high_resolution_clock::now();
            greedy_time += chrono::duration_cast<chrono::milliseconds>(end - start).count();
            greedy_count += countColors(greedy_colors);

            start = chrono::high_resolution_clock::now();
            auto dsatur_colors = dsatur(graph);
            end = chrono::high_resolution_clock::now();
            dsatur_time += chrono::duration_cast<chrono::milliseconds>(end - start).count();
            dsatur_count += countColors(dsatur_colors);
        }

        outfile << m << "," << greedy_count / 20 << "," << dsatur_count / 20 << ","
            << greedy_time / 20 << "," << dsatur_time / 20 << endl;

        cout << m << "\t" << greedy_count / 20 << "\t" << dsatur_count / 20;
        cout << " (greedy: " << greedy_time / 20  << "ms, dsatur: " << dsatur_time / 20 << "ms)" << endl;
        //std::this_thread::sleep_for(std::chrono::seconds(5));
    }
    

    outfile.close();
    cout << "Результаты сохранены в experiment_results.csv" << endl;


    return 0;
}