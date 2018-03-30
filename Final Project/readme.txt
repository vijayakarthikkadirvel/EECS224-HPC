Project Members : Vijaya karthik kadirvel (58968746)
				  Snehal Srinivas Gandham (79007757) 


There are 2 folders
1. Code - this folder has the sequential and parallel code of Betweenness Centrality
2. Results - this folder has results what we got for various datasets. In the folder "seq_BC_8846" file means
             the file has Betweenness Centrality values for the sequential code of dataset of 8846 vertices.
             In the folder "par_BC_8846" means the file has Betweenness Centrality values for the parallel code of
             dataset of 8846 vertices.

To Run the code:
1. Open Folder with the name "code"
2. Run "make clean" command.
3. Run "make" command.
4. 2 executable files namely "serial-brandes" and "parallel-brandes" will be generated for
5. For running the sequential Code the command has to be passed with 4 parameters
    1st parameter - path of test_file
    2nd parameter - path of file where betweenness centrality values should be written
    3rd parameter - Number of Vertices. Adjacency List will be formed based on this value. Also, the vertices
                    greater than this value won't be read from the file. This check is done to avoid the
                    segmentation fault
   for example
   ./serial-brandes test123.txt seq_BC_100.txt 10

6. For running the parallel Code the command has to be passed with 4 parameters
   1st parameter - path of test_file
   2nd parameter - path of file where betweenness centrality values should be written
   3rd parameter - Number of Vertices. Adjacency List will be formed based on this value. Also, the vertices
                   greater than this value won't be read from the file. This check is done to avoid the
                   segmentation fault
   for example
    mpirun -np 4 ./parallel-brandes test123.txt par_BC_100.txt 10

7. Once both the sequential and parallel executables have been run
   a. the sequential run time can be seen in "sequential_run_time_BC.txt" file
   b. the parallel run time can be seen in "parallel_run_time_BC.txt" file
   c. for the above cases "seq_BC_100.txt" and "par_BC_100.txt" files will be generated with centrality values 
