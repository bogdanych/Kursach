#include <iostream>
#include <vector>

using namespace std;

int zykov_method(vector<vector<bool>>& g) {
	int n = g.size();

	pair<int, int> res = find_non_adj(g);
	int u = res.first;
	int v = res.second;

	if (u == -1 && v == -1) {
		return n;
	}

	vector<vector<bool>> g1 = add_edge(g, u, v);
	vector<vector<bool>> g2 = contract_vertices(g, u, v);

	int chi1 = zykov_method(g1);
	int chi2 = zykov_method(g2);

	return min(chi1, chi2);
}

vector<vector<bool>> add_edge(vector<vector<bool>>& g, int u, int v) {
	vector<vector<bool>> new_g = g;
	new_g[u][v] = 1;
	new_g[v][u] = 1;

	return new_g;
}

vector<vector<bool>> contract_vertices(vector<vector<bool>>& g, int u, int v) {
	int n = g.size();

	vector<vector<bool>> new_g(n - 1, vector<bool>(n - 1, false));

	int new_i = 0;
	for (int i = 0; i < n; i++) {
		if (i == v) continue;

		int new_j = 0;
		for (int j = 0; j < n; j++) {
			if (j == v) continue;

			if (i != j) {
				if (g[i][j]) {
					new_g[new_i][new_j] = true;
				}

				if ((i == u && g[v][j]) || (j == u && g[i][v])) {
					new_g[new_i][new_j] = true;
				}
			}

			new_j++;
		}
		new_i++;
	}

	return new_g;
}

pair<int, int> find_non_adj(vector<vector<bool>>& g) {
	int n = g.size();
	for (int i = 0; i < n; i++) {
		for (int j = i + 1; j < n; j++) {
			if (!g[i][j]) {
				return { i, j };
			}
		}
	}
	return { -1, -1 };
}