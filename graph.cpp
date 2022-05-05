#include "graph.h"

using namespace std;




void Tree_Graph() {
    int type, from, to;
    TreeNode tree_root = TreeNode(from = Graph_line[0][1]);
    tree_root.to_num++;
    for (int i = 1; i < edges; i++) {
        type = Graph_line[i][0]; 
        from = Graph_line[i][1]; 

        to = Graph_line[i][2];
        //TreeNode tree_cur = TreeNode()
        //if(tree_root.val == to) 

        if (type == 0)
            adjacent_matrix[from][to] = 1;
        if (type == 1)
            adjacent_matrix[from][to] = 2;
    }
}