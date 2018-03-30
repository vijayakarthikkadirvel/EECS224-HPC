REDUCTION USING CUDA AND DIVERSE OPTIMISATION TECHNQUES:

I implemented reduction using CUDA to reduce the elements in the array to a single element which is the summation of all the elements.
During this implementation, I used many different optimisation techniques,
-> Naive parallel reduction
-> strided access by threads to reduce
-> sequential reduction
-> First add before reduce
-> Loop unrolling
-> Algorithm Cascading


To run it:
1.) make 
2.) run the script in HPC cluster 
3.) Check the results after the execution is over in the HPC cluster.
