

# Ant_algorithm-ACO
This is a 3 students team project for parallel programing class. In this project, we tried to simulate the real world ants behavior. 
The demo video is on Youtube. Link : https://www.youtube.com/watch?v=BTM4Y7a898Q

## Introduction
>  The ant colony optimization system (ACO) was published by Dorigo, Maniezzo, and Colorni in 1996. Inspired by the foraging behavior of real ants, ACO have emerged as an efficient computational method to solve combinatorial optimization problems. One of the problems is searching optimal path in the large, unknown and dynamic environments. Especially for finding multi-targets simultaneously on map, many researchers have delivered lots of works to optimize ACO algorithm.  
>    
>  Real world environment is an open space which means that it might be infinite solutions. By using DFS or BFS, we may run out of memory and time. On the other hand, the ACO is superior to dynamic path searching in open space because ants don’t need to memorize the paths they went. However, ACO is facing with computing speed dilemma on basic functions, such as the movements of ants and hormone calculation. Both of these operations will slow down the speed of this algorithm dramatically when the number of ants increase. That’s why we decided to use parallel programing to speed up the algorithm on the coding aspect
  
## Feature  
Unlike most of ACO designed for graph map. We make ants run in a large 3D space. We had tested 1000 x 1000 x 10 space with 10000 ants.  
  
## Flow chart  
S[Start] --> A[Initial]
--> B[Leave hormone per ant] --> C[Ants choose directions] --> D[Decline hormone map]
--> E{Done?F:B} --> F[Finish]
  
## Parallel libraries
* pthread
* OpenMP
* CUDA
* SSE

## Future work
* Use CPU and GPU together (pthread + SSE + CUDA)
* Move all the source code to GPU
* Build a much simpler model with less parameters.

## Code reading suggest  
We strongly suggest read serial version first because the code is much cleaner.  
