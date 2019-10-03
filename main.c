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

int ROWS = 10;
int COLS = 10;
int ITERATIONS = 1000;
int COLUMN_WIDTH = 0;
const int send_tag = 0;
// function prototypes
void CopyNewToOld(float new[][COLS], float old[][COLS]);
void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource);
void PrintGrid(float grid[][COLS], int xsource, int ysource);
void printColors(float mesh[][COLS]);
void copyBoundaryColumns(float new[][COLS], float old[ROWS], int columnIndex);

int main(int argc, char * argv[]) {
  MPI_Status status;
 
  int myrank;
  int numprocs;
  
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &myrank);
  MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
    
  if (argc != 4) {
    printf("Invalid usage, using default values: %dx%d with %d iterations.\n",ROWS,COLS, ITERATIONS);
    printf("Valid usage is ./main ROWS COLS ITERATIONS\n");
  } else {
    ROWS = atoi(argv[1]);
    COLS = atoi(argv[2]);
    ITERATIONS = atoi(argv[3]);
  }
  
    
  COLUMN_WIDTH = COLS / numprocs;
  
  float mesh[ROWS][COLS];
  float old[ROWS][COLS];
  float fireplaceWidth = .4 * COLS;
  // Initialize the mesh to 20.0 degrees celsius, the edges will stay fixed at this temperature
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      mesh[i][j] = 20.0;
    }
  }
  // Place the fireplace at the top of the room and set to 300.0 celsius
  int startIndex = (COLS - fireplaceWidth) / 2;
  for (int i = startIndex; i < startIndex + fireplaceWidth; i++) {
    mesh[0][i] = 300.0;
  }
  
  int columnIndex = 0;
  columnIndex = myrank * COLUMN_WIDTH;
  float tempColLeft[ROWS];
  float tempColRight[ROWS];
  float sendSize = 1;
    
  for (int i = 0; i < ITERATIONS; i++) {
      if (myrank == 0) {
          // calculate right column
          for (int j = 1; j < ROWS - 1; j++) {
              tempColRight[j] = mesh[j][(columnIndex + COLUMN_WIDTH) - 1];
          }
          //send only right most column
          MPI_Send(&tempColRight, sendSize, MPI_FLOAT, myrank + 1,send_tag, MPI_COMM_WORLD);
          // receive only right most column
          MPI_Recv(&tempColRight, sendSize, MPI_FLOAT, myrank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          // update mesh with right most column
          for (int j = 1; j < ROWS - 1; j++) {
              // columnIndex + COLUMN_WIDTH gives you the column neighboring this one
              mesh[j][(columnIndex + COLUMN_WIDTH)] = tempColRight[j];
          }
      }
      else if (myrank == numprocs - 1) {
          // calculate left column
          for (int j = 1; j < ROWS - 1; j++) {
              tempColLeft[j] = mesh[j][columnIndex];
          }
          //send only left most column
          MPI_Send(&tempColLeft, sendSize, MPI_FLOAT, myrank - 1,send_tag, MPI_COMM_WORLD);
          // receive only left most
          MPI_Recv(&tempColLeft, sendSize, MPI_FLOAT, myrank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          // update mesh with left most column
          for (int j = 1; j < ROWS - 1; j++) {
              mesh[j][columnIndex-1] = tempColLeft[j];
          }
      }
      else { 
          // calculate left and right columns
          for (int j = 1; j < ROWS - 1; j++) {
              tempColLeft[j] = mesh[j][columnIndex];
              tempColRight[j] = mesh[j][(columnIndex + COLUMN_WIDTH)];
          }
          // send left column down one rank and right up one rank
          MPI_Send(&tempColLeft, sendSize, MPI_FLOAT, myrank - 1,send_tag, MPI_COMM_WORLD);
          MPI_Send(&tempColRight, sendSize, MPI_FLOAT, myrank + 1,send_tag, MPI_COMM_WORLD);
          // receive left and right columns
          MPI_Recv(&tempColLeft, sendSize, MPI_FLOAT, myrank - 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          MPI_Recv(&tempColRight, sendSize, MPI_FLOAT, myrank + 1, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
          // update mesh with left and right most
          for (int j = 1; j < ROWS - 1; j++) {
              mesh[j][columnIndex-1] = tempColLeft[j];
              mesh[j][(columnIndex + COLUMN_WIDTH)] = tempColRight[j];
          }
      }
      
      CopyNewToOld(mesh, old);
      CalculateNew(mesh, old, 0, columnIndex); 
  }
  if (myrank > 0) 
      MPI_Barrier(MPI_COMM_WORLD);
  if (myrank > 1)
      MPI_Barrier(MPI_COMM_WORLD);
  if (myrank > 2)
      MPI_Barrier(MPI_COMM_WORLD);

  printf("Process %d\n", myrank);
  for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
          printf("%5.1f ", mesh[i][j]);
      }
      printf("\n");
  }
//   if (myrank == 0) {
      MPI_Barrier(MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
      MPI_Barrier(MPI_COMM_WORLD);
//       printColors(mesh);
//   }
//   printColors(mesh);
  MPI_Finalize();
  return 0;
}

// function implementations
void CopyNewToOld(float new[][COLS], float old[][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            old[i][j] = new[i][j];
        }
    }
}

void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource) {
  // pseudo code
  
  for (int i = 1; i < ROWS-1; i++)
      // error here, only works for 2 processes (ysource + COLUMN_WIDTH - 1) *** I THINK I FIXED IT
    for (int j = ysource; j < ysource + COLUMN_WIDTH; j++)
      if (j != 0)
          new[i][j] = 0.25*(old[i-1][j]+old[i+1][j]+old[i][j-1]+old[i][j+1]);
}

void PrintGrid(float grid[][COLS], int xsource, int ysource) {

}

void printColors(float mesh[][COLS]) {
    FILE * fp;

   int numcolors = 10;
   char * color = BLACK;

   /* Colors are list in order of intensity */
   char * colors[10] = { RED, ORANGE, YELLOW, LTGREEN, GREEN, 
                         LTBLUE, BLUE, DARKTEAL, BROWN, BLACK };

   /* The image will be 300 pixels wide and 300 pixels tall */
   int linelen = COLS;
   int numlines = ROWS;
   int i, j;

   /* The pnm filename is hard-coded.  */

   fp = fopen("c.pnm", "w");

   /* Print the P3 format header */
   fprintf(fp, "P3\n%d %d\n15\n", linelen, numlines);

   /* Print 300 lines of colors. ASCII makes this easy.           */
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
