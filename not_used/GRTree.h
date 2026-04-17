#ifndef _GRTREE_H_
#define _GRTREE_H_
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <list>
#include "parameter.h"
#include "BTree.h"

// Deal with multiple cells in one guard ring

class GRNode{
    public:
        Cell Cell_info;
        int id;
        int parent;
        int left_child;
        int right_child;

        GRNode(){};
        GRNode(BTree);
        GRNode(Cell);
        ~GRNode(){};
};

class GRTree{
    public:
        vector<GRNode> GRNodes;
        vector<int> GR_contour;

        GRTree(){};
        ~GRTree(){};

        void initial(vector<Node>);
        void output_tree(int);
        int show_root();
        void placement(int *, int *);
        void perturb();
        void clean_contour();
        void show_tree();
        void show_all_tree();

    private:
        int root;
        void set_coordinate(int, int *, int *);
        void move_node(int, int);
        void swap_node(int, int);
        void preorder(int);
        //void update_ASF_node_coordinate();

};

#endif