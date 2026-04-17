#include <iostream>
#include "HBTree.h"

using namespace std;

HNode::HNode(BTree asf_btree)
{
    id = 0;
    ASF_BTree = asf_btree;
    parent = -1;
    left_child = -1;
    right_child = -1;
    ASF_or_not = 1;
}

HNode::HNode(Cell cell)
{
    id = 0;
    Cell_info = cell;
    parent = -1;
    left_child = -1;
    right_child = -1;
    ASF_or_not = 0;
}

void HBTree::initial(vector<Node> data_in)
{
    HNode tmp_Hnode;
	vector<Node> ASF_node;
	ASF_node_id = -1; //assume no hierarchy node
	ASF_node_empty = 1;

	// cout << "The Guard ring begin ";

	// cout << data_in.size() << endl;
	for(int i=0; i<data_in.size(); i++) 
    {
		// cout << "Begin HBTree initial " << i << endl;
		if(data_in[i].Cell_info.symmetry == SYMMETRY)
			ASF_node.push_back(data_in[i]);
		else 
        {
			// cout << "node name = " << data_in[i].Cell_info.name << endl;
			tmp_Hnode.Cell_info.name = data_in[i].Cell_info.name;
			tmp_Hnode.Cell_info.width = data_in[i].Cell_info.width;
			tmp_Hnode.Cell_info.length = data_in[i].Cell_info.length;
			tmp_Hnode.ASF_or_not = 0;
			tmp_Hnode.Contour_or_not = 0;
			Hnodes.push_back(tmp_Hnode);
		}
	}
	if(!ASF_node.empty()) {
		ASF_node_empty = 0;
		initial_ASF_Node(ASF_node);
	}
	update_tree();
}

void HBTree::output_tree(int node_id) 
{
	//cout << node_id << " " << nodes[node_id].Cell_info.name;
    //cout << " left=" << nodes[node_id].left_child << " right=" << nodes[node_id].right_child << " x=" << nodes[node_id].Cell_info.x << " y=" << nodes[node_id].Cell_info.y << endl;
	//cout << "length=" << nodes[node_id].Cell_info.length << " width=" << nodes[node_id].Cell_info.width << endl;
    //cout << "***************************************" << endl;
	
	if(Hnodes[node_id].ASF_or_not != 1)
	{
		cout << node_id << " " << Hnodes[node_id].Cell_info.name;
    	cout << " left=" << Hnodes[node_id].left_child << " right=" << Hnodes[node_id].right_child << " x=" << Hnodes[node_id].Cell_info.x << " y=" << Hnodes[node_id].Cell_info.y << endl;
	}
	else
	{
		Hnodes[node_id].ASF_BTree.output_tree(Hnodes[node_id].ASF_BTree.show_root());
	}

	if(Hnodes[node_id].left_child != -1) 
    {
		output_tree(Hnodes[node_id].left_child);
	}
	if(Hnodes[node_id].right_child != -1) 
    {
		output_tree(Hnodes[node_id].right_child);
	}
}

int HBTree::show_root()
{
	return root;
}

void HBTree::placement(int *total_width, int *total_length) 
{
	clean_contour();
	set_coordinate(root, total_width, total_length);
}

void HBTree::perturb()
{
	int option;
	int block_num;
	int node1 = -1;
	int node2 = -1;
	
	option = bool(rand()%2);
	block_num = Hnodes.size();

	if(block_num == 1 || block_num-contour_nodes.size() == 1)
        return;

	while(node1 == node2) 
	{
		node1 = rand()%block_num;
		node2 = rand()%block_num;
	}

	if(option) //move
	{
		// cout << "Do move" << endl;

		bool move_illegal = 0;

		if(Hnodes[node1].ASF_or_not == 1 && Hnodes[node1].Contour_or_not == 0) 
		{
			move_illegal = check_hierarchy_swap(node1, node2);
		} 
		else if(Hnodes[node2].ASF_or_not == 1 &&  Hnodes[node2].Contour_or_not == 0) 
		{
			move_illegal = check_hierarchy_swap(node2, node1);
		}
		if(Hnodes[node1].Contour_or_not == 1 || move_illegal == 1)
			return;

		move_node(node1, node2);
	} 
	else //swap
	{ 
		while(Hnodes[node1].Contour_or_not == 1 || node1 == node2)
			node1 = rand()%block_num;

		while(Hnodes[node2].Contour_or_not == 1 || node1 == node2)
			node2 = rand()%block_num;

		if(Hnodes[node1].ASF_or_not == 1)
			swap_hierarchy_node(node2, node1);
		else if(Hnodes[node2].ASF_or_not == 1)
			swap_hierarchy_node(node1, node2);
		else
			swap_node(node1, node2);
	}
}

void HBTree::perturb_ASF_Tree() 
{
	Hnodes[ASF_node_id].ASF_BTree.perturb();
	Hnodes[ASF_node_id].ASF_BTree.clean_contour();
	Hnodes[ASF_node_id].ASF_BTree.placement(&Hnodes[ASF_node_id].Cell_info.width, &Hnodes[ASF_node_id].Cell_info.length);
	Hnodes[ASF_node_id].ASF_BTree.symmetry_placement(&Hnodes[ASF_node_id].Cell_info.width, &Hnodes[ASF_node_id].Cell_info.length);

	for(int i=Hnodes.size(); i>ASF_node_id+1; i--)	//clean the contour for updating contour
		Hnodes.pop_back();
	
	update_contour_node();	
	update_tree();
}

void HBTree::clean_contour()
{
	H_contour.clear();
}

void HBTree::show_tree() 
{
	preorder(root);
}

void HBTree::show_all_tree()
{
	for(int i=0; i<Hnodes.size(); i++)
	{
		cout << Hnodes[i].Cell_info.name << " id=" << i << endl;
		cout << "parent=" << Hnodes[i].parent << " left=" << Hnodes[i].left_child << " right=" << Hnodes[i].right_child << " x=" << Hnodes[i].Cell_info.x << " y=" << Hnodes[i].Cell_info.y << endl;
	}
}

////////////////////private

void HBTree::initial_ASF_Node(vector<Node> ASF_node) 
{
	HNode tmp_Hnode;
	//vector<Contour> ASF_top_contour;
	vector<HNode> contour_node;

	tmp_Hnode.Cell_info.name = "group1"; //push hierarchy node
	tmp_Hnode.Cell_info.length = 0;
	tmp_Hnode.ASF_or_not = 1;
	tmp_Hnode.Contour_or_not = 0;

	Hnodes.push_back(tmp_Hnode);
	ASF_node_id = Hnodes.size()-1;
	Hnodes.back().ASF_BTree.initial(ASF_node); //initial hierarchy node
	Hnodes.back().Cell_info.width = 0;
	Hnodes.back().Cell_info.length = 0;
	Hnodes.back().ASF_BTree.placement(&Hnodes.back().Cell_info.width, &Hnodes.back().Cell_info.length); //placement hierarchy node
	Hnodes.back().ASF_BTree.symmetry_placement(&Hnodes.back().Cell_info.width, &Hnodes.back().Cell_info.length);

	update_contour_node();
}

void HBTree::update_tree()
{
	for(int i=0; i<Hnodes.size(); i++) 
	{
	    Hnodes[i].id = i;
	    Hnodes[i].Cell_info.x = 0;
	    Hnodes[i].Cell_info.y = 0;
	    if(Hnodes[i].Contour_or_not != 1) 
		{
		    Hnodes[i].parent = (i+1)/2-1;
		    Hnodes[i].left_child = (2*i+1 < Hnodes.size()-contour_nodes.size() ? 2*i+1 : -1);
		    Hnodes[i].right_child = (2*i+2 < Hnodes.size()-contour_nodes.size() ? 2*i+2 : -1);
		} 
		else 
		{ /// set contour node
			if(Hnodes[i-1].ASF_or_not == 1 && Hnodes[i-1].Contour_or_not ==0)
			    Hnodes[i-1].right_child = i;

			Hnodes[i].parent = i-1;
			Hnodes[i].left_child = (i+1 < Hnodes.size() ? i+1 : -1);
			Hnodes[i].right_child = -1;
		}
	}
	Hnodes[0].parent = -1;
	root = Hnodes[0].id;
}

void HBTree::update_contour_node()
{
	contour_nodes.clear();

	vector<int> contour_tmp;
	contour_tmp = Hnodes.back().ASF_BTree.get_contour();

	int y_max = contour_tmp[0];
	int contour_width = 0;

	for(int i=0; i<contour_tmp.size(); i++)
	{
		if(y_max != contour_tmp[i])
		{
			HNode contour_node_tmp;
			contour_node_tmp.Contour_or_not = 1;
			contour_node_tmp.ASF_or_not = 1;
			contour_node_tmp.Cell_info.width = contour_width;
			contour_node_tmp.Cell_info.x = i - contour_width;
			contour_node_tmp.Cell_info.y = y_max;
			contour_node_tmp.Cell_info.name = "contour_node";

			Hnodes.push_back(contour_node_tmp);
			contour_nodes.push_back(Hnodes.back());

			y_max = contour_tmp[i];
			contour_width = 0;
		}

		contour_width++;
	}

	HNode contour_node_tmp;
	contour_node_tmp.Contour_or_not = 1;
	contour_node_tmp.ASF_or_not = 1;
	contour_node_tmp.Cell_info.width = contour_width;
	contour_node_tmp.Cell_info.x = contour_tmp.size() - contour_width;
	contour_node_tmp.Cell_info.y = y_max;
	contour_node_tmp.Cell_info.name = "contour_node";

	Hnodes.push_back(contour_node_tmp);
	contour_nodes.push_back(Hnodes.back());

	return;
}

void HBTree::set_coordinate(int node_id, int *total_width, int *total_length)
{
	// int SPACE = CELL_DISTANCE;
    int SPACE_V = 2*WELL_TO_GUARD_RING + 2*GUARD_RING_HEIGHT + GR_TO_GR;
    int SPACE_H = 2*WELL_TO_GUARD_RING + 2*GUARD_RING_WIDTH + GR_TO_GR;

    int parent_id = Hnodes[node_id].parent;

	if(Hnodes[node_id].ASF_or_not == 0) //deal with normal node
	{
		// cout << "deal with the normal node" << endl;
		if(node_id == root)
		{
			Hnodes[node_id].Cell_info.x = 0;
		}
		else if(node_id == Hnodes[parent_id].left_child)
		{
			if(Hnodes[parent_id].ASF_or_not == 1)
				Hnodes[node_id].Cell_info.x = Hnodes[parent_id].Cell_info.x + Hnodes[parent_id].Cell_info.width + GR_TO_GR;
			else
				Hnodes[node_id].Cell_info.x = Hnodes[parent_id].Cell_info.x + Hnodes[parent_id].Cell_info.width + SPACE_H;
		}
		else if(node_id == Hnodes[parent_id].right_child)
		{
			Hnodes[node_id].Cell_info.x = Hnodes[parent_id].Cell_info.x;
		}

		int contour_begin = Hnodes[node_id].Cell_info.x;
		int contour_end = Hnodes[node_id].Cell_info.x + Hnodes[node_id].Cell_info.width + SPACE_H;
		int lower_y = 0;

		while(H_contour.size() < contour_end + 1)
		{
			H_contour.push_back(0);
		}

		for(int i = contour_begin; i < contour_end; i++)
		{
			if(lower_y < H_contour[i])
				lower_y = H_contour[i];
		}

		Hnodes[node_id].Cell_info.y = lower_y;

		for(int i = contour_begin; i < contour_end; i++)
			H_contour[i] = Hnodes[node_id].Cell_info.y + Hnodes[node_id].Cell_info.length + SPACE_V;

		if(*total_width < Hnodes[node_id].Cell_info.x + Hnodes[node_id].Cell_info.width + SPACE_H - GR_TO_GR)
			*total_width = Hnodes[node_id].Cell_info.x + Hnodes[node_id].Cell_info.width + SPACE_H - GR_TO_GR;
		if(*total_length < Hnodes[node_id].Cell_info.y + Hnodes[node_id].Cell_info.length + SPACE_V - GR_TO_GR)
			*total_length = Hnodes[node_id].Cell_info.y + Hnodes[node_id].Cell_info.length + SPACE_V - GR_TO_GR;

	}
	else //deal with ASFBtree node
	{
		if(Hnodes[node_id].Contour_or_not == 0)
		{
			// cout << "deal with  ASF node" << endl;
			
			if(node_id == root)
			{
				Hnodes[node_id].Cell_info.x = 0;
			}
			else if(node_id == Hnodes[parent_id].left_child)
			{
				Hnodes[node_id].Cell_info.x =Hnodes[parent_id].Cell_info.x + Hnodes[parent_id].Cell_info.width + SPACE_H;
			}
			else if(node_id == Hnodes[parent_id].right_child)
			{
				Hnodes[node_id].Cell_info.x =Hnodes[parent_id].Cell_info.x;
			}

			int contour_begin = Hnodes[node_id].Cell_info.x;
			int contour_end = Hnodes[node_id].Cell_info.x + Hnodes[node_id].Cell_info.width + SPACE_H;
			int lower_y = 0;

			while(H_contour.size() < contour_end + 1)
			{
				H_contour.push_back(0);
			}

			for(int i = contour_begin; i < contour_end; i++)
			{
				if(lower_y < H_contour[i])
					lower_y = H_contour[i];
			}

			Hnodes[node_id].Cell_info.y = lower_y;

			for(int i = contour_begin; i < contour_end; i++)
			{
				if(i-contour_begin <= Hnodes[node_id].Cell_info.width)
					H_contour[i] = Hnodes[node_id].Cell_info.y + Hnodes[node_id].ASF_BTree.get_contour()[i-contour_begin] + GR_TO_GR;
				else
					H_contour[i] = Hnodes[node_id].Cell_info.y + Hnodes[node_id].ASF_BTree.get_contour().back() + GR_TO_GR;
			}

			if(*total_width < Hnodes[node_id].Cell_info.x + Hnodes[node_id].Cell_info.width)
				*total_width = Hnodes[node_id].Cell_info.x + Hnodes[node_id].Cell_info.width;
			if(*total_length < Hnodes[node_id].Cell_info.y + Hnodes[node_id].Cell_info.length)
				*total_length = Hnodes[node_id].Cell_info.y + Hnodes[node_id].Cell_info.length;
		}
	}

    if(Hnodes[node_id].left_child != -1)
		set_coordinate(Hnodes[node_id].left_child, total_width, total_length);
	if(Hnodes[node_id].right_child != -1)
		set_coordinate(Hnodes[node_id].right_child, total_width, total_length);

    return;
}

void HBTree::insert_node(int parent_id, int node_id) 
{
	Hnodes[node_id].parent = Hnodes[parent_id].id;
	bool option = bool(rand()%2);
	int tmp_node_id = -1;

	if(Hnodes[parent_id].ASF_or_not == 1 && Hnodes[parent_id].Contour_or_not == 0)		//parent is hierarchy node
		option = 1;
	else if(Hnodes[parent_id].ASF_or_not == 1 && Hnodes[parent_id].Contour_or_not == 1)	//parent is contour node
		option = 0;

	if(option)	//add node to left
	{
		Hnodes[node_id].left_child = Hnodes[parent_id].left_child;
		if(Hnodes[node_id].ASF_or_not == 0) 
			Hnodes[node_id].right_child = -1;
		
		if(Hnodes[parent_id].left_child != -1) 
			Hnodes[Hnodes[parent_id].left_child].parent = Hnodes[node_id].id;
		
		Hnodes[parent_id].left_child = Hnodes[node_id].id;
	} 
	else	//add node to right
	{
		if(Hnodes[node_id].ASF_or_not == 0) 	//not hierarchy node
		{
			Hnodes[node_id].right_child = Hnodes[parent_id].right_child;
			tmp_node_id = parent_id;
			Hnodes[node_id].left_child = -1;
		} 
		else 
		{
			tmp_node_id = node_id;
			
			while(Hnodes[tmp_node_id].right_child != -1)
				tmp_node_id = Hnodes[tmp_node_id].right_child;
		
			Hnodes[tmp_node_id].right_child = Hnodes[parent_id].right_child;
			Hnodes[node_id].left_child = -1;
		}
		if(Hnodes[parent_id].right_child != -1 && Hnodes[node_id].ASF_or_not == 0)
			Hnodes[Hnodes[parent_id].right_child].parent = Hnodes[node_id].id;
		else if(Hnodes[tmp_node_id].right_child != -1 && Hnodes[node_id].ASF_or_not == 1)
			Hnodes[Hnodes[tmp_node_id].right_child].parent = tmp_node_id;
		
		Hnodes[parent_id].right_child = Hnodes[node_id].id;
	}
}

void HBTree::delete_node(int node_id) 
{
	bool is_leaf = false;
	int child_id = -1;
	int subchild_id = -1;
	int subparent_id = -1;

	if(Hnodes[node_id].Contour_or_not == 1)	//contour node
	{ 
		return;
	} 
	else	//normal node or hierarchy node
	{ 
		if(Hnodes[node_id].left_child == -1 && Hnodes[node_id].right_child == -1) 
			is_leaf = true;

		if(is_leaf != true) 
		{
			bool left = bool(rand()%2);
			if(Hnodes[node_id].left_child == -1 || Hnodes[Hnodes[node_id].left_child].ASF_or_not == 1) 
				left = false;
			if(Hnodes[node_id].right_child == -1) 
				left = true;			

			if(Hnodes[node_id].ASF_or_not == 1 && Hnodes[node_id].Contour_or_not == 0) 	//hierarchy node
			{ 
				if(Hnodes[node_id].left_child != -1)
					child_id = Hnodes[node_id].left_child;
			} 
			else if(left) 
			{
				child_id = Hnodes[node_id].left_child;
				if(Hnodes[node_id].right_child != -1) 
				{
					subchild_id = Hnodes[child_id].right_child;
					subparent_id = Hnodes[node_id].right_child;
					Hnodes[subparent_id].parent = child_id;
					Hnodes[child_id].right_child = subparent_id;
				}
			} 
			else 
			{
				child_id = Hnodes[node_id].right_child;
				if(Hnodes[node_id].left_child != -1) 
				{
					subchild_id = Hnodes[child_id].left_child;
					subparent_id = Hnodes[node_id].left_child;
					Hnodes[Hnodes[node_id].left_child].parent = child_id;
					Hnodes[child_id].left_child = Hnodes[node_id].left_child;
				}
			}

			if(child_id != -1)
				Hnodes[child_id].parent = Hnodes[node_id].parent;
		}
	
		if(Hnodes[node_id].parent == -1) 
		{	
			root = Hnodes[child_id].id;
		} 
		else 
		{
			if(Hnodes[node_id].id == Hnodes[Hnodes[node_id].parent].left_child) 
			{
				Hnodes[Hnodes[node_id].parent].left_child = child_id;
			} 
			else 
			{
				Hnodes[Hnodes[node_id].parent].right_child = child_id;
			}
		}
	
		if(subchild_id != -1) 
		{
			while(1) 
			{
				if((Hnodes[subparent_id].left_child == -1 && Hnodes[subparent_id].Contour_or_not == 0) || Hnodes[subparent_id].right_child == -1) 
				{
					Hnodes[subchild_id].parent = Hnodes[subparent_id].id;
					if(Hnodes[subparent_id].left_child == -1) 
					{
						Hnodes[subparent_id].left_child = Hnodes[subchild_id].id;
					} 
					else 
					{
						Hnodes[subparent_id].right_child = Hnodes[subchild_id].id;
					}
					break;
				} 
				else if((Hnodes[subparent_id].left_child == -1 && Hnodes[subparent_id].Contour_or_not == 1)) 
				{
					subparent_id = Hnodes[subparent_id].right_child;
				} 
				else 
				{
					subparent_id = (bool(rand()%2) ? Hnodes[subparent_id].left_child : Hnodes[subparent_id].right_child);
				}
			}
		}
	}
}

void HBTree::move_node(int node1, int node2)
{
	delete_node(node1);
	insert_node(node2, node1);
}

void HBTree::swap_hierarchy_node(int node1, int node2)		//node2 is ASF node
{
	int parent1_tmp = Hnodes[node1].parent;
    int left1_tmp = Hnodes[node1].left_child;
    int right1_tmp = Hnodes[node1].right_child;

    int parent2_tmp = Hnodes[node2].parent;
    int left2_tmp = Hnodes[node2].left_child;
    int right2_tmp = Hnodes[node2].right_child;

	if(Hnodes[node1].left_child == node2)
	{
		Hnodes[node1].left_child = left2_tmp;
		if(Hnodes[node2].left_child != -1)
			Hnodes[left2_tmp].parent = node1;
		
		Hnodes[node2].left_child = node1;
		Hnodes[node2].parent = parent1_tmp;

		if(Hnodes[node1].parent != -1)
		{
			if(node1 == Hnodes[parent1_tmp].left_child)
				Hnodes[parent1_tmp].left_child = node2;
			else
				Hnodes[parent1_tmp].right_child = node2;
		}
		else 
		{
			root = node2;
		}
		Hnodes[node1].parent = node2;
	}
	else if(Hnodes[node1].right_child == node2)
	{
		Hnodes[node1].right_child = left2_tmp;
		if(Hnodes[node2].left_child != -1)
			Hnodes[left2_tmp].parent = node1;
		
		Hnodes[node2].left_child = node1;
		Hnodes[node2].parent = parent1_tmp;

		if(Hnodes[node1].parent != -1)
		{
			if(node1 == Hnodes[parent1_tmp].left_child)
				Hnodes[parent1_tmp].left_child = node2;
			else
				Hnodes[parent1_tmp].right_child = node2;
		}
		else 
		{
			root = node2;
		}
		Hnodes[node1].parent = node2;
	}
	else if (Hnodes[node2].left_child == node1) 
	{
		if(Hnodes[node1].left_child != -1)
			Hnodes[left1_tmp].parent = node2;

		Hnodes[node2].left_child = left1_tmp;


		if(Hnodes[node2].parent != -1) 
		{
			if(node2 == Hnodes[parent2_tmp].right_child)
				Hnodes[parent2_tmp].right_child = node1;
			else
				Hnodes[parent2_tmp].left_child = node1;
			
		} 
		else
		{
			root = node1;
		}
		Hnodes[node1].parent = parent2_tmp;
		Hnodes[node1].left_child = node2;
		Hnodes[node2].parent = node1;
	}
	else if(check_hierarchy_swap(node1, node2) == 1)
	{
		if(Hnodes[node1].left_child != -1)
			Hnodes[left1_tmp].parent = node2;

		if(Hnodes[node2].left_child != -1)
			Hnodes[left2_tmp].parent = node1;

		if(Hnodes[node1].parent != -1) 
		{
			if(Hnodes[parent1_tmp].right_child == node1)
				Hnodes[parent1_tmp].right_child = node2;
			else
				Hnodes[parent1_tmp].left_child = node2;
		} 
		else 
		{
			root = node2;
		}


		if(Hnodes[parent2_tmp].right_child == node2)
			Hnodes[parent2_tmp].right_child = -1;	//change hieararchy node's parent right child
		else
			Hnodes[parent2_tmp].left_child = -1;

		Hnodes[node2].parent = parent1_tmp;

		int tmp_node;
	    tmp_node = node2;
	    
	    while(Hnodes[tmp_node].right_child != -1)
		    tmp_node = Hnodes[tmp_node].right_child;

		Hnodes[tmp_node].right_child = node1;
		Hnodes[node1].parent = tmp_node;

		Hnodes[node1].left_child = left2_tmp;
		Hnodes[node2].left_child = left1_tmp;
	}
	else if(check_hierarchy_swap(node2, node1) == 1)
	{
		if(Hnodes[node1].right_child != -1)
		{
			Hnodes[right1_tmp].parent = parent1_tmp;
		    if(Hnodes[parent1_tmp].right_child == node1)
			   Hnodes[parent1_tmp].right_child = right1_tmp;
		    else
		       Hnodes[parent1_tmp].left_child = right1_tmp;
		    
			if(Hnodes[node1].left_child != -1) 
			{
				int tmp_node;
				tmp_node = right1_tmp;

				while(Hnodes[tmp_node].left_child != -1)
					tmp_node = Hnodes[tmp_node].left_child;

				Hnodes[left1_tmp].parent = tmp_node;
				Hnodes[tmp_node].left_child = left1_tmp;
			}
		}
		else 
		{
			if(Hnodes[node1].left_child != -1)
				Hnodes[left1_tmp].parent = parent1_tmp;
			
			if(Hnodes[parent1_tmp].right_child == node1)
				Hnodes[parent1_tmp].right_child = left1_tmp;
			else
				Hnodes[parent1_tmp].left_child = left1_tmp;
			
			if(Hnodes[node1].right_child != -1) 
			{
				int tmp_node;
				tmp_node = left1_tmp;
				while(Hnodes[tmp_node].right_child != -1)
					tmp_node = Hnodes[tmp_node].right_child;
				
				Hnodes[right1_tmp].parent = tmp_node;
				Hnodes[tmp_node].right_child = right1_tmp;
            }
		}

		if(Hnodes[node2].parent != -1) 
		{
			if(Hnodes[parent2_tmp].right_child == node2)
			    Hnodes[parent2_tmp].right_child = node1;
		    else
		        Hnodes[parent2_tmp].left_child = node1;
		} 
		else 
		{
			root = node1;
		}

		Hnodes[node1].right_child = node2;
		Hnodes[node1].parent = parent2_tmp;
		Hnodes[node2].parent = node1;

		if(Hnodes[node2].left_child != -1)
		    Hnodes[left2_tmp].parent = node1;
		
		Hnodes[node1].left_child = left2_tmp;
		Hnodes[node2].left_child = -1;
	}
	else
	{
		if(Hnodes[node1].left_child != -1)
			Hnodes[left1_tmp].parent = node2;

		if(Hnodes[node2].left_child != -1)
			Hnodes[left2_tmp].parent = node1;
		
		if(Hnodes[node1].parent == Hnodes[node2].parent) 
		{
			if(Hnodes[parent1_tmp].left_child == node1) 
			{
				Hnodes[parent1_tmp].left_child = node2;
				Hnodes[parent1_tmp].right_child = node1;
			} 
			else 
			{
				Hnodes[parent1_tmp].left_child = node1;
				Hnodes[parent1_tmp].right_child = node2;
			}
		}
		else 
		{
			if(Hnodes[node1].parent != -1) 
			{
				if(Hnodes[parent1_tmp].right_child == node1)	//node1 is the right child of its parent
					Hnodes[parent1_tmp].right_child = node2;
				else
					Hnodes[parent1_tmp].left_child = node2;
			} 
			else 
			{
				root = node2;
			}

			if(Hnodes[node2].parent != -1) 
			{
				if(Hnodes[parent2_tmp].right_child == node2)		//node2 is the right child of its parent
					Hnodes[parent2_tmp].right_child = node1;
				else
					Hnodes[parent2_tmp].left_child = node1;
			} 
			else 
			{
				root = node1;
			}
		}

		Hnodes[node1].left_child = left2_tmp;
		Hnodes[node1].parent = parent2_tmp;
		Hnodes[node2].left_child = left1_tmp;
		Hnodes[node2].parent = parent1_tmp;
		
	}
}

void HBTree::swap_node(int node1, int node2)
{
	if(root == node1)
        root = node2;
    else if(root == node2)
        root = node1;

	int parent1_tmp = Hnodes[node1].parent;
    int left1_tmp = Hnodes[node1].left_child;
    int right1_tmp = Hnodes[node1].right_child;

    int parent2_tmp = Hnodes[node2].parent;
    int left2_tmp = Hnodes[node2].left_child;
    int right2_tmp = Hnodes[node2].right_child;

	if(parent1_tmp != -1)	//調整node1的parent的left child或是right child
    {
        if(Hnodes[parent1_tmp].left_child == node1)
            Hnodes[parent1_tmp].left_child = node2;
        else
            Hnodes[parent1_tmp].right_child = node2;
    }

	if(left1_tmp != -1)		//調整node1的child的parent
        Hnodes[left1_tmp].parent = node2;
    if(right1_tmp != -1)
        Hnodes[right1_tmp].parent = node2;
	
	if(parent2_tmp != -1)	//調整node2的parent的left child或是right child
    {
        if(Hnodes[parent2_tmp].left_child == node2)
            Hnodes[parent2_tmp].left_child = node1;
        else
            Hnodes[parent2_tmp].right_child = node1;
    }

    if(left2_tmp != -1)		//調整node2的child的parent
        Hnodes[left2_tmp].parent = node1;
    if(right2_tmp != -1)
        Hnodes[right2_tmp].parent = node1;


	//調整node1和node2的parent、left child、right child
	Hnodes[node1].parent = parent2_tmp;
    Hnodes[node1].left_child = left2_tmp;
    Hnodes[node1].right_child = right2_tmp;

    Hnodes[node2].parent = parent1_tmp;
    Hnodes[node2].left_child = left1_tmp;
    Hnodes[node2].right_child = right1_tmp;

    if(Hnodes[node1].parent == node1)
        Hnodes[node1].parent = node2;
    else if(Hnodes[node1].left_child == node1)
        Hnodes[node1].left_child = node2;
    else if(Hnodes[node1].right_child == node1)
        Hnodes[node1].right_child = node2;

    if(Hnodes[node2].parent == node2)
        Hnodes[node2].parent = node1;
    else if(Hnodes[node2].left_child == node2)
        Hnodes[node2].left_child = node1;
    else if(Hnodes[node2].right_child == node2)
        Hnodes[node2].right_child = node1;

	return;
}

bool HBTree::check_hierarchy_swap(int node1, int node2)		//檢查node2是不是在hierarchy node1的右子底下
{
	if(Hnodes[node1].right_child == node2)
		return 1;
	else if(Hnodes[node2].parent == -1)
		return 0;
	else
		return check_hierarchy_swap(node1, Hnodes[node2].parent);
}

void HBTree::update_contour_node_coordinate()
{
	int ASF_node_x = Hnodes[ASF_node_id].Cell_info.x;
	int ASF_node_y = Hnodes[ASF_node_id].Cell_info.y;

	for(int i=Hnodes.size()-contour_nodes.size(); i<Hnodes.size(); i++)
	{
		if(i == Hnodes.size()-contour_nodes.size())
			Hnodes[i].Cell_info.x = ASF_node_x;
		else
			Hnodes[i].Cell_info.x = Hnodes[i-1].Cell_info.x + Hnodes[i-1].Cell_info.width;
	}
}

void HBTree::preorder(int node_id) 
{
	cout << Hnodes[node_id].Cell_info.name << " id=" << node_id << endl;
	cout << "parent=" << Hnodes[node_id].parent << " left=" << Hnodes[node_id].left_child << " right=" << Hnodes[node_id].right_child << " x=" << Hnodes[node_id].Cell_info.x << " y=" << Hnodes[node_id].Cell_info.y << endl;
	cout << "*************" << endl;
	system("pause");

	if(Hnodes[node_id].left_child != -1)
		preorder(Hnodes[node_id].left_child);

	if(Hnodes[node_id].right_child != -1)
		preorder(Hnodes[node_id].right_child);
}

/*void HBTree::update_ASF_node_coordinate()
{
	for(int i=0; i<Hnodes[ASF_node_id].ASF_BTree.nodes.size(); i++)
	{
		Node *ASF_node = &Hnodes[ASF_node_id].ASF_BTree.nodes[i];
		ASF_node->Cell_info.x = ASF_node->Cell_info.x + Hnodes[ASF_node_id].Cell_info.x;
		ASF_node->Cell_info.y = ASF_node->Cell_info.y + Hnodes[ASF_node_id].Cell_info.y;

		string symmetry_transistor = symmetry_group[ASF_node->get_cell_name()];
        All_Node[id_check[symmetry_transistor]].Cell_info.x = 2*symmetry_axis - nodes[i].Cell_info.x;
        All_Node[id_check[symmetry_transistor]].Cell_info.y = nodes[i].Cell_info.y;
	}
}*/