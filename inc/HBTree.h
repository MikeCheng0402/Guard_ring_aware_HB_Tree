#ifndef _HBTREE_H_
#define _HBTREE_H_
#include "BTree.h"

// HBTree: the 3rd top level of hierarchy, records all types of H-B* Tree nodes
// ASF = Automatically Symmetric-Feasible

class HNode{
    public:
        BTree ASF_BTree;
        Cell Cell_info;
        int id;
        int parent;
        int left_child;
        int right_child;
        bool ASF_or_not;
        bool Contour_or_not;

        HNode(){};
        HNode(BTree);
        HNode(Cell);
        ~HNode(){};
};

class HBTree{
    public:
        vector<HNode> Hnodes;
        vector<HNode> contour_nodes;
        vector<int> H_contour;

        HBTree(){};
        ~HBTree(){};

        bool ASF_node_empty;
        void initial(vector<Node>);
        void output_tree(int);
        int show_root();
        void placement(int *, int *);
        void perturb();
        void perturb_ASF_Tree();
        // void perturb_ASF_GuardRing();
        void clean_contour();
        void show_tree();
        void show_all_tree();

    private:
        int root;
        int ASF_node_id;
        void initial_ASF_Node(vector<Node>);
        void update_tree();
        void update_contour_node();
        void set_coordinate(int, int *, int *);
        void insert_node(int, int);
	    void delete_node(int);
        void move_node(int, int);
        void swap_hierarchy_node(int, int);
        void swap_node(int, int);
        bool check_hierarchy_swap(int, int);
        void update_contour_node_coordinate();
        void preorder(int);
        //void update_ASF_node_coordinate();

};

#endif
