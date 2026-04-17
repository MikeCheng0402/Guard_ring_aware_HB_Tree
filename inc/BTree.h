#ifndef _BTREE_H_
#define _BTREE_H_
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <list>
#include "parameter.h"

#define SYMMETRY 1

using namespace std;

class Cell{
    public:
        string name;
        string drain, gate, source, body, type;
        int x, y;
        int fin, l, m;
        int width, length;
        bool symmetry;
        bool common;
        friend class BTree;
        friend class Node;


        Cell(){}
        Cell(string, string, string, string, string, string, int, int, int, bool, bool);
        ~Cell(){};
};

class Node{
    public:
        Cell Cell_info;
        int id;
        int parent;
        int left_child;
        int right_child;
        bool parent_weight, left_weight, right_weight;
        Node(){};
        Node(int, Cell);
        ~Node(){};
        string get_cell_name();
        void output_info();
};

class BTree{
    public:
        vector<Node> nodes;
        vector<int> contour;

        BTree(){};
        BTree(vector<Node> a):nodes(a){}
        ~BTree(){};
        void output_data();
        void initial(vector<Node>);
        void output_tree(int);
        int show_root();
        void placement(int *, int *);
        void symmetry_placement(int *, int *);
        vector<int> get_contour();
        void clean_contour();
        void perturb();
        // void perturb_GuardRing();
        void show_tree();
        void show_all_tree();

    private:
        int root;
        void set_coordinate(int, int *, int *);
        void set_coordinate2(int, int *, int *, bool, int *, int *, int *);
        void move_node(int, int);
        void swap_node(int, int);
        void update_symmetry_coordinate(int);
        void preorder(int);
};


inline map<string, int> id_check;
inline vector<Node> All_Node;
inline map<string, int> symmetry_check;
inline vector<vector<vector<string> > > proximity_group;
inline map<string, string> symmetry_group;
inline int number_of_symmetry_node;
inline int current_op;
inline int *left_check;
inline map<string, pair<pair<int, int>, pair<int, int> > > guard_ring_info;
//inline vector<pair<pair<int, int>, pair<int, int> > > guard_ring_info;

#endif