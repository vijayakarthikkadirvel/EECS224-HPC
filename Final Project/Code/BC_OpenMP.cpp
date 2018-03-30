#include <iostream>
#include <list>
#include <iostream>
#include <iterator>
#include <vector>
#include <stack>
#include <map>
#include <set>
#include <string>
#include <queue>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct PTriple {
	long parent;
	long child;
	long level;
};

vector<PTriple> PTri;

void adjacency_list(vector<long> vertices, vector<vector<long> > vertex_list,
		long V, vector<long> adj[], long no_of_processes_per_processor,
		int current_threads) {
#pragma omp parallel
	{
		for (int i = 0; i < current_threads; i++) {
			for (int j = (no_of_processes_per_processor * i);
					j < ((no_of_processes_per_processor) * (i + 1)); j += 2) {
				adj[vertex_list[i][j]].push_back(vertex_list[i][j + 1]);
			}
		}
	}
}

void prlong_BC(vector<float> BC, long V, string file_name) {
	cout << "\nBetweenness Centrality \n";
	ofstream myfile(file_name);
	if (myfile.is_open()) {
		for (long i = 1; i <= V; i++) {
			myfile << i << " : ";
			myfile << BC[i] << "\n";
		}
		myfile.close();
	} else
		cout << "Unable to open BC file" << endl;
}

bool search(vector<long> adj[], long vertex, vector<vector<long> > vertex_list,
		long i, long no_of_processes_per_processor) {

	for (long j = (no_of_processes_per_processor * i);
			j < ((no_of_processes_per_processor) * (i + 1)); j++) {
		vector<long>::iterator k;
		for(auto &k: adj[vertex]){
			long tmp = k;
		if (tmp == vertex_list[i][j]) {
			return true;
		} else
			continue;

	}
	return false;
}
}

bool search_parent(long parent, int tr,
		vector<vector<long> > vertex_list, long i,
		long no_of_processes_per_processor) {

	for (long j = (no_of_processes_per_processor * i);
			j < ((no_of_processes_per_processor) * (i + 1)); j++) {
		if (PTri[tr].parent == vertex_list[i][j]) {
			return true;
		} else
			continue;

	}
	return false;
}

vector<float> parallel_centrality(vector<vector<long> > vertex_list,
		vector<long> vertices, long V, vector<long> adj[], vector<queue<long> > q,
		long no_of_processes_per_processor, int current_threads,
		vector<float> CB) {
	q[0].push(vertices[0]);
	for (int i = 1; i < current_threads; i++) {
		q[i].push(vertices[no_of_processes_per_processor * i]);
	}
	long L = 0;
	long vertex = 0;
	vector<PTriple> adj_list[V];
	vector<long> delta(V,0);
	int insert_index = 0;
//	for (auto &s : vertices) {
#pragma omp task
		{
			int tr = 0;

			vector<long> sigma(V, 0);
			vector<long> distance(V, -1);
			vector<long> delta(V, 0);
			L = 0;


			std::vector<long>::iterator it;
			for (long i = 0; i < current_threads; i++) {
				while (!q[i].empty()) {
					vertex = q[i].front();
					q[i].pop();
					insert_index = 0;
					vector<long>::iterator k;
					for(auto &k: adj[vertex]){
//					for (k = adj[vertex].begin(); k != adj[vertex].end(); k++) {
						tr++;
						if (search(adj, vertex, vertex_list, i,
								no_of_processes_per_processor)) {
							if (distance[k] < 0) {
								queue<long> queue_tmp;
								queue_tmp.push(k);
								q.at(i) = queue_tmp;
								distance.at(k) = distance.at(vertex) + 1;
							}
							if (distance.at(k) == (distance.at(vertex) + 1)) {
								sigma[k] = sigma[k] + sigma[vertex];

								adj_list[vertex].push_back(PTriple());
								adj_list[vertex].at(insert_index).parent =
										vertex;
								adj_list[vertex].at(insert_index).child = k;
								adj_list[vertex].at(insert_index).level = L;
								insert_index++;
							}
						}
					}
					L++;
				}
			}
		}
		int tr = 0;

	while (L > 0) {

#pragma omp task
		{

						for(auto &de: adj_list[vertex]){



//								if(search_parent(vertex,tr,vertex_list,i,no_of_processes_per_processor)){
//									delta[v] += (sigma[v]/sigma[k])(1+delta[k]);
//								}
								tr++;
							}
					L--;
				}
#pragma omp taskwait
			}

			for (auto &v : vertices) {
				CB[v] = CB[v] + delta[v];
			}
			return CB;
		}



	void split(string s, vector<long> adj[], long V,
			vector<long> vertices) {
		stringstream ss(s);
		vector<string> tokens;
		string buf;
		bool var_bool = true;
		long src = 0, dest = 0;

		while (ss >> buf) {
			if (var_bool) {
				src = stol(buf);
				vertices.push_back(src);
				var_bool = false;
			} else {
				dest = stol(buf);
				vertices.push_back(dest);
				var_bool = true;
			}
		}
	}

	void read_file(string path, vector<long> adj[], long V,
			vector<long> vertices) {
		string line;
		ifstream myfile(path);
		stringstream ss(line);
		string item;
		vector<string> tokens;
		if (myfile.is_open()) {
			while (getline(myfile, line)) {
				cout << line << endl;
				if (line.find("#") != string::npos)
					continue;
				if (line.find(":") != string::npos)
					continue;
				split(line, adj, V, vertices);
			}
			myfile.close();
		} else
			cout << "Unable to open File_100 file" << endl;
	}

	int main(int argc, char* argv[]) {

		long V = stol(argv[4]);
		vector<long> vertices;
		vector<long> adj[V];
		vector<float> vector_BC(V, 0);
		vector<float> CB;

		vector<queue<long> > q;
		string input_filename = argv[1];
		string output_filename = argv[2];
		string runtime_file = argv[3];
		ofstream out;
		clock_t t1, t2;
		vector<vector<long> > vertex_list;
		cout << "Started Reading the file";




		read_file(input_filename, adj, V, vertices);
		int current_threads = omp_get_num_threads(); //gets the number of current processes running
		long no_of_processes_per_processor = V / current_threads;
#pragma omp task
		{
			for (int i = 0; i < current_threads; i++) {
				for (int j = (no_of_processes_per_processor * i);
						j < ((no_of_processes_per_processor) * (i + 1)); j++) {
					vertex_list[i][j] = vertices[j];
				}
			}
		}
#pragma omp taskwait
		adjacency_list(vertices, vertex_list, V, adj,
				no_of_processes_per_processor, current_threads);
		cout << "Calculation of Betweenness Centrality Started" << endl;
		t1 = clock();

		vector_BC = parallel_centrality(vertex_list, vertices, V, adj, q,
				no_of_processes_per_processor, current_threads, CB);



		t2 = clock();
		cout << "Calculation of Betweenness Centrality ended" << endl;

		double run_time = double(t2 - t1) / CLOCKS_PER_SEC;
		cout << "Centrality calculated in : %f" << run_time << endl;
		out.open(runtime_file, std::ios::app);
		out << "\nRun Time for : " << V << " : vertices is : ";
		out << run_time;
		out.close();

		prlong_BC(vector_BC, V, output_filename);
		return 0;
	}

