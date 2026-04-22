#include <iostream>
#include <vector>
#include <random>
#include <queue>
#include <set>

using namespace std;

int n, m;
int best;

const int INF = 1e9;

vector<int> dsatur(vector<vector<int>>& graph) {
	int n = graph.size(); // число вершин
	vector<int> colors(n, -1); // цвета вершин
	vector<set<int>> sat(n); // степень насыщения

	queue<int> q;
	q.push(0);
	while (!q.empty()) { // обход в ширину
		int v = q.front();
		q.pop();

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
			sat[i].insert(min_color); // пересчитываем степень насыщения для смежных вершин
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

int next_vertex(vector<vector<int>>& edges, vector<set<int>>& sat, vector<int>& solution) {
	int mx = -1, next_vertex = -1, deg = -1;
	// процесс выбора следующей вершины по двум критериям:
	// 1) степень насыщения
	// 2) степень вершины
	for (int i = 0; i < n; i++) {
		if (solution[i] == -1 && (mx < (int)sat[i].size() ||
			mx == (int)sat[i].size() && (int)edges[i].size() > deg)) {
			mx = sat[i].size();
			next_vertex = i;
			deg = edges[i].size();
		}
	}

	return next_vertex;

}


int find_chromatic_number(int v, vector<vector<int>>& edges, vector<int> solution, vector<set<int>>& sat, int large_color, int cnt) {
	if (large_color >= best - 1)
		return INF;
	if (cnt == n) { // просмотрели все вершины
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
		vector<set<int>> new_sat = sat;
		solution[v] = 0;
		for (int i : edges[v]) {
			new_sat[i].insert(0); // пересчитываем степень насыщения для смежных вершин
		}
		int next = next_vertex(edges, new_sat, solution);
		min_ans = find_chromatic_number(next, edges, solution, new_sat, 0, 1);
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
				vector<set<int>> new_sat = sat;
				for (int i : edges[v]) {
					new_sat[i].insert(color); // пересчитываем степень насыщения для смежных вершин
				}
				int next = next_vertex(edges, new_sat, solution);
				int cur_ans = find_chromatic_number(next, edges, solution, new_sat, max(color, large_color), cnt + 1); // запускаем рекурсию
				min_ans = min(min_ans, cur_ans); // из всех ответов ищем минимальный
			}
		}
	}
	return min_ans;
}



int main() {
	cin >> n;
	vector<vector<int>> edges(n);

	std::random_device rd;
	std::mt19937 gen(rd());

	std::uniform_int_distribution<int> vert(0, n - 1);

	cin >> m;

	vector<vector<int>> mat(n, vector<int>(n));

	for (int i = 0; i < m; i++) {
		int u = -1, v = -1;
		while (u == v || mat[u][v] == 1)
			u = vert(gen), v = vert(gen);
		cout << u << ' ' << v << '\n';
		edges[u].push_back(v);
		edges[v].push_back(u);
		mat[u][v] = 1;
		mat[v][u] = 1;
	}

	vector<int> ans = dsatur(edges);
	for (int i = 0; i < n; i++)
		cout << ans[i] << ' ';
}