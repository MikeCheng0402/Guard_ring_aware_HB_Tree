#include <iostream>
#include "FHBTree.h"

using namespace std;

void FHBTree::initial(vector<vector<vector<Node> > > data_in)
{
    FH_node.resize(data_in.size());	

	for(int i=0; i<data_in.size(); i++) 
    {
        // cout << "Begin FHBTree initial " << i << endl;
		FH_node[i].TH_Btree.initial(data_in[i]);
		FH_node[i].id = i;
		FH_node[i].parent = (i+1)/2-1;
		FH_node[i].left_child = (2*i+1 < FH_node.size() ? 2*i+1 : -1);
		FH_node[i].right_child = (2*i+2 < FH_node.size() ? 2*i+2 : -1);
		FH_node[i].Cell_info.x = 0;
		FH_node[i].Cell_info.y = 0;
		FH_node[i].Cell_info.name = "schemetic";
	}

	root = FH_node[0].id;
    overall_width = INT32_MAX;
    overall_height = INT32_MAX;
}

void FHBTree::output_tree(int node_id) 
{
	//cout << node_id << " " << nodes[node_id].Cell_info.name;
    //cout << " left=" << nodes[node_id].left_child << " right=" << nodes[node_id].right_child << " x=" << nodes[node_id].Cell_info.x << " y=" << nodes[node_id].Cell_info.y << endl;
	//cout << "length=" << nodes[node_id].Cell_info.length << " width=" << nodes[node_id].Cell_info.width << endl;
    //cout << "***************************************" << endl;
	cout << "Schemetic--------------------------" << node_id << endl;
    FH_node[node_id].TH_Btree.output_tree(FH_node[node_id].TH_Btree.show_root());
	if(FH_node[node_id].left_child != -1) 
    {
		output_tree(FH_node[node_id].left_child);
	}
	if(FH_node[node_id].right_child != -1) 
    {
		output_tree(FH_node[node_id].right_child);
	}
}

int FHBTree::show_root()
{
    return root;
}

void FHBTree::placement(int *total_width, int *total_length)
{
    clean_contour();
	set_coordinate(root, total_width, total_length);
}

void FHBTree::clean_contour()
{
	FH_contour.clear();
}

void FHBTree::clean_overall_width_height()
{
	overall_width = 0;
	overall_height = 0;
}

void FHBTree::perturb()
{
	int option;
	int block_num;
	int node1 = -1;
	int node2 = -1;
	
	option = bool(rand()%2);;
	block_num = FH_node.size();

    if(block_num == 1)
        return;

	while(node1 == node2) 
	{
		node1 = rand()%block_num;
		node2 = rand()%block_num;
	}

	if(option) //move
	{ 
		move_node(node1, node2);
	} 
	else //swap
	{ 
		swap_node(node1, node2);
	}
}

void FHBTree::perturb_THBTtree()
{
	int random_num;
	int block_num = FH_node.size();

	random_num = rand()%block_num;
	FH_node[random_num].TH_Btree.perturb();
}

void FHBTree::perturb_HBTree(vector<int> &record)
{
    int random_num;
	int block_num = FH_node.size();

    if(record[0] == -1)
	    random_num = rand()%block_num;
    else
        random_num = record[0];
    record[0] = random_num;

	FH_node[random_num].TH_Btree.perturb_HBTree(record);
}

void FHBTree::perturb_ASF_Tree(vector<int> &record)
{
    int random_num;
	int block_num = FH_node.size();

	if(record[0] == -1)
	    random_num = rand()%block_num;
    else
        random_num = record[0];
    record[0] = random_num;
    
	FH_node[random_num].TH_Btree.perturb_ASF_Tree(record);
}

void FHBTree::update_final_coordinate()
{
    for(int i=0; i<FH_node.size(); i++)
    {
        int fh_x = FH_node[i].Cell_info.x;
        int fh_y = FH_node[i].Cell_info.y;

        for(int j=0; j<FH_node[i].TH_Btree.TH_node.size(); j++)
        {
            int th_x = FH_node[i].TH_Btree.TH_node[j].Cell_info.x + fh_x;
            int th_y = FH_node[i].TH_Btree.TH_node[j].Cell_info.y + fh_y;

            for(int k=0; k<FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes.size(); k++)
            {
                int h_x = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.x + th_x;
                int h_y = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.y + th_y;

                if(FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_or_not == 0)
                {
                    FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.x = h_x + GUARD_RING_WIDTH + WELL_TO_GUARD_RING;
                    FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.y = h_y + GUARD_RING_HEIGHT + WELL_TO_GUARD_RING;

                    string cell_name = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.name;
                    All_Node[id_check[cell_name]].Cell_info.x = h_x + GUARD_RING_WIDTH + WELL_TO_GUARD_RING;
                    All_Node[id_check[cell_name]].Cell_info.y = h_y + GUARD_RING_HEIGHT + WELL_TO_GUARD_RING;

                    int guard_ring_x = All_Node[id_check[cell_name]].Cell_info.x - WELL_TO_GUARD_RING - GUARD_RING_WIDTH;
                    int guard_ring_y = All_Node[id_check[cell_name]].Cell_info.y - WELL_TO_GUARD_RING - GUARD_RING_HEIGHT;
                    int guard_ring_width = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.width + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_WIDTH;
                    int guard_ring_height = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.length + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_HEIGHT;

                    guard_ring_info[cell_name] = make_pair(make_pair(guard_ring_x, guard_ring_y), make_pair(guard_ring_width, guard_ring_height));
                }
                else
                {
                    int displacement = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.width/2 + CELL_DISTANCE/2;
                    int symmetry_axis_x = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].Cell_info.width/2 + h_x;

                    for(int l=0; l<FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes.size(); l++)
                    {
                        int ASF_x = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.x + h_x + displacement;
                        int ASF_y = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.y + h_y + GUARD_RING_HEIGHT + WELL_TO_GUARD_RING;

                        int sym_x = 2*symmetry_axis_x - ASF_x;
                        int sym_y = ASF_y;

                        FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.x = ASF_x;
                        FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.y = ASF_y;

                        string cell_name = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.name;
                        All_Node[id_check[cell_name]].Cell_info.x = ASF_x;
                        All_Node[id_check[cell_name]].Cell_info.y = ASF_y;
                        All_Node[id_check[symmetry_group[cell_name]]].Cell_info.x = sym_x - All_Node[id_check[symmetry_group[cell_name]]].Cell_info.width;
                        All_Node[id_check[symmetry_group[cell_name]]].Cell_info.y = sym_y;

                        int guard_ring_x = All_Node[id_check[cell_name]].Cell_info.x - WELL_TO_GUARD_RING - GUARD_RING_WIDTH;
                        int guard_ring_y = All_Node[id_check[cell_name]].Cell_info.y - WELL_TO_GUARD_RING - GUARD_RING_HEIGHT;
                        int guard_ring_width = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.width + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_WIDTH;
                        int guard_ring_height = FH_node[i].TH_Btree.TH_node[j].H_BTree.Hnodes[k].ASF_BTree.nodes[l].Cell_info.length + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_HEIGHT;

                        guard_ring_info[cell_name] = make_pair(make_pair(guard_ring_x, guard_ring_y), make_pair(guard_ring_width, guard_ring_height));

                        guard_ring_x = All_Node[id_check[symmetry_group[cell_name]]].Cell_info.x - WELL_TO_GUARD_RING - GUARD_RING_WIDTH;
                        guard_ring_y = All_Node[id_check[symmetry_group[cell_name]]].Cell_info.y - WELL_TO_GUARD_RING - GUARD_RING_HEIGHT;
                        guard_ring_width = All_Node[id_check[symmetry_group[cell_name]]].Cell_info.width + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_WIDTH;
                        guard_ring_height = All_Node[id_check[symmetry_group[cell_name]]].Cell_info.length + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_HEIGHT;

                        guard_ring_info[symmetry_group[cell_name]] = make_pair(make_pair(guard_ring_x, guard_ring_y), make_pair(guard_ring_width, guard_ring_height));
                    }
                }
            }
        }
    }
}

double FHBTree::get_area()
{
    return double(overall_width) * double(overall_height);
}

void FHBTree::show_HBTree()
{
    cout << "Show HBTree" << endl;
    for(int i=0; i<FH_node.size(); i++)
    {
        for(int j=0; j<FH_node[i].TH_Btree.TH_node.size(); j++)
        {
            FH_node[i].TH_Btree.TH_node[j].H_BTree.show_tree();
        }
    }
}

void FHBTree::show_all_HBTree()
{
    cout << "Show all HBTree" << endl;
    for(int i=0; i<FH_node.size(); i++)
    {
        for(int j=0; j<FH_node[i].TH_Btree.TH_node.size(); j++)
        {
            FH_node[i].TH_Btree.TH_node[j].H_BTree.show_all_tree();
        }
    }
}

////////////////////private
void FHBTree::set_coordinate(int node_id, int *total_width, int *total_length)
{
    // cout << "Begin THBTree placement" << endl;
    // cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << endl;
    // FH_node[node_id].TH_Btree.show_all_tree();

	FH_node[node_id].Cell_info.width = 0;
	FH_node[node_id].Cell_info.length = 0;
	FH_node[node_id].TH_Btree.placement(&FH_node[node_id].Cell_info.width, &FH_node[node_id].Cell_info.length);
	FH_node[node_id].Cell_info.width = FH_node[node_id].Cell_info.width;
	FH_node[node_id].Cell_info.length = FH_node[node_id].Cell_info.length;
	//TH-B*tree placement end
    // cout << "THBTree placement end" << endl;

    int parent_id = FH_node[node_id].parent;
    if(node_id == root)
    {
        FH_node[node_id].Cell_info.x = 0;
    }
    else if(node_id == FH_node[parent_id].left_child)
    {
        FH_node[node_id].Cell_info.x =FH_node[parent_id].Cell_info.x + FH_node[parent_id].Cell_info.width + GR_TO_GR;
    }
    else if(node_id == FH_node[parent_id].right_child)
    {
        FH_node[node_id].Cell_info.x =FH_node[parent_id].Cell_info.x;
    }

    int contour_begin = FH_node[node_id].Cell_info.x;
    int contour_end = FH_node[node_id].Cell_info.x + FH_node[node_id].Cell_info.width + GR_TO_GR;
    int lower_y = 0;

    while(FH_contour.size() < contour_end + 1)
    {
        FH_contour.push_back(0);
    }

    for(int i = contour_begin; i < contour_end; i++)
    {
        if(lower_y < FH_contour[i])
            lower_y = FH_contour[i];
    }

    FH_node[node_id].Cell_info.y = lower_y;

    for(int i = contour_begin; i < contour_end; i++)
        FH_contour[i] = FH_node[node_id].Cell_info.y + FH_node[node_id].Cell_info.length + GR_TO_GR;
    // for(int i = contour_begin; i < contour_end; i++)
    // {
    //     if(i-contour_begin <= FH_node[node_id].TH_Btree.TH_contour.size())
	// 		FH_contour[i] = FH_node[node_id].Cell_info.y + FH_node[node_id].TH_Btree.TH_contour[i-contour_begin] + GR_TO_GR;
	// 	else
	// 		FH_contour[i] = FH_node[node_id].Cell_info.y + FH_node[node_id].TH_Btree.TH_contour.back() + GR_TO_GR;
    // }

    if(*total_width < FH_node[node_id].Cell_info.x + FH_node[node_id].Cell_info.width)
        *total_width = FH_node[node_id].Cell_info.x + FH_node[node_id].Cell_info.width;
    if(*total_length < FH_node[node_id].Cell_info.y + FH_node[node_id].Cell_info.length)
        *total_length = FH_node[node_id].Cell_info.y + FH_node[node_id].Cell_info.length;

    if(FH_node[node_id].left_child != -1)
        set_coordinate(FH_node[node_id].left_child, total_width, total_length);
    if(FH_node[node_id].right_child != -1)
        set_coordinate(FH_node[node_id].right_child, total_width, total_length);

    return;
}

void FHBTree::move_node(int node1, int node2)
{
	if(FH_node[node1].left_child == -1 && FH_node[node1].right_child == -1) 		//當node1的兩個child都為-1，直接拔掉
    {
        int parent = FH_node[node1].parent;
        if(FH_node[parent].left_child == node1)
            FH_node[parent].left_child = -1;
        else if(FH_node[parent].right_child == node1)
            FH_node[parent].right_child = -1;
    }
	else if(FH_node[node1].left_child != -1 && FH_node[node1].right_child != -1)	//當node1的兩個child都不是-1，要把node1 shift到樹的terminal node
    {
        while(FH_node[node1].left_child != -1 || FH_node[node1].right_child != -1)
        {
            int swapped_node;
            bool p = rand() % 2;
            if(p == 1)
            {
                if(FH_node[node1].left_child == -1)
                    swapped_node = FH_node[node1].right_child;
                else
                    swapped_node = FH_node[node1].left_child;
            }
            else
            {
                if(FH_node[node1].right_child == -1)
                    swapped_node = FH_node[node1].left_child;
                else
                    swapped_node = FH_node[node1].right_child;
            }

            swap_node(node1, swapped_node);
        }

        int parent = FH_node[node1].parent;
        if(FH_node[parent].left_child == node1)
            FH_node[parent].left_child = -1;
        else if(FH_node[parent].right_child == node1)
            FH_node[parent].right_child = -1;
    }
	else	//當node1的其中一個child為-1，把node1拔掉，並把node1不為-1的child接到parent上
    {
        int child;
        if(FH_node[node1].left_child != -1)
            child = FH_node[node1].left_child;
        else
            child = FH_node[node1].right_child;

        if(FH_node[node1].parent != -1)
        {
            int parent = FH_node[node1].parent;
            if(FH_node[parent].left_child == node1)
                FH_node[parent].left_child = child;
            else
                FH_node[parent].right_child = child;
        }

        FH_node[child].parent = FH_node[node1].parent;

        if(node1 == root)
            root = child;
    }

	FH_node[node1].parent = node2;

    int state = rand() % 4;

	int child;
    switch (state)
    {
    case 0:
        child = FH_node[node2].left_child;
        FH_node[node1].left_child = child;
        FH_node[node1].right_child = -1;
        FH_node[node2].left_child = node1;
        break;
    case 1:
        child = FH_node[node2].left_child;
        FH_node[node1].left_child = -1;
        FH_node[node1].right_child = child;
        FH_node[node2].left_child = node1;
        break;
    case 2:
        child = FH_node[node2].right_child;
        FH_node[node1].left_child = child;
        FH_node[node1].right_child = -1;
        FH_node[node2].right_child = node1;
        break;
    case 3:
        child = FH_node[node2].right_child;
        FH_node[node1].left_child = -1;
        FH_node[node1].right_child = child;
        FH_node[node2].right_child = node1;
        break;
    }

	if(child != -1)
        FH_node[child].parent = node1;

    return;
}

void FHBTree::swap_node(int node1, int node2)
{
	if(root == node1)
        root = node2;
    else if(root == node2)
        root = node1;

	int parent1_tmp = FH_node[node1].parent;
    int left1_tmp = FH_node[node1].left_child;
    int right1_tmp = FH_node[node1].right_child;

    int parent2_tmp = FH_node[node2].parent;
    int left2_tmp = FH_node[node2].left_child;
    int right2_tmp = FH_node[node2].right_child;

	if(parent1_tmp != -1)	//調整node1的parent的left child或是right child
    {
        if(FH_node[parent1_tmp].left_child == node1)
            FH_node[parent1_tmp].left_child = node2;
        else
            FH_node[parent1_tmp].right_child = node2;
    }

	if(left1_tmp != -1)		//調整node1的child的parent
        FH_node[left1_tmp].parent = node2;
    if(right1_tmp != -1)
        FH_node[right1_tmp].parent = node2;
	
	if(parent2_tmp != -1)	//調整node2的parent的left child或是right child
    {
        if(FH_node[parent2_tmp].left_child == node2)
            FH_node[parent2_tmp].left_child = node1;
        else
            FH_node[parent2_tmp].right_child = node1;
    }

    if(left2_tmp != -1)		//調整node2的child的parent
        FH_node[left2_tmp].parent = node1;
    if(right2_tmp != -1)
        FH_node[right2_tmp].parent = node1;


	//調整node1和node2的parent、left child、right child
	FH_node[node1].parent = parent2_tmp;
    FH_node[node1].left_child = left2_tmp;
    FH_node[node1].right_child = right2_tmp;

    FH_node[node2].parent = parent1_tmp;
    FH_node[node2].left_child = left1_tmp;
    FH_node[node2].right_child = right1_tmp;

    if(FH_node[node1].parent == node1)
        FH_node[node1].parent = node2;
    else if(FH_node[node1].left_child == node1)
        FH_node[node1].left_child = node2;
    else if(FH_node[node1].right_child == node1)
        FH_node[node1].right_child = node2;

    if(FH_node[node2].parent == node2)
        FH_node[node2].parent = node1;
    else if(FH_node[node2].left_child == node2)
        FH_node[node2].left_child = node1;
    else if(FH_node[node2].right_child == node2)
        FH_node[node2].right_child = node1;

	return;
}