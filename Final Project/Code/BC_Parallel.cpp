//============================================================================================
// Name        : test.cpp
// Author      : Vijaya Karthik Kadirvel (58968746)
//				 Snehal Srinivas Gandham (79007757)
// Version     : 1.0
// Course      : High Performance Computing
// Description : Betweenness Centrality calculation using OpenMP.
//				 Brandes Algorithm has been used for calculating Betweenness Centrality
//============================================================================================

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
#include <sys/time.h>
#include <mpi.h>
#include <chrono>

using namespace std;

struct neig {
	int target;
	int weight;
	neig(int mTarget, int mWeight) :
			target(mTarget), weight(mWeight) {
	}
};

typedef vector<vector<neig> > adjacency_list;
set<int> set_vertices_all;
float shortest_path_func(int src, int n, stack<int> &visitStack,
		vector<int> &sigma, list<int> *pred, adjacency_list &adjList) {

	float closeness = 0;

	vector<int> dist;
	dist.resize(n, -1);
	dist[src] = 0;

	queue<int> visitQueue;
	visitQueue.push(src);

	while (!visitQueue.empty()) {

		int v = visitQueue.front();
		visitQueue.pop();
		visitStack.push(v);

		closeness += dist[v];

		for (vector<neig>::iterator it = adjList[v].begin();
				it != adjList[v].end(); it++) {
			int w = it->target;

			if (dist[w] < 0) {
				visitQueue.push(w);
				dist[w] = dist[v] + 1;
			}

			if (dist[w] == dist[v] + 1) {
				pred[w].push_back(v);
				sigma[w] += sigma[v];
			}
		}

	}

	if (closeness != 0) {
		return 1.0 / closeness;
	} else {
		return 0;
	}
}

void time_print(int n, int e, float t) {
	ofstream out;
	out.open("parallel_run_time_BC.txt", std::ios::app);

	cout << "\nParallel Run Time for " << n << " Nodes and " <<n
			<< " Edges  is : " << t << " ms ";
	out << "\nParallel Run Time for " << n << " Nodes and " << n
			<< " Edges  is : " << t;

	out.close();
}
bool split(const string &s, int V, adjacency_list &adjList, char* input) {
	stringstream ss(s);
	string buf;

	bool var_bool = true;
	long src = 0, dest = 0;
	while (ss >> buf) {

		if (var_bool) {
			src = stol(buf);
			var_bool = false;
		} else {
			dest = stol(buf);
			var_bool = true;
		}

	}
//	cout << src << " : " << dest;
	if (src <= V && dest <= V)
		adjList[src].push_back(neig(dest, 1));
	if (src > V)
		return false;
//	else return false;

	return true;
}

// Method read file
void read_file(long n, adjacency_list &adjList, char* input) {

	string line;
	ifstream myfile(input);
	stringstream ss(line);
	adjList.reserve(500000);
//	cout << input << endl;
	int count = 0;
	int read_count = 1;
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
//			cout << line << endl;
			if (line.find("#") != string::npos)
				continue;
			if (line.find(":") != string::npos)
				continue;

			stringstream ss(line);
			string buf;

			bool var_bool = true;
			int src = 0, dest = 0;
			while (ss >> buf) {

				if (var_bool) {
					src = stoi(buf);
					var_bool = false;
				} else {
					dest = stoi(buf);
					var_bool = true;
				}

			}

			if (src < n && dest < n) {
				adjList[src].push_back(neig(dest, 1));
//				cout << src << " : " << dest<<endl;
			} else {
				count++;
//			if (read_count % 100000 == 0)
//				cout << read_count << " read . ";

//				if (count == 20)
//					myfile.close();
			}
		}
		myfile.close();
	} else
		cout << "Unable to open File_100 file" << endl;
}


void betweenness_centrality_print(int n, vector<float> nodeBetweenness,
		string bc_file) {

	ofstream out;
	out.open(bc_file);
//	cout << endl << "> Parallel Betweenness Centrality" << endl;
	for (int i = 0; i < n; i++) {
		//cout << "Node " << i << ": " << nodeBetweenness[i] / nrml << endl;
		out << "Vertex " << i << ": " << nodeBetweenness[i] << endl;
//				<< nodeBetweenness[i] / ((n - 1) * (n - 2)) << endl;
	}
	out.close();
}

int main(int argc, char* argv[]) {

	MPI_Init(NULL, NULL);
	int world_size;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	int world_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

	int n = stoi(argv[3]), e = 100000;
	adjacency_list adjList;

	vector<float> nodeBetweenness;
	vector<float> nodeBetweenness_g;
	vector<float> closeness;
//	cout << endl << "Parallel Betweenness Centrality" << endl << endl;
//	cout << "Before going into read file";
//	cout << "Reading of file started" << endl;
	read_file(n, adjList, argv[1]);

	//cout << "reading the file finished" << endl;


	nodeBetweenness.resize(n, 0);
	nodeBetweenness_g.resize(n, 0);
	closeness.resize(n, 0);

	list<int> pred[n];
	vector<int> sigma;
	vector<float> delta;
	stack<int> visit_nodes;
	MPI_Barrier(MPI_COMM_WORLD);
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::milliseconds milliseconds;
//	cout << "Calculation of Betweenness Centrality Started" << endl;
	clock_t t1, t2;
	t1 = clock();
	Clock::time_point t0 = Clock::now();

	int begin_vertex = n / world_size * world_rank;
	int end_vertex = n / world_size * (world_rank + 1);
	if (world_rank == world_size - 1) {
		end_vertex = n;
	}
	for (int src = begin_vertex; src < end_vertex; src++) {

		if (src < n) {
			for (int i = 0; i < n; i++) {
				pred[i].clear();
			}

			sigma.clear();
			sigma.resize(n, 0);
			sigma[src] = 1;

			delta.clear();
			delta.resize(n, 0);

			closeness[src] = shortest_path_func(src, n, visit_nodes, sigma,
					pred, adjList);

			while (!visit_nodes.empty()) {
				int w = visit_nodes.top();
				visit_nodes.pop();

				for (list<int>::iterator it = pred[w].begin();
						it != pred[w].end(); it++) {
					int v = *it;
					float c = ((float) sigma[v] / (float) sigma[w])
							* (1.0 + delta[w]);

					delta[v] += c;
				}

				if (w != src) {
					nodeBetweenness[w] += delta[w];
				}
			}
		}

	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Reduce(&nodeBetweenness.front(), &nodeBetweenness_g.front(), n,
	MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
	Clock::time_point t3 = Clock::now();
	t2 = clock();

//	cout << "Calculation of Betweenness Centrality ended" << endl;

	MPI_Barrier(MPI_COMM_WORLD);
	if (world_rank == 0) {
		milliseconds ms = std::chrono::duration_cast<milliseconds>(t3 - t0);
		double t = double((t2 - t1) * 100) / CLOCKS_PER_SEC;
		cout << "t : " << t << endl;

		time_print(n, e, ms.count());

		betweenness_centrality_print(n, nodeBetweenness_g, argv[2]);

//		cout << "\n";
//		cout << "Time Taken : " << ms.count();
//		cout << "\n";
	}
	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Finalize();
	return 0;
}
