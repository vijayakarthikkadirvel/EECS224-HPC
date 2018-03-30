PARALLEL ALGORITHM FOR FINDING BETWEENNESS CENTRALITY USING BRANDES ALGORITHM:


In this project, We present the efficient parallel algorithm to find the betweenness centrality using brandes method. Brandes algorithm[1] presents the most efficient algorithm with time Complexity of O(nm) for unweighted graph and Space Complexity of O(n+m), where n is the number of vertices and m is the number of edges. Making it parallel reduces the complexity from O(nm) to O(nm/p) for an unweighted graph, where p is the number of processors. We used two different parallel programming models, OpenMP and MPI to parallelize the algorithm. Our first algorithm uses a proper data-processor mapping, a novel edge-numbering strategy and a new triple array data structure recording the shortest path for eliminating conflicts to access the shared memory [2]. This can be done using OpenMP but this parallel model has few drawbacks which can be eliminated by MPI. Speedup achieved is significant using MPI rather than OpenMP. 


There are 2 folders
1. Code - this folder has the sequential and parallel code of Betweenness Centrality
2. Results - this folder has results what we got for various datasets. In the folder "seq_BC_8846" file means
             the file has Betweenness Centrality values for the sequential code of dataset of 8846 vertices.
             In the folder "par_BC_8846" means the file has Betweenness Centrality values for the parallel code of dataset 8846 vertices.

To Run the code:
1. Open Folder with the name "code"
2. Run "make clean" command.
3. Run "make" command.
4. 2 executable files namely "serial-brandes" and "parallel-brandes" will be generated for
5. For running the sequential Code the command has to be passed with 4 parameters
    1st parameter - path of test_file
    2nd parameter - path of file where betweenness centrality values should be written
    3rd parameter - Number of Vertices. Adjacency List will be formed based on this value. Also, the vertices greater than this value won't be read from the file. This check is done to avoid the segmentation fault
   for example
   ./serial-brandes test123.txt seq_BC_100.txt 10

6. For running the parallel Code the command has to be passed with 4 parameters
   1st parameter - path of test_file
   2nd parameter - path of file where betweenness centrality values should be written 3rd parameter - Number of Vertices. Adjacency List will be formed based on this value. Also, the vertices greater than this value won't be read from the file. This check is done to avoid the segmentation fault.
   for example
    mpirun -np 4 ./parallel-brandes test123.txt par_BC_100.txt 10

7. Once both the sequential and parallel executables have been run
   a. the sequential run time can be seen in "sequential_run_time_BC.txt" file
   b. the parallel run time can be seen in "parallel_run_time_BC.txt" file
   c. for the above cases "seq_BC_100.txt" and "par_BC_100.txt" files will be generated with centrality values 
