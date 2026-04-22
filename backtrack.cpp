#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <set>
#include <chrono>

using namespace std;

int n, m;
int best;

const int INF = 1e9;

vector<int> bfs(int v, vector<vector<int>>& graph, vector<int>& colors, vector<set<int>>& sat) {
	queue<int> q;
	q.push(v);
	while (!q.empty()) { // обход в ширину
		int v = q.front();
		q.pop();
		if (colors[v] != -1)
			continue;

		vector<bool> available(n, 0);
		for (int i : graph[v]) {
			if (colors[i] != -1)
				available[colors[i]] = 1;
		}

		int min_color = 0;
		while (available[min_color] == 1)
			min_color++;

		colors[v] = min_color;

		for (int i : graph[v]) {
			if (colors[i] != -1)
				sat[i].insert(min_color); // считаем степень насыщения
		}

		int mx = -1, next_vertex = -1, deg = -1;
		// процесс выбора следующей вершины по двум критериям:
		// 1) степень насыщения
		// 2) степень вершины
		for (int i = 0; i < n; i++) {
			if (colors[i] == -1 && (mx < (int)sat[i].size() ||
				mx == (int)sat[i].size() && (int)graph[i].size() > deg)) {
				mx = sat[i].size();
				next_vertex = i;
				deg = graph[i].size();
			}
		}

		if (next_vertex != -1)
			q.push(next_vertex); // выбираем следующую вершину для обхода в ширину

	}
	return colors;
}

vector<int> dsatur(vector<vector<int>>& graph) {
	int n = graph.size(); // число вершин
	vector<pair<int, int>> deg(n); // массив пар (степень вершины, вершина)

	for (int i = 0; i < n; i++) {
		deg[i].first = graph[i].size();
		deg[i].second = i;
	}

	sort(deg.rbegin(), deg.rend()); // сортируем по невозрастанию степеней вершин

	vector<int> colors(n, -1); // цвета вершин
	vector<set<int>> sat(n); // степень насыщения
	int vert = 0;
	while (vert < n) { // рассматриваем кажду компоненту связности

		bfs(vert, graph, colors, sat);

		while (vert < n && colors[deg[vert].second] != -1)
			vert++;
	}
	return colors;
}


int vertex_backtrack(int v, vector<vector<int>>& edges, vector<int> solution, int large_color) {
	if (large_color >= best - 1)
		return INF;

	if (v == n) { // просмотрели все вершины
		int cnt = 0;
		vector<int> cnt_col(n);
		for (int i = 0; i < n; i++) {
			cnt_col[solution[i]]++;
			if (cnt_col[solution[i]] == 1)
				cnt++;
		}
		best = min(best, cnt);
		return cnt; // возвращаем количество цветов
	}

	int min_ans = n;

	if (v == 0) { // отдельно обрабатываем первую вершину
		solution[v] = 0;
		min_ans = vertex_backtrack(v + 1, edges, solution, 0);
	}
	else {
		for (int color = 0; color < best - 1; color++) {
			solution[v] = color;
			// проверяем - можно ли использовать текущий цвет так,
			// чтобы не было конфликта с уже расскрашенными вершинами
			bool correct = 1;
			for (int i = 0; i < edges[v].size(); i++) {
				if (solution[edges[v][i]] == color)
					correct = 0; // цвет не подходит
			}
			if (correct) {
				int cur_ans = vertex_backtrack(v + 1, edges, solution, max(color, large_color)); // запускаем рекурсию
				min_ans = min(min_ans, cur_ans); // из всех ответов ищем минимальный
			}
		}
	}
	return min_ans;
}

vector<vector<int>> rationalization(int n, vector<vector<int>>& edges, vector<vector<int>>& matrix) {

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

	vector<vector<int>> edg(n);

	for (int i = 0; i < n; i++)
		for (int j = i + 1; j < n; j++) {
			if (banned[i] == 0 && banned[j] == 0 && matrix[i][j] == 1) {
				edg[i].push_back(j);
				edg[j].push_back(i);
			}
		}

	return edg;
}


int main() {
	cin >> n;
	vector<vector<int>> edges(n);

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> int_dist(0, n * (n - 1) / 2);
	std::uniform_int_distribution<int> vert(0, n - 1);

	cin >> m;

	vector<vector<int>> mat(n, vector<int>(n));

	for (int i = 0; i < m; i++) {
		int u = -1, v = -1;
		while(u == v || mat[u][v] == 1)
			u = vert(gen), v = vert(gen);
		cout << u << ' ' << v << '\n';
		edges[u].push_back(v);
		edges[v].push_back(u);
		mat[u][v] = 1;
		mat[v][u] = 1;
	}

	best = 0;

	for (int i = 0; i < n; i++)
		best = max(best, (int)edges[i].size() + 1);

	auto start = chrono::high_resolution_clock::now();
	auto verrrr = vertex_backtrack(0, edges, vector<int>(n, -1), -1);
	auto end = chrono::high_resolution_clock::now();

	cout << best << '\n';

	cout << chrono::duration_cast<chrono::milliseconds>(end - start).count() << '\n';

	best = 0;

	for (int i = 0; i < n; i++)
		best = max(best, (int)edges[i].size() + 1);

	start = chrono::high_resolution_clock::now();
	edges = rationalization(n, edges, mat);
	verrrr = vertex_backtrack(0, edges, vector<int>(n, -1), -1);
	end = chrono::high_resolution_clock::now();

	cout << best << '\n';

	cout << chrono::duration_cast<chrono::milliseconds>(end - start).count();

}