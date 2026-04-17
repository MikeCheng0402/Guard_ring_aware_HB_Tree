#include <iostream>
#include "GRTree.h"

using namespace std;

void GRTree::initial(vector<vector<Node> > data_in)
{
    nodes = data_in;
    for(int i=0; i<nodes.size(); i++)
    {
        //cout << "Begin BTree initial " << i << endl;
        //cout << "node name = " << nodes[i].Cell_info.name << endl;
        nodes[i].id = i;
        nodes[i].parent = (i+1)/2-1;
        nodes[i].left_child = (2*i+1 < nodes.size() ? 2*i+1 : -1);
        nodes[i].right_child = (2*i+2 < nodes.size() ? 2*i+2 : -1);
    }
    nodes[0].parent = -1;
    root = nodes[0].id;
}

void GRTree::output_tree(int node_id) 
{
	cout << node_id << " " << nodes[node_id].Cell_info.name;
    cout << " left=" << nodes[node_id].left_child << " right=" << nodes[node_id].right_child << " x=" << nodes[node_id].Cell_info.x << " y=" << nodes[node_id].Cell_info.y << endl;
	//cout << "length=" << nodes[node_id].Cell_info.length << " width=" << nodes[node_id].Cell_info.width << endl;
    //cout << "***************************************" << endl;
	  
	if(nodes[node_id].left_child != -1) 
    {
		output_tree(nodes[node_id].left_child);
	}
	if(nodes[node_id].right_child != -1) 
    {
		output_tree(nodes[node_id].right_child);
	}
}

int GRTree::show_root()
{
    return root;
}

void GRTree::placement(int *total_width, int *total_length)
{
    clean_contour();
    *total_width = 0;
    *total_length = 0;
    set_coordinate(root, total_width, total_length);
}

void GRTree::symmetry_placement(int *total_width, int *total_length)
{
    vector<int> symmetry_contour;
    for(int i=contour.size()-GR_TO_GR-1; i>=(GUARD_RING_WIDTH+WELL_TO_GUARD_RING); i--)      //左半邊
        symmetry_contour.push_back(contour[i]);

    for(int i=0; i<CELL_DISTANCE; i++)                          //中間
        symmetry_contour.push_back(contour[0]);

    for(int i=(GUARD_RING_WIDTH+WELL_TO_GUARD_RING); i<contour.size()-GR_TO_GR; i++)         //右半邊
        symmetry_contour.push_back(contour[i]);

    //*total_width = 2*(*total_length) + CELL_DISTANCE;
    *total_width = symmetry_contour.size();

    // cout << "current contour size " << contour.size() << endl;
    contour = symmetry_contour;
    // cout << "symmetry contour size " << contour.size() << endl;
    return;
}

vector<int> GRTree::get_contour()
{
    return contour;
}

void GRTree::perturb()
{
    int option;
	int block_num;
	int node1 = -1;
	int node2 = -1;
	
	option = bool(rand()%2);;
	block_num = nodes.size();

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

void GRTree::clean_contour()
{
    contour.clear();
}

void GRTree::show_tree() 
{
	preorder(root);
}

void GRTree::show_all_tree()
{
	for(int i=0; i<nodes.size(); i++)
	{
		cout << nodes[i].Cell_info.name << " id=" << i << endl;
		cout << "parent=" << nodes[i].parent << " left=" << nodes[i].left_child << " right=" << nodes[i].right_child << " x=" << nodes[i].Cell_info.x << " y=" << nodes[i].Cell_info.y << endl;
	}
}

////////////////////private
void GRTree::set_coordinate(int node_id, int *total_width, int *total_length)
{
    // int SPACE = CELL_DISTANCE;
    int SPACE_V = 2*WELL_TO_GUARD_RING + 2*GUARD_RING_HEIGHT + GR_TO_GR;
    int SPACE_H = 2*WELL_TO_GUARD_RING + 2*GUARD_RING_WIDTH + GR_TO_GR;

    int parent_id = nodes[node_id].parent;
    if(node_id == root)
    {
        nodes[node_id].Cell_info.x = 0;
    }
    else if(node_id == nodes[parent_id].left_child)
    {
        nodes[node_id].Cell_info.x =nodes[parent_id].Cell_info.x + nodes[parent_id].Cell_info.width + SPACE_H;
    }
    else if(node_id == nodes[parent_id].right_child)
    {
        nodes[node_id].Cell_info.x =nodes[parent_id].Cell_info.x;
    }

    int contour_begin = nodes[node_id].Cell_info.x;
    int contour_end = nodes[node_id].Cell_info.x + nodes[node_id].Cell_info.width + SPACE_H;
    int lower_y = 0;

    while(contour.size() < contour_end + 1)
    {
        contour.push_back(0);
    }

    for(int i = contour_begin; i < contour_end; i++)
    {
        if(lower_y < contour[i])
            lower_y = contour[i];
    }

    nodes[node_id].Cell_info.y = lower_y;

    for(int i = contour_begin; i < contour_end; i++)
        contour[i] = nodes[node_id].Cell_info.y + nodes[node_id].Cell_info.length + SPACE_V;

    if(*total_width < nodes[node_id].Cell_info.x + nodes[node_id].Cell_info.width + SPACE_H - GR_TO_GR)
        *total_width = nodes[node_id].Cell_info.x + nodes[node_id].Cell_info.width + SPACE_H - GR_TO_GR;
    if(*total_length < nodes[node_id].Cell_info.y + nodes[node_id].Cell_info.length + SPACE_V - GR_TO_GR)
        *total_length = nodes[node_id].Cell_info.y + nodes[node_id].Cell_info.length + SPACE_V - GR_TO_GR;

    if(nodes[node_id].left_child != -1)
        set_coordinate(nodes[node_id].left_child, total_width, total_length);
    if(nodes[node_id].right_child != -1)
        set_coordinate(nodes[node_id].right_child, total_width, total_length);

    return;
}

void GRTree::move_node(int node1, int node2)
{
	if(nodes[node1].left_child == -1 && nodes[node1].right_child == -1) 		//當node1的兩個child都為-1，直接拔掉
    {
        int parent = nodes[node1].parent;
        if(nodes[parent].left_child == node1)
            nodes[parent].left_child = -1;
        else if(nodes[parent].right_child == node1)
            nodes[parent].right_child = -1;
    }
	else if(nodes[node1].left_child != -1 && nodes[node1].right_child != -1)	//當node1的兩個child都不是-1，要把node1 shift到樹的terminal node
    {
        while(nodes[node1].left_child != -1 || nodes[node1].right_child != -1)
        {
            int swapped_node;
            bool p = rand() % 2;
            if(p == 1)
            {
                if(nodes[node1].left_child == -1)
                    swapped_node = nodes[node1].right_child;
                else
                    swapped_node = nodes[node1].left_child;
            }
            else
            {
                if(nodes[node1].right_child == -1)
                    swapped_node = nodes[node1].left_child;
                else
                    swapped_node = nodes[node1].right_child;
            }

            swap_node(node1, swapped_node);
        }

        int parent = nodes[node1].parent;
        if(nodes[parent].left_child == node1)
            nodes[parent].left_child = -1;
        else if(nodes[parent].right_child == node1)
            nodes[parent].right_child = -1;
    }
	else	//當node1的其中一個child為-1，把node1拔掉，並把node1不為-1的child接到parent上
    {
        int child;
        if(nodes[node1].left_child != -1)
            child = nodes[node1].left_child;
        else
            child = nodes[node1].right_child;

        if(nodes[node1].parent != -1)
        {
            int parent = nodes[node1].parent;
            if(nodes[parent].left_child == node1)
                nodes[parent].left_child = child;
            else
                nodes[parent].right_child = child;
        }

        nodes[child].parent = nodes[node1].parent;

        if(node1 == root)
            root = child;
    }

	nodes[node1].parent = node2;

    int state = rand() % 4;

	int child;
    switch (state)
    {
    case 0:
        child = nodes[node2].left_child;
        nodes[node1].left_child = child;
        nodes[node1].right_child = -1;
        nodes[node2].left_child = node1;
        break;
    case 1:
        child = nodes[node2].left_child;
        nodes[node1].left_child = -1;
        nodes[node1].right_child = child;
        nodes[node2].left_child = node1;
        break;
    case 2:
        child = nodes[node2].right_child;
        nodes[node1].left_child = child;
        nodes[node1].right_child = -1;
        nodes[node2].right_child = node1;
        break;
    case 3:
        child = nodes[node2].right_child;
        nodes[node1].left_child = -1;
        nodes[node1].right_child = child;
        nodes[node2].right_child = node1;
        break;
    }

	if(child != -1)
        nodes[child].parent = node1;

    return;
}

void GRTree::swap_node(int node1, int node2)
{
	if(root == node1)
        root = node2;
    else if(root == node2)
        root = node1;

	int parent1_tmp = nodes[node1].parent;
    int left1_tmp = nodes[node1].left_child;
    int right1_tmp = nodes[node1].right_child;

    int parent2_tmp = nodes[node2].parent;
    int left2_tmp = nodes[node2].left_child;
    int right2_tmp = nodes[node2].right_child;

	if(parent1_tmp != -1)	//調整node1的parent的left child或是right child
    {
        if(nodes[parent1_tmp].left_child == node1)
            nodes[parent1_tmp].left_child = node2;
        else
            nodes[parent1_tmp].right_child = node2;
    }

	if(left1_tmp != -1)		//調整node1的child的parent
        nodes[left1_tmp].parent = node2;
    if(right1_tmp != -1)
        nodes[right1_tmp].parent = node2;
	
	if(parent2_tmp != -1)	//調整node2的parent的left child或是right child
    {
        if(nodes[parent2_tmp].left_child == node2)
            nodes[parent2_tmp].left_child = node1;
        else
            nodes[parent2_tmp].right_child = node1;
    }

    if(left2_tmp != -1)		//調整node2的child的parent
        nodes[left2_tmp].parent = node1;
    if(right2_tmp != -1)
        nodes[right2_tmp].parent = node1;


	//調整node1和node2的parent、left child、right child
	nodes[node1].parent = parent2_tmp;
    nodes[node1].left_child = left2_tmp;
    nodes[node1].right_child = right2_tmp;

    nodes[node2].parent = parent1_tmp;
    nodes[node2].left_child = left1_tmp;
    nodes[node2].right_child = right1_tmp;

    if(nodes[node1].parent == node1)
        nodes[node1].parent = node2;
    else if(nodes[node1].left_child == node1)
        nodes[node1].left_child = node2;
    else if(nodes[node1].right_child == node1)
        nodes[node1].right_child = node2;

    if(nodes[node2].parent == node2)
        nodes[node2].parent = node1;
    else if(nodes[node2].left_child == node2)
        nodes[node2].left_child = node1;
    else if(nodes[node2].right_child == node2)
        nodes[node2].right_child = node1;

	return;
}

void GRTree::preorder(int node_id) 
{
	cout << nodes[node_id].Cell_info.name << " id=" << node_id << endl;
	cout << "parent=" << nodes[node_id].parent << " left=" << nodes[node_id].left_child << " right=" << nodes[node_id].right_child << " x=" << nodes[node_id].Cell_info.x << " y=" << nodes[node_id].Cell_info.y << endl;
	cout << "*************" << endl;
	system("pause");

	if(nodes[node_id].left_child != -1)
		preorder(nodes[node_id].left_child);

	if(nodes[node_id].right_child != -1)
		preorder(nodes[node_id].right_child);
}