#include <iostream>
#include "THBTree.h"

using namespace std;

void THBTree::initial(vector<vector<Node> > data_in) 
{
	TH_node.resize(data_in.size());	

	for(int i=0; i<data_in.size(); i++) 
    {
        // cout << "Begin THBTree initial " << i << endl;
		TH_node[i].H_BTree.initial(data_in[i]);
		TH_node[i].id = i;
		TH_node[i].parent = (i+1)/2-1;
		TH_node[i].left_child = (2*i+1 < TH_node.size() ? 2*i+1 : -1);
		TH_node[i].right_child = (2*i+2 < TH_node.size() ? 2*i+2 : -1);
		TH_node[i].Cell_info.x = 0;
		TH_node[i].Cell_info.y = 0;
		TH_node[i].Cell_info.name = "proximity";
	}

	root = TH_node[0].id;
}

void THBTree::output_tree(int node_id) 
{
	//cout << node_id << " " << TH_node[node_id].Cell_info.name;
    //cout << " left=" << TH_node[node_id].left_child << " right=" << TH_node[node_id].right_child << " x=" << TH_node[node_id].Cell_info.x << " y=" << TH_node[node_id].Cell_info.y << endl;
	//cout << "length=" << TH_node[node_id].Cell_info.length << " width=" << TH_node[node_id].Cell_info.width << endl;
    //cout << "***************************************" << endl;
	
    cout << "GUARD RING******************" << node_id << endl;
    TH_node[node_id].H_BTree.output_tree(TH_node[node_id].H_BTree.show_root());
	if(TH_node[node_id].left_child != -1) 
    {
		output_tree(TH_node[node_id].left_child);
	}
	if(TH_node[node_id].right_child != -1) 
    {
		output_tree(TH_node[node_id].right_child);
	}
}

int THBTree::show_root()
{
    return root;
}

void THBTree::placement(int *total_width, int *total_length)
{
    clean_contour();
	set_coordinate(root, total_width, total_length);
}

void THBTree::perturb()
{
    int option;
	int block_num;
	int node1 = -1;
	int node2 = -1;
	
	option = bool(rand()%2);;
	block_num = TH_node.size();

    // cout << "Executing THBTree perturb" << endl;

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

void THBTree::perturb_HBTree(vector<int> &record)
{
	int random_num;
	int block_num = TH_node.size();

    // cout << "Executing HBTree perturb" << endl;

	if(record[1] == -1)
	    random_num = rand()%block_num;
    else
        random_num = record[1];

    record[1] = random_num;
    // cout << "Perturbation id is " << random_num << endl;
    // TH_node[random_num].H_BTree.show_all_tree();
	TH_node[random_num].H_BTree.perturb();
}

void THBTree::perturb_ASF_Tree(vector<int> &record)
{
	int random_num;
	int block_num = TH_node.size();

	if(record[1] == -1)
	    random_num = rand()%block_num;
    else
        random_num = record[1];

    record[1] = random_num;
	
	if(TH_node[random_num].H_BTree.ASF_node_empty == 1) 
	{
		return;
	}
	TH_node[random_num].H_BTree.perturb_ASF_Tree();
}

void THBTree::show_all_tree()
{
	for(int i=0; i<TH_node.size(); i++)
	{
		cout << TH_node[i].Cell_info.name << " id=" << i << endl;
		cout << "parent=" << TH_node[i].parent << " left=" << TH_node[i].left_child << " right=" << TH_node[i].right_child << " x=" << TH_node[i].Cell_info.x << " y=" << TH_node[i].Cell_info.y << endl;
	}
}

////////////////////private
void THBTree::set_coordinate(int node_id, int *total_width, int *total_length)
{
    int SPACE = GR_TO_GR;

    // cout << "Begin HBTree placement" << endl;
	//H-B*tree placement begin
    TH_node[node_id].Cell_info.width = 0;
	TH_node[node_id].Cell_info.length = 0;
	TH_node[node_id].H_BTree.placement(&TH_node[node_id].Cell_info.width, &TH_node[node_id].Cell_info.length);
	TH_node[node_id].Cell_info.width = TH_node[node_id].Cell_info.width;
	TH_node[node_id].Cell_info.length = TH_node[node_id].Cell_info.length;
	//H-B*tree placement end
    // cout << "HBTree placement end" << endl;

    int parent_id = TH_node[node_id].parent;
    if(node_id == root)
    {
        TH_node[node_id].Cell_info.x = 0;
    }
    else if(node_id == TH_node[parent_id].left_child)
    {
        TH_node[node_id].Cell_info.x =TH_node[parent_id].Cell_info.x + TH_node[parent_id].Cell_info.width + GR_TO_GR;
    }
    else if(node_id == TH_node[parent_id].right_child)
    {
        TH_node[node_id].Cell_info.x =TH_node[parent_id].Cell_info.x;
    }

    int contour_begin = TH_node[node_id].Cell_info.x;
    int contour_end = TH_node[node_id].Cell_info.x + TH_node[node_id].Cell_info.width + GR_TO_GR;
    int lower_y = 0;

    while(TH_contour.size() < contour_end + 1)
    {
        TH_contour.push_back(0);
    }

    for(int i = contour_begin; i < contour_end; i++)
    {
        if(lower_y < TH_contour[i])
            lower_y = TH_contour[i];
    }

    TH_node[node_id].Cell_info.y = lower_y;

    for(int i = contour_begin; i < contour_end; i++)
        TH_contour[i] = TH_node[node_id].Cell_info.y + TH_node[node_id].Cell_info.length + GR_TO_GR;
    // for(int i = contour_begin; i < contour_end; i++)
    // {
    //     if(i-contour_begin <= TH_node[node_id].H_BTree.H_contour.size())
	// 		TH_contour[i] = TH_node[node_id].Cell_info.y + TH_node[node_id].H_BTree.H_contour[i-contour_begin] + GR_TO_GR;
	// 	else
	// 		TH_contour[i] = TH_node[node_id].Cell_info.y + TH_node[node_id].H_BTree.H_contour.back() + GR_TO_GR;
    // }

    if(*total_width < TH_node[node_id].Cell_info.x + TH_node[node_id].Cell_info.width)
        *total_width = TH_node[node_id].Cell_info.x + TH_node[node_id].Cell_info.width;
    if(*total_length < TH_node[node_id].Cell_info.y + TH_node[node_id].Cell_info.length)
        *total_length = TH_node[node_id].Cell_info.y + TH_node[node_id].Cell_info.length;

    if(TH_node[node_id].left_child != -1)
        set_coordinate(TH_node[node_id].left_child, total_width, total_length);
    if(TH_node[node_id].right_child != -1)
        set_coordinate(TH_node[node_id].right_child, total_width, total_length);

    return;
}

void THBTree::move_node(int node1, int node2)
{
    // cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    // cout << "Excute the THBTree move node " << endl;
    if(TH_node[node1].left_child == -1 && TH_node[node1].right_child == -1) 		//當node1的兩個child都為-1，直接拔掉
    {
        int parent = TH_node[node1].parent;
        if(TH_node[parent].left_child == node1)
            TH_node[parent].left_child = -1;
        else if(TH_node[parent].right_child == node1)
            TH_node[parent].right_child = -1;
    }
	else if(TH_node[node1].left_child != -1 && TH_node[node1].right_child != -1)	//當node1的兩個child都不是-1，要把node1 shift到樹的terminal node
    {
        while(TH_node[node1].left_child != -1 || TH_node[node1].right_child != -1)
        {
            int swapped_node;
            bool p = rand() % 2;
            if(p == 1)
            {
                if(TH_node[node1].left_child == -1)
                    swapped_node = TH_node[node1].right_child;
                else
                    swapped_node = TH_node[node1].left_child;
            }
            else
            {
                if(TH_node[node1].right_child == -1)
                    swapped_node = TH_node[node1].left_child;
                else
                    swapped_node = TH_node[node1].right_child;
            }

            swap_node(node1, swapped_node);
        }

        int parent = TH_node[node1].parent;
        if(TH_node[parent].left_child == node1)
            TH_node[parent].left_child = -1;
        else if(TH_node[parent].right_child == node1)
            TH_node[parent].right_child = -1;
    }
	else	//當node1的其中一個child為-1，把node1拔掉，並把node1不為-1的child接到parent上
    {
        int child;
        if(TH_node[node1].left_child != -1)
            child = TH_node[node1].left_child;
        else
            child = TH_node[node1].right_child;

        if(TH_node[node1].parent != -1)
        {
            int parent = TH_node[node1].parent;
            if(TH_node[parent].left_child == node1)
                TH_node[parent].left_child = child;
            else
                TH_node[parent].right_child = child;
        }

        TH_node[child].parent = TH_node[node1].parent;

        if(node1 == root)
            root = child;
    }

	TH_node[node1].parent = node2;

    int state = rand() % 4;

	int child;
    switch (state)
    {
    case 0:
        child = TH_node[node2].left_child;
        TH_node[node1].left_child = child;
        TH_node[node1].right_child = -1;
        TH_node[node2].left_child = node1;
        break;
    case 1:
        child = TH_node[node2].left_child;
        TH_node[node1].left_child = -1;
        TH_node[node1].right_child = child;
        TH_node[node2].left_child = node1;
        break;
    case 2:
        child = TH_node[node2].right_child;
        TH_node[node1].left_child = child;
        TH_node[node1].right_child = -1;
        TH_node[node2].right_child = node1;
        break;
    case 3:
        child = TH_node[node2].right_child;
        TH_node[node1].left_child = -1;
        TH_node[node1].right_child = child;
        TH_node[node2].right_child = node1;
        break;
    }

	if(child != -1)
        TH_node[child].parent = node1;

    return;
}


void THBTree::swap_node(int node1, int node2)
{
    // cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    // cout << "Excute the THBTree swap node " << endl;
    if(root == node1)
        root = node2;
    else if(root == node2)
        root = node1;

	int parent1_tmp = TH_node[node1].parent;
    int left1_tmp = TH_node[node1].left_child;
    int right1_tmp = TH_node[node1].right_child;

    int parent2_tmp = TH_node[node2].parent;
    int left2_tmp = TH_node[node2].left_child;
    int right2_tmp = TH_node[node2].right_child;

	if(parent1_tmp != -1)	//調整node1的parent的left child或是right child
    {
        if(TH_node[parent1_tmp].left_child == node1)
            TH_node[parent1_tmp].left_child = node2;
        else
            TH_node[parent1_tmp].right_child = node2;
    }

	if(left1_tmp != -1)		//調整node1的child的parent
        TH_node[left1_tmp].parent = node2;
    if(right1_tmp != -1)
        TH_node[right1_tmp].parent = node2;
	
	if(parent2_tmp != -1)	//調整node2的parent的left child或是right child
    {
        if(TH_node[parent2_tmp].left_child == node2)
            TH_node[parent2_tmp].left_child = node1;
        else
            TH_node[parent2_tmp].right_child = node1;
    }

    if(left2_tmp != -1)		//調整node2的child的parent
        TH_node[left2_tmp].parent = node1;
    if(right2_tmp != -1)
        TH_node[right2_tmp].parent = node1;


	//調整node1和node2的parent、left child、right child
	TH_node[node1].parent = parent2_tmp;
    TH_node[node1].left_child = left2_tmp;
    TH_node[node1].right_child = right2_tmp;

    TH_node[node2].parent = parent1_tmp;
    TH_node[node2].left_child = left1_tmp;
    TH_node[node2].right_child = right1_tmp;

    if(TH_node[node1].parent == node1)
        TH_node[node1].parent = node2;
    else if(TH_node[node1].left_child == node1)
        TH_node[node1].left_child = node2;
    else if(TH_node[node1].right_child == node1)
        TH_node[node1].right_child = node2;

    if(TH_node[node2].parent == node2)
        TH_node[node2].parent = node1;
    else if(TH_node[node2].left_child == node2)
        TH_node[node2].left_child = node1;
    else if(TH_node[node2].right_child == node2)
        TH_node[node2].right_child = node1;

	return;
}

void THBTree::clean_contour()
{
    TH_contour.clear();
}