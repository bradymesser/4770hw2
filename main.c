#include <stdio.h>

int ROWS = 1000;
int COLS = 1000;

// function prototypes
void CopyNewToOld(float new[][COLS], float old[][COLS]);
void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource);
void PrintGrid(float grid[][COLS], int xsource, int ysource);


int main(int argc, char * argv[]) {
  if (argc != 3) {
    printf("No size specified on command line. Using default %dx%d\n",ROWS,COLS);
  } else {
    ROWS = argv[1];
    COLS = argv[2];
  }
  double mesh[ROWS][COLS];
  double fireplaceWidth = .4 * COLS;
  // Initialize the mesh to 20.0 degrees celsius, the edges will stay fixed at this temperature
  for (int i = 0; i < ROWS; i++) {
    for (int j = 0; j < COLS; j++) {
      mesh[i][j] = 20.0
    }
  }
  // Place the fireplace at the top of the room and set to 300.0 celsius
  int startIndex = (COLS - fireplaceWidth) / 2;
  for (int i = startIndex; i < fireplaceWidth; i++) {
    mesh[0][i] = 300.0
  }
  // Pseudo code
  // for each iteration {
  //   CopyNewToOld(new, old);
  //   CalculateNew(new, old, xsource, ysource);
  //   Optionally, PrintGridtoFile(new, xsource, ysource);
  // }
  return 0;
}

// function implementations
void CopyNewToOld(float new[][COLS], float old[][COLS]) {

}

void CalculateNew(float new[][COLS], float old[][COLS], int xsource, int ysource ) {
  // pseudo code
  // for (i = 1; i < n-1; i++)
  //   for (j = 1; j < n-1; j++)
  //     new[i][j] = 0.25*(old[i-1][j]+old[i+1][j]+old[i][j-1]+old[i][j+1]);
}

void PrintGrid(float grid[][COLS], int xsource, int ysource) {

}
