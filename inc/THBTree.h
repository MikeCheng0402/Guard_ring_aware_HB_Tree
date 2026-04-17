#ifndef _THBTREE_H_
#define _THBTREE_H_
#include "BTree.h"
#include "HBTree.h"

// THBTree: the 2nd top level of hierarchy, each node is a HBTree
// Each THNode is a H-B* Tree, represents a proximity group of blocks

class THNode {
public:
	HBTree H_BTree;
	Cell Cell_info;
	int id, parent, left_child, right_child;

	THNode(){};
	~THNode(){};
};

class THBTree {
	public:
		THBTree(){};
		~THBTree(){};

		vector<THNode> TH_node;
		vector<int> TH_contour;

		void initial(vector<vector<Node> >);
		void output_tree(int);
		int show_root();
		void placement(int *, int *);
		void perturb();
		void perturb_HBTree(vector<int>&);
		void perturb_ASF_Tree(vector<int>&);
		// void perturb_ASF_GuardRing();

		void show_tree();
        void show_all_tree();
		/*void show_contour();
		void show_HBTree();
		void show_HB_contour();*/

	private:
		int root;
		void set_coordinate(int, int *, int *);
		void move_node(int, int);
		void swap_node(int, int);
		void clean_contour();
		/*void preorder(int);
		void place_contour(int);
		void set_contour(int, int, float *);
		void find_max_y(int, int *,float *);
		bool rand_bool();*/
};

#endif