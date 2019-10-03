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

}

void PrintGrid(float grid[][COLS], int xsource, int ysource) {

}
