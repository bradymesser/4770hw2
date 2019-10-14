/*
William Hendrickson
Brady Messer
Bhavik Suthar
CPSC 4770/6770 Hw2
Fall 2019
*/

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define WHITE    "15 15 15 "
#define RED      "15 00 00 "
#define ORANGE   "15 05 00 "
#define YELLOW   "15 10 00 "
#define LTGREEN  "00 13 00 "
#define GREEN    "05 10 00 "
#define LTBLUE   "00 05 10 "
#define BLUE     "00 00 10 "
#define DARKTEAL "00 05 05 "
#define BROWN    "03 03 00 "
#define BLACK    "00 00 00 "

int ROWS = 480;
int COLS = 480;
int ITERATIONS = 1000;
int ROW_HEIGHT = 0;
const int send_tag = 0;
int myrank;
int numprocs;

// function prototypes
void CopyNewToOld(float new[][COLS], float old[][COLS], int xsource, int ysource);
void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource);
void printColors(float mesh[][COLS]);
void mergeMesh(float mesh[][COLS], float tempMesh[][COLS], int rank);

// these seqential functions are included for when the parallelized code is ran with 1 process
void CopyNewToOldSequential(float new[][COLS], float old[][COLS]) {
    for (int i = 0; i < ROWS; i++) 
        for (int j = 0; j < COLS; j++) 
            old[i][j] = new[i][j];
}

// these seqential functions are included for when the parallelized code is ran with 1 process
void CalculateNewSequential(float new[][COLS], float old[][COLS], int xsource, int ysource ) {
  for (int i = 1; i < ROWS-1; i++)
    for (int j = 1; j < COLS-1; j++)
      new[i][j] = 0.25*(old[i-1][j]+old[i+1][j]+old[i][j-1]+old[i][j+1]);
}

int main(int argc, char * argv[]) {
  MPI_Status status;
   
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
  double startTime = MPI_Wtime();

  if (argc != 4) {
    if (myrank == 0) {
        printf("Parameters not specified, using default values: %dx%d with %d iterations.\n",ROWS,COLS, ITERATIONS);
        printf("Valid usage is ./main ROWS COLS ITERATIONS\n");
    }
  } else {
    ROWS = atoi(argv[1]);
    COLS = atoi(argv[2]);
    ITERATIONS = atoi(argv[3]);
  }
  
    
  ROW_HEIGHT = ROWS / numprocs;  // The size of each processes row range
  
  float mesh[ROWS][COLS];
  float old[ROWS][COLS];
  float fireplaceWidth = .4 * COLS;
  MPI_Datatype rowType;
  MPI_Type_contiguous(COLS, MPI_FLOAT, &rowType);
  MPI_Type_commit(&rowType);
    
  // Initialize the mesh to 20.0 degrees celsius, the edges will stay fixed at this temperature
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      mesh[i][j] = 20.0;
      old[i][j] = 20.0;
    }
  }
  // Place the fireplace at the top of the room and set to 300.0 celsius
  int startIndex = (COLS - fireplaceWidth) / 2;
  for (int i = startIndex; i < startIndex + fireplaceWidth; i++) {
    mesh[0][i] = 300.0;
  }
  
  int rowIndex = 0;  // rowIndex is the starting row index of this processes row range
  rowIndex = myrank * ROW_HEIGHT;
  float tempRowTop[COLS];  // 
  float tempRowBottom[COLS];
  float sendSize = COLS;
    
  if (numprocs == 1) {
      for (int i = 0; i < ITERATIONS; i++) {
          CopyNewToOldSequential(mesh,old);
          CalculateNewSequential(mesh, old, 0, 0);
      }
      printColors(mesh);
      double finalTime = MPI_Wtime();
      printf("Total time %f seconds\n", finalTime-startTime);
      MPI_Finalize();
      return 0;
  }
  for (int i = 0; i < ITERATIONS; i++) {
      if (myrank == 0) {
          //send only bottom most row
          MPI_Send(&mesh[rowIndex+ROW_HEIGHT-1], 1, rowType, myrank + 1,send_tag, MPI_COMM_WORLD);
          // receive only bottom + 1 row
          MPI_Recv(&tempRowBottom, 1, rowType, myrank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          // update mesh with bottom + 1 row
          for (int j = 1; j < COLS - 1; j++) {
              // rowIndex + ROW_HEIGHT gives you the row neighboring this section
              mesh[(rowIndex + ROW_HEIGHT)][j] = tempRowBottom[j];
          }
      }
      else if (myrank == numprocs - 1) {
          //send only top most row
          MPI_Send(&mesh[rowIndex], 1, rowType, myrank - 1,send_tag, MPI_COMM_WORLD);
          // receive only top - 1 row
          MPI_Recv(&tempRowTop, 1, rowType, myrank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          // update mesh with top - 1 row
          for (int j = 1; j < COLS - 1; j++) {
              mesh[rowIndex-1][j] = tempRowTop[j];
          }
      }
      else { 
          // send top row down one rank and bottom up one rank
          MPI_Send(&mesh[rowIndex], 1, rowType, myrank - 1,send_tag, MPI_COMM_WORLD);
          MPI_Send(&mesh[rowIndex+ROW_HEIGHT-1], 1, rowType, myrank + 1,send_tag, MPI_COMM_WORLD);
          // receive top and bottom rows
          MPI_Recv(&tempRowTop, 1, rowType, myrank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          MPI_Recv(&tempRowBottom, 1, rowType, myrank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          // update mesh with top and bottom rows
          for (int j = 1; j < COLS - 1; j++) {
              mesh[rowIndex-1][j] = tempRowTop[j];
              mesh[(rowIndex + ROW_HEIGHT)][j] = tempRowBottom[j];
          }
      }
      
      CopyNewToOld(mesh, old, rowIndex, 0);
      CalculateNew(mesh, old, rowIndex, 0); 
  }
    
    MPI_Barrier(MPI_COMM_WORLD);
    double calculationEndTime = MPI_Wtime();
    if (myrank == 0)
        printf("All calculations finished in %f seconds\n",calculationEndTime - startTime);
    
    float tempMesh[ROWS][COLS];
    
    if (myrank != 0)  {
        MPI_Send(&mesh, ROWS*COLS, MPI_FLOAT, 0, send_tag, MPI_COMM_WORLD);
    } else {
        for (int j = 1; j < numprocs; j++) {
            MPI_Recv(&tempMesh, ROWS*COLS, MPI_FLOAT, j, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            mergeMesh(mesh, tempMesh, j);
        }
        double mergeTime = MPI_Wtime();
        printf("Merging took %f seconds\n", mergeTime-calculationEndTime);
        printColors(mesh);
        printf("Printing took %f seconds\n", MPI_Wtime() - mergeTime);
    }
    
  if (myrank == 0) {
      double finalTime = MPI_Wtime();
      printf("Total time %f seconds\n", finalTime-startTime);
  }
    
  MPI_Finalize();
  return 0;
}

// function implementations
void CopyNewToOld(float new[][COLS], float old[][COLS], int xsource, int ysource) {
    if (myrank == 0) {
        for (int i = xsource; i <= xsource + ROW_HEIGHT; i++) {
            for (int j = 0; j < COLS; j++) {
                old[i][j] = new[i][j];
            }
        }
    }
    else if (myrank == numprocs - 1) {
        xsource--;
        for (int i = xsource; i <= xsource + ROW_HEIGHT; i++) {
            for (int j = 0; j < COLS; j++) {
                old[i][j] = new[i][j];
            }
        }
    } else {
        for (int i = xsource-1; i <= xsource + ROW_HEIGHT+1; i++) {
            for (int j = 0; j < COLS; j++) {
                old[i][j] = new[i][j];
            }
        }
    }
}

void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource) {
 
  for (int i = xsource; i < xsource + ROW_HEIGHT; i++)
    for (int j = 1; j < COLS-1; j++)
      if (i != 0 && i!= ROWS - 1)
          new[i][j] = 0.25*(old[i-1][j]+old[i+1][j]+old[i][j-1]+old[i][j+1]);
}

void printColors(float mesh[][COLS]) {
    FILE * fp;

   int numcolors = 10;
   char * color = BLACK;

   /* Colors are list in order of intensity */
   char * colors[10] = { RED, ORANGE, YELLOW, LTGREEN, GREEN, 
                         LTBLUE, BLUE, DARKTEAL, BROWN, BLACK };

   int linelen = COLS;
   int numlines = ROWS;
   int i, j;

   /* The pnm filename is hard-coded.  */

   fp = fopen("c.pnm", "w");

   /* Print the P3 format header */
   fprintf(fp, "P3\n%d %d\n15\n", linelen, numlines);

   /* Each %s (color string) is a single pixel in the final image */
   for (i=0; i<numlines; i++) {
      for (j=0; j<linelen; j++) {
         if (mesh[i][j] >= 250.0)
             color = RED;
         else if (mesh[i][j] >= 180.0)
             color = ORANGE;
         else if (mesh[i][j] >= 120.0)
             color = YELLOW;
         else if (mesh[i][j] >= 80.0) 
             color = LTGREEN;
         else if (mesh[i][j] >= 60.0)
             color = GREEN;
         else if (mesh[i][j] >= 50.0)
             color = LTBLUE;
         else if (mesh[i][j] >= 40.0)
             color = BLUE;
         else if (mesh[i][j] >= 30.0)
             color = DARKTEAL;
         else if (mesh[i][j] > 20.0) 
             color = BROWN;
         else 
             color = BLACK;
         fprintf(fp, "%s ", color );
      }
      fprintf(fp, "\n");
   }

   fclose(fp);

   /* Convert the pnm file to a format that is more easily viewed
      in a web browser. */ 
   /*   system("convert c.pnm c.png"); */
   system("convert c.pnm c.gif"); 
}

void mergeMesh(float mesh[][COLS], float tempMesh[][COLS], int rank) {
    int rowIndex = rank * ROW_HEIGHT;
    for (int i = rowIndex; i < rowIndex+ROW_HEIGHT; i++) {
        for (int j = 1; j < COLS-1; j++) {
            mesh[i][j] = tempMesh[i][j];
        }
    }
}
