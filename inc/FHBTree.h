#ifndef _FHBTREE_H_
#define _FHBTREE_H_
#include "THBTree.h"
#include <map>

// FHBTree: the top level of hierarchy, each node is a THBTree
// Total placement tree

using namespace std;

class FHNode {
public:
    THBTree TH_Btree;
	Cell Cell_info;
	int id, parent, left_child, right_child;

	FHNode(){};
	~FHNode(){};
};

class FHBTree {
	public:
		vector<FHNode> FH_node;
		vector<int> FH_contour;
		int overall_width, overall_height;

		FHBTree(){};
		~FHBTree(){};

		void initial(vector<vector<vector<Node> > >);
		void output_tree(int);
		int show_root();
		void placement(int *, int *);
		double get_area();
		void clean_contour();
		void clean_overall_width_height();
		void perturb();
		void perturb_THBTtree();
		void perturb_HBTree(vector<int>&);
		void perturb_ASF_Tree(vector<int>&);
		void perturb_ASF_GuardRing();
		void update_final_coordinate();
		
		void show_HBTree();
		void show_all_HBTree();

		/*void show_contour();
		void show_HBTree();
		void show_HB_contour();*/

	private:
		int root;
		void set_coordinate(int, int *, int *);
		void move_node(int, int);
		void swap_node(int, int);
		/*void preorder(int);
		void place_contour(int);
		void set_contour(int, int, float *);
		void find_max_y(int, int *,float *);
		void insert_node(FHNode &, FHNode &);
		void delete_node(FHNode &);
		void swap_node(FHNode &, FHNode &);
		bool rand_bool();*/
};

#endif