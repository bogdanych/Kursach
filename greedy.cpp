#include <iostream>
#include <vector>
#include <random>

using namespace std;

vector<int> GreedySequental(vector<vector<int>>& graph) {
	int n = graph.size(); // число вершин
	vector<int> colors(n, -1); // массив цветов 

	for (int vertex = 0; vertex < n; vertex++) {

		vector<bool> available(n, 0); // массив допустимых значений цветов
		for (int j : graph[vertex]) { //рассматриваем смежные вершины
			if (colors[j] != -1) // если покрашена
				available[colors[j]] = 1; // отмечаем цвет как занятый
		}

		int min_color = 0;
		while (available[min_color] == 1) //находим минимальный не занятый цвет
			min_color++;

		colors[vertex] = min_color; // присваиваем его текущей вершине

	}
	return colors;
}

int n, m;
vector<vector<int>> graph;
vector<vector<bool>> matrix;

void generateRandomGraph(int n, int m) {
	

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<int> vert_dist(0, n - 1);

	int edges_added = 0;
	while (edges_added < m) {
		int u = vert_dist(gen);
		int vert = vert_dist(gen);

		if (u != vert && !matrix[u][vert]) {
			cout << u << ' ' << vert << '\n';
			graph[u].push_back(vert);
			graph[vert].push_back(u);
			matrix[u][vert] = true;
			matrix[vert][u] = true;
			edges_added++;
		}
	}
}


vector<vector<int>> rationalization() {

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
			if (!banned[i] && !banned[j]) {
				edges[i].push_back(j);
				edges[j].push_back(i);
			}
		}

	return edges;
}

int main()
{
    cin >> n >> m;

	graph.resize(n);
	matrix.resize(n);
	for (int i = 0; i < n; i++)
		matrix[i].resize(n);

	generateRandomGraph(n, m);

	vector<vector<int>> e = rationalization();

	cout << '\n';

	for (int i=0;i<n;i++)
		for (int j = 0; j < e[i].size(); j++) {
			cout << i << ' ' << e[i][j] << '\n';
		}

	//vector<int> ans = GreedySequental(e);

	//for (int i = 0; i < n; i++)
	//	cout << ans[i] << ' ';
}
