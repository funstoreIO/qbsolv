
#include "../src/include.h"
#include "../src/extern.h"

void reduce(int *Icompress, double **val, int subMatrix,int maxNodes,double **val_s,short *Q,short *Q_s);

FILE   *outFile_;
int    maxNodes_,nCouplers_,nNodes_,findMax_;
int    Verbose_,SubMatrix_,UseDwave_,TargetSet_,WriteMatrix_,Tlist_;
char   *outFileNm_,pgmName_[16];
double **val;
double Target_,Time_;
struct nodeStr_  *nodes_;
struct nodeStr_  *couplers_;

void test_eq(int a, int b){
    if(a == b){
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
}

void test_eqf(double a, double b){
    if(a == b){
        printf("PASS\n");
    } else {
        printf("FAIL\n");
    }
}

void testOne(){
    // -------------------------------------------------------------------------
    // TEST 1:  Extracting one conditioned variable from a two variable system
    printf("Single variable from two variable system:\n");

    // -- Bootstrap
    // Declare the full QUBO
    int maxNodes = maxNodes_ = 2;
    double ** quboMat = (double**)malloc2D(2, 2, sizeof(double));

    // Encode simple 2 variable system
    // E(a, b) = 2a + 2ab + 3b
    quboMat[0][0] = 2;
    quboMat[1][1] = 3;
    quboMat[0][1] = 2;

    // selection variables
    int selectionMapping[1];
    short globalState[2];

    // output variables
    short selectionState[1];
    maxNodes_ = 1;
    double ** selectionMat = (double**)malloc2D(1, 1, sizeof(double));

    // -- part 1a
    // E(a, b=0) = 2a + 2a(0) + 3(0)
    //           = 2a
    selectionMapping[0] = 0;
    globalState[0] = 0;
    globalState[1] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(2, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 1b, same but with change in initialization of selected variable
    // E(a, b=0) = 2a + 2a(0) + 3(0)
    //           = 2a
    selectionMapping[0] = 0;
    globalState[0] = 1;
    globalState[1] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(2, selectionMat[0][0]);
    test_eq(1, selectionState[0]);

    // -- part 2a
    // E(a, b=1) = 2a + 2a(1) + 3(1)
    //           = 4a + c
    selectionMapping[0] = 0;
    globalState[0] = 0;
    globalState[1] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(4, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 2b, same but with change in initialization of selected variable
    // E(a, b=1) = 2a + 2a(1) + 3(1)
    //           = 4a + c
    selectionMapping[0] = 0;
    globalState[0] = 1;
    globalState[1] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(4, selectionMat[0][0]);
    test_eq(1, selectionState[0]);

    // -- part 3a
    // E(a=0, b) = 2(0) + 2(0)b + 3b
    //           = 3b
    selectionMapping[0] = 1;
    globalState[0] = 0;
    globalState[1] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(3, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 3b, same but with change in initialization of selected variable
    selectionMapping[0] = 1;
    globalState[0] = 0;
    globalState[1] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(3, selectionMat[0][0]);
    test_eq(1, selectionState[0]);

    // -- part 4a
    // E(a=1, b) = 2(1) + 2(1)b + 3b
    //           = 5b + c
    selectionMapping[0] = 1;
    globalState[0] = 1;
    globalState[1] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(5, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 4b, same but with change in initialization of selected variable
    selectionMapping[0] = 1;
    globalState[0] = 1;
    globalState[1] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(5, selectionMat[0][0]);
    test_eq(1, selectionState[0]);
}

void testTwo(){
    // -------------------------------------------------------------------------
    // TEST 2:  Same tests as test 1 but with some unused variables inserted
    // in the beginning of the matrix, should be ignored.
    //
    // This test should run EXACTLY the same as test 1, but with the index
    // of `a` being 2, and the index of `b` being 3
    printf("Single variable from two variable system, extra variables:\n");

    // -- Bootstrap
    // Declare the full QUBO
    int maxNodes = maxNodes_ = 4;
    double ** quboMat = (double**)malloc2D(4, 4, sizeof(double));

    // Encode simple 2 variable system
    // E(a, b) = 2a + 2ab + 3b
    quboMat[2][2] = 2;
    quboMat[3][3] = 3;
    quboMat[2][3] = 2;

    // selection variables
    int selectionMapping[1];
    short globalState[4];
    globalState[0] = 0;
    globalState[1] = 0;

    // output variables
    short selectionState[1];
    maxNodes_ = 1;
    double ** selectionMat = (double**)malloc2D(1, 1, sizeof(double));

    // -- part 1a
    // E(a, b=0) = 2a + 2a(0) + 3(0)
    //           = 2a
    selectionMapping[0] = 2;
    globalState[2] = 0;
    globalState[3] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(2, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 1b, same but with change in initialization of selected variable
    // E(a, b=0) = 2a + 2a(0) + 3(0)
    //           = 2a
    selectionMapping[0] = 2;
    globalState[2] = 1;
    globalState[3] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(2, selectionMat[0][0]);
    test_eq(1, selectionState[0]);

    // -- part 2a
    // E(a, b=1) = 2a + 2a(1) + 3(1)
    //           = 4a + c
    selectionMapping[0] = 2;
    globalState[2] = 0;
    globalState[3] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(4, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 2b, same but with change in initialization of selected variable
    // E(a, b=1) = 2a + 2a(1) + 3(1)
    //           = 4a + c
    selectionMapping[0] = 2;
    globalState[2] = 1;
    globalState[3] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(4, selectionMat[0][0]);
    test_eq(1, selectionState[0]);

    // -- part 3a
    // E(a=0, b) = 2(0) + 2(0)b + 3b
    //           = 3b
    selectionMapping[0] = 3;
    globalState[2] = 0;
    globalState[3] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(3, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 3b, same but with change in initialization of selected variable
    selectionMapping[0] = 3;
    globalState[2] = 0;
    globalState[3] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(3, selectionMat[0][0]);
    test_eq(1, selectionState[0]);

    // -- part 4a
    // E(a=1, b) = 2(1) + 2(1)b + 3b
    //           = 5b + c
    selectionMapping[0] = 3;
    globalState[2] = 1;
    globalState[3] = 0;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(5, selectionMat[0][0]);
    test_eq(0, selectionState[0]);

    // -- part 4b, same but with change in initialization of selected variable
    selectionMapping[0] = 3;
    globalState[2] = 1;
    globalState[3] = 1;

    reduce(selectionMapping, quboMat, 1, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(5, selectionMat[0][0]);
    test_eq(1, selectionState[0]);
}


void testThree(){
    // -------------------------------------------------------------------------
    // TEST 3:  Test a slightly larger problem
    printf("Two variables from a five variable system\n");

    // -- Bootstrap
    // Declare the full QUBO
    int maxNodes = maxNodes_ = 5;
    double ** quboMat = (double**)malloc2D(5, 5, sizeof(double));

    // Encode simple 2 variable system
    // E(b) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 +
    //        b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 +
    //        5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    quboMat[0][0] = 1;
    quboMat[1][1] = 2;
    quboMat[2][2] = -3;
    quboMat[3][3] = 4;
    quboMat[4][4] = 2;

    quboMat[0][1] = 1;
    quboMat[0][2] = -1;
    quboMat[1][2] = 4;
    quboMat[1][3] = -2;
    quboMat[2][3] = 5;
    quboMat[2][4] = 1;
    quboMat[3][4] = 2;


    // selection variables
    int selectionMapping[2];
    short globalState[5];

    // output variables
    short selectionState[2];
    double ** selectionMat = (double**)malloc2D(2, 2, sizeof(double));

    // -- part 1
    // E([0, 0, ?, ?, 0]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (0) + 2(0) - 3b_2 + 4b_3 + 2(0) + (0) * (0) - (0) * b_2 + 4 * (0) * b_2 - 2 * (0) * b_3 + 5 * b_2 * b_3 + b_2 * (0) + 2 * b_3 * (0)
    //                    = -3b_2 + 4b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 0;
    globalState[1] = 0;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 0;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(-3, selectionMat[0][0]);
    test_eqf(4, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 2
    // E([0, 0, ?, ?, 1]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (0) + 2(0) - 3b_2 + 4b_3 + 2(1) + (0) * (0) - (0) * b_2 + 4 * (0) * b_2 - 2 * (0) * b_3 + 5 * b_2 * b_3 + b_2 * (1) + 2 * b_3 * (1)
    //                    = -2 b_2 + 6 b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 0;
    globalState[1] = 0;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 1;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(-2, selectionMat[0][0]);
    test_eqf(6, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 3
    // E([0, 1, ?, ?, 0]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (0) + 2(1) - 3b_2 + 4b_3 + 2(0) + (0) * (1) - (0) * b_2 + 4 * (1) * b_2 - 2 * (1) * b_3 + 5 * b_2 * b_3 + b_2 * (0) + 2 * b_3 * (0)
    //                    = 1 b_2 + 2b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 0;
    globalState[1] = 1;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 0;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(1, selectionMat[0][0]);
    test_eqf(2, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 4
    // E([0, 1, ?, ?, 1]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (0) + 2(1) - 3b_2 + 4b_3 + 2(1) + (0) * (1) - (0) * b_2 + 4 * (1) * b_2 - 2 * (1) * b_3 + 5 * b_2 * b_3 + b_2 * (1) + 2 * b_3 * (1)
    //                    = - 3b_2 + 4b_3 + 4 * b_2 - 2 * b_3 + 5 * b_2 * b_3 + b_2 + 2 * b_3
    //                    = 2 * b_2 + 4b_3 + 5 * b_2 * b_3

    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 0;
    globalState[1] = 1;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 1;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(2, selectionMat[0][0]);
    test_eqf(4, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 5
    // E([1, 0, ?, ?, 0]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (1) + 2(0) - 3b_2 + 4b_3 + 2(0) + (1) * (0) - (1) * b_2 + 4 * (0) * b_2 - 2 * (0) * b_3 + 5 * b_2 * b_3 + b_2 * (0) + 2 * b_3 * (0)
    //                    = - 4b_2 + 4b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 1;
    globalState[1] = 0;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 0;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(-4, selectionMat[0][0]);
    test_eqf(4, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 6
    // E([1, 0, ?, ?, 1]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (1) + 2(0) - 3b_2 + 4b_3 + 2(1) + (1) * (0) - (1) * b_2 + 4 * (0) * b_2 - 2 * (0) * b_3 + 5 * b_2 * b_3 + b_2 * (1) + 2 * b_3 * (1)
    //                    = - 3b_2 + 6b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 1;
    globalState[1] = 0;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 1;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(-3, selectionMat[0][0]);
    test_eqf(6, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 7
    // E([1, 1, ?, ?, 0]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (1) + 2(1) - 3b_2 + 4b_3 + 2(0) + (1) * (1) - (1) * b_2 + 4 * (1) * b_2 - 2 * (1) * b_3 + 5 * b_2 * b_3 + b_2 * (0) + 2 * b_3 * (0)
    //                    = -0b_2 + 2b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 1;
    globalState[1] = 1;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 0;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(0, selectionMat[0][0]);
    test_eqf(2, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);

    // -- part 8
    // E([1, 1, ?, ?, 1]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (1) + 2(1) - 3b_2 + 4b_3 + 2(1) + (1) * (1) - (1) * b_2 + 4 * (1) * b_2 - 2 * (1) * b_3 + 5 * b_2 * b_3 + b_2 * (1) + 2 * b_3 * (1)
    //                    = -3 b_2 + 4b_3 - b_2 + 4 * b_2 - 2 * b_3 + 5 * b_2 * b_3 + b_2 + 2 * b_3
    //                    = +1 b_2 + 4 b_3 + 5 * b_2 * b_3
    selectionMapping[0] = 2;
    selectionMapping[1] = 3;
    globalState[0] = 1;
    globalState[1] = 1;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 1;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(1, selectionMat[0][0]);
    test_eqf(4, selectionMat[1][1]);
    test_eqf(5, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);
}

void testFour(){
    // -------------------------------------------------------------------------
    // TEST 3:  Test a slightly larger problem
    printf("Non-contiguous, two variables from a five variable system\n");

    // -- Bootstrap
    // Declare the full QUBO
    int maxNodes = maxNodes_ = 5;
    double ** quboMat = (double**)malloc2D(5, 5, sizeof(double));

    // Encode simple 2 variable system
    // E(b) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 +
    //        b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 +
    //        5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    quboMat[0][0] = 1;
    quboMat[1][1] = 2;
    quboMat[2][2] = -3;
    quboMat[3][3] = 4;
    quboMat[4][4] = 2;

    quboMat[0][1] = 1;
    quboMat[0][2] = -1;
    quboMat[1][2] = 4;
    quboMat[1][3] = -2;
    quboMat[2][3] = 5;
    quboMat[2][4] = 1;
    quboMat[3][4] = 2;


    // selection variables
    int selectionMapping[2];
    short globalState[5];

    // output variables
    short selectionState[2];
    double ** selectionMat = (double**)malloc2D(2, 2, sizeof(double));

    // -- part 8
    // E([1, ?, 1, ?, 1]) = b_0 + 2b_1 - 3b_2 + 4b_3 + 2b_4 + b_0 * b_1 - b_0 * b_2 + 4 * b_1 * b_2 - 2 * b_1 * b_3 + 5 * b_2 * b_3 + b_2 * b_4 + 2 * b_3 * b_4
    //                    = (1) + 2b_1 - 3(1) + 4b_3 + 2(1) + (1) * b_1 - (1) * (1) + 4 * b_1 * (1) - 2 * b_1 * b_3 + 5 * (1) * b_3 + (1) * (1) + 2 * b_3 * (1)
    //                    = 2b_1 + 4b_3 + b_1 + 4 * b_1 - 2 * b_1 * b_3 + 5 b_3 + 2 * b_3
    //                    = 7b_1 + 11b_3 - 2 * b_1 * b_3

    selectionMapping[0] = 1;
    selectionMapping[1] = 3;
    globalState[0] = 1;
    globalState[1] = 1;
    globalState[2] = 1;
    globalState[3] = 1;
    globalState[4] = 1;

    reduce(selectionMapping, quboMat, 2, maxNodes, selectionMat, globalState, selectionState);

    test_eqf(7, selectionMat[0][0]);
    test_eqf(11, selectionMat[1][1]);
    test_eqf(-2, selectionMat[0][1] + selectionMat[1][0]);

    test_eq(1, selectionState[0]);
    test_eq(1, selectionState[1]);
}


int main(int argc, char *argv[]){

    testOne();
    testTwo();
    testThree();
    testFour();

    //
    return 0;
}
