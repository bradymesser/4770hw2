#include <stdio.h>
#include <stdlib.h>

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

// function prototypes
void CopyNewToOld(float new[][COLS], float old[][COLS]);
void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource);
void PrintGrid(float grid[][COLS], int xsource, int ysource);
void printColors(float mesh[][COLS]);

int main(int argc, char * argv[]) {
  if (argc != 3) {
    printf("No size specified on command line. Using default %dx%d for %d iterations\n",ROWS,COLS,ITERATIONS);
  } else {
    ROWS = atoi(argv[1]);
    COLS = atoi(argv[2]);
    ITERATIONS = atoi(argv[3]);
  }
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
  for (int i = 0; i < ITERATIONS; i++) {
      CopyNewToOld(mesh,old);
      CalculateNew(mesh, old, 0, 0);
  }
  printColors(mesh);
    
  return 0;
}

// function implementations
void CopyNewToOld(float new[][COLS], float old[][COLS]) {
    for (int i = 0; i < ROWS; i++) 
        for (int j = 0; j < COLS; j++) 
            old[i][j] = new[i][j];
}

void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource ) {
  for (int i = 1; i < ROWS-1; i++)
    for (int j = 1; j < COLS-1; j++)
      new[i][j] = 0.25*(old[i-1][j]+old[i+1][j]+old[i][j-1]+old[i][j+1]);
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