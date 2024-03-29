program = main.c
programName = main

default:
	mpicc $(program) -o $(programName)

gcc:
	gcc $(program) -o $(programName)
	./$(programName)

clean:
	-rm $(programName)
	-rm *.gif *.pnm
git:
	git add .
	git commit -m "MAKE GIT"
	git push

init:
	qsub -I -l select=1:ncpus=4:mpiprocs=4,walltime=0:10:00
init16:
	qsub -I -l select=1:ncpus=16:mpiprocs=16,walltime=0:10:00
    
module:
	module add gcc/5.3.0 openmpi/1.8.1

test:
	make default
	mpirun -np 4 --mca mpi_cuda_support 0 $(programName) 1000 1000 1000

16:
	mpicc $(program) -o $(programName)
	mpirun -np 16 --mca mpi_cuda_support 0 $(programName)

sequential:
	gcc sequential.c -o sequential
	./sequential