# CPSC 4770/6770 Fall 2019
# Assignment 2

Team Members: 
William Hendrickson
Brady Messer
Bhavik Suthar

Specifying grid size and iterations:
    - When running either the parallelized or sequential version, grid size and iterations can be specified on the command line
    - ./program rows columns iterations
    example: ./main 480 480 100000
    - If not specified on the command line, default values defined in the code are used
    
Compilation Instructions for Parallel Optimization:
: make module
    - Add GCC and OpenMPI (Note sometimes it doesn't work and will have to be copy/pasted)
: make
    - Compiles the parallelized version of the code
: make 16
    - Compiles and runs the parallelized code with 16 processors and the default values 
    
Other makefile commands:
: make clean
    - Removes all executables and image files from the directory 
: make init
    - request 4 processors and 10 minutes of time
: make init16
    - request 16 processors and 10 minutes of time
: make test
    - compile the parallelized code and run it with specified values: 1000x1000 for 1000 iterations 
    - note 1000 is not a power of 2 and won't work for all numbers of processes
: make sequential
    - compile the sequential code and run it with the default values

# Team Roles
# *Note* Group coding was done for all parts of project


- Brady Messer
    Wrote sequential implementation
    Helped with code optimization by array access orders (row major instead of column major)
    
- William Hendrickson
    Helped with parallel implementation and division of grid
    Helped document the code and write the readme

- Bhavik Suthar
    Helped with MPI functions and processes
    Helped with code optimization by function calls
    
We all coded the parallel implementation together on one computer.
This helped us catch errors quickly, and reason through complicated aspects of the code


