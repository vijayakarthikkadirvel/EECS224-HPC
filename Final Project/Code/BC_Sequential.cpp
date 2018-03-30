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
#include <chrono>

using namespace std;

map<int, vector<int> > adj;
map<int, float> BC;
set<int> set_vertices_all;
long edge_count=0;

//Method for forming the adjacency list
void edge_add(int src, int dest) {
//	cout << "Gone " << src << ":" << dest << "\n";
	set_vertices_all.insert(src);
	set_vertices_all.insert(dest);
	adj[src].push_back(dest);
	edge_count++;
//	adj[dest].push_back(src);

}

//Method for printing the adj
void adj_print() {

	for (const auto& pair : adj) {
		cout << "Vertex: " << pair.first << "  and its neighbors are : ";
		for (int d : pair.second)
			cout << d << "  ";
		cout <<endl;
	}

}

void print_BC(int V, string bc_file) {
//	cout << "\nBetweenness Centrality \n";
	ofstream out;
		out.open(bc_file);

		for (auto i : set_vertices_all) {
			//cout << "Node " << i << ": " << nodeBetweenness[i] / nrml << endl;
			out << "Vertex " << i << ": "<<BC[i]<<endl;
//					<< BC[i] / ((V - 1) * (V - 2)) << endl;
		}
	out.close();
//	ofstream myfile(file_name);
//	if (myfile.is_open()) {
//		for (auto i : set_vertices_all) {
////			cout << "Vertex  : " << i << " : " << BC[i] << "\n";
//			myfile << i << " : ";
//			myfile << BC[i] << "\n";
//		}
//		myfile.close();
//	} else
//		cout << "Unable to open BC file" << endl;
}

//Method for calculating the Betweenness Centrality
void calculate_centrality(int V) { //	cout << "went in calculate centrality";

	//	iterating through each vertex
	int vertex_count=1;
	for (auto source_vertex:set_vertices_all) {
		vertex_count++;
//		if(vertex_count%1000==0) cout<<source_vertex<<endl;
		//		Begin of Initialization
//		cout << "Vertex : " << source_vertex << endl;

		map<int, int> sigma, distance;
		map<int, float> delta;
		map<int, set<int> > map_predecessor;
		queue<int> q;
		stack<int> st;
		for (auto source_vertex_tmp : set_vertices_all) {
					sigma[source_vertex_tmp] = 0;
					distance[source_vertex_tmp] = -1;
					delta[source_vertex_tmp] = 0;

				}

		distance[source_vertex] = 0;
		sigma[source_vertex] = 1;

		q.push(source_vertex);
		while (!q.empty()) {

			int vertex = q.front();
			q.pop();
			st.push(vertex);

			for (auto &i : adj[vertex]) {
//				cout << "Vertex : " << vertex << " neighbor :" << i << endl;
				if (distance[i] < 0) {
					q.push(i);
					distance[i] = distance[vertex] + 1;
				}

				if (distance[i] == distance[vertex] + 1) {
					sigma[i] = sigma[i] + sigma[vertex];

						map_predecessor[i].insert(vertex);
					}
				}
			}


		while (!st.empty()) {
			int st_neigh = st.top();
			st.pop();
			for (auto &vertex_pred : map_predecessor[st_neigh]) {
				//				cout << "neigh : " << st_neigh << " pred : " << vertex_pred
				//						<< endl;

				//				cout << "before:\n" << "delta[" << vertex_pred << "] : "
				//						<< delta[vertex_pred];
				float tmp_delta = delta[vertex_pred]
						+ (((float) ((float) sigma[vertex_pred]
								/ (float) sigma[st_neigh]))
								* (1 + delta[st_neigh]));
				delta[vertex_pred] += tmp_delta;

				if (source_vertex != st_neigh)
					BC[st_neigh] += delta[st_neigh];
			}
		}

	}

}

bool split(const string &s, int V) {
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

	if (src <= V && dest <= V)
		edge_add(src, dest);
	if(src>V) return false;
//	else return false;

	return true;
}

// Method read file
void read_file(string path, long V) {
//	Reference - reading the file
//	http://www.cplusplus.com/doc/tutorial/files/

	string line;
	ifstream myfile(path);
	stringstream ss(line);

//	  Reference - split functionality
//	  http://ysonggit.github.io/coding/2014/12/16/split-a-string-using-c.html
	int count=0;
	int read_count=1;
	if (myfile.is_open()) {
		while (getline(myfile, line)) {
//			cout << line << endl;
			if (line.find("#") != string::npos)
				continue;
			if (line.find(":") != string::npos)
				continue;
			bool var  = split(line, V);
			read_count++;
//			if(read_count%100000==0) cout<<read_count<<" read . ";
			if(!var) count++;
//			if(count==20) myfile.close();


		}
		myfile.close();
	} else
		cout << "Unable to open File_100 file" << endl;
}

int main(int argc, char* argv[]) {

//	Declarations
	int V = stoi(argv[3]);
	V++;
//	vector<int> adj[V];
	ofstream out;
	string input_filename = argv[1];
//	string output_filename = argv[2];
//	string runtime_file = argv[3];
	clock_t t1, t2;
	typedef std::chrono::high_resolution_clock Clock;
	typedef std::chrono::milliseconds milliseconds;


	cout << endl << "Sequential Betweenness Centrality" << endl<<endl;
//	Reading from the file
	cout<<"Reading of file started"<<endl;
	read_file(input_filename, V);
	cout << "reading the file finished" << endl;
//	adj_print();
	cout << "Calculation of Betweenness Centrality Started" << endl;
//	t1=chrono::high_resolution_clock::now();
	Clock::time_point t0 = Clock::now();


//	calculating the centrality
	calculate_centrality(V);
	Clock::time_point t3 = Clock::now();
	milliseconds ms = std::chrono::duration_cast<milliseconds>(t3 - t0);
//		    std::cout << ms.count() << "ms\n";

	auto done = std::chrono::high_resolution_clock::now();


	cout << "Calculation of Betweenness Centrality ended" << endl;
//	double run_time = double((t2 - t1)*100) / CLOCKS_PER_SEC;
//	cout << "Centrality calculated in : %f" << ms.count() << "ms\n"<< endl;

	out.open("sequential_run_time_BC.txt", std::ios::app);
	cout << "\nSequential Run Time for : " << V-1 << " : vertices  and "<< V-1<<" edges is : "<<ms.count() <<" ms";
	out << "\nRun Time for : " << V << " : vertices  and "<< V<<" edges is : ";
	out << ms.count() <<"ms";
	out.close();

//	Print Betweenness Centrality
	print_BC(V,argv[2]);

	return 0;
}
