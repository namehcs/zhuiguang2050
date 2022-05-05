#pragma once
#include "data.h"

using namespace std;

struct TreeNode {
    int val;
    int to_num = 0;
    int** to[];
    TreeNode(int val):val(val){}
};

//0:不通，1：通，2：可同时开始加工
vector<vector<int>> adjacent_matrix(devices, vector<int>(devices, 0));

int edges = 15;
int Graph_line[15][3] = { {0, 0, 1},
                         {0, 1, 6},
                         {1, 6, 2},
                         {0, 2, 7},
                         {0, 7, 5},
                         {0, 5, 9},
                         {0, 7, 4},
                         {0, 4, 5},
                         {0, 1, 10},
                         {0, 10, 3},
                         {1, 3, 8},
                         {0, 8, 5},
                         {0, 5, 12},
                         {0, 12, 11},
                         {0, 10, 8}, };


