#include <iostream>
#include "BTree.h"

using namespace std;


Cell::Cell(string a, string b, string c, string d, string e, string f, int g, int h, int i, bool j, bool k)
{
    name = a;
    drain = b;
    gate = c;
    source = d;
    body = e;
    type = f;
    fin = g;
    l = h;
    m = i;
    symmetry = j;
    common = k;
    x = 0;
    y = 0;
    width = m*l + (m+1)*POLY_TO_POLY + 2*DUMMY_POLY_WIDTH + 2*POLY_TO_WELL;
    length = (fin-1)*FIN_SPACE + FIN_HIGH + OD_TO_WELL_TOP + OD_TO_WELL_BOTTOM;
}

Node::Node(int a, Cell b)
{
    id = a;
    Cell_info = b;
    parent = -1;
    left_child = -1;
    right_child = -1;
}

string Node::get_cell_name()
{
    return Cell_info.name;
}

void Node::output_info()
{
    cout << Cell_info.name << " " << Cell_info.drain << " " << Cell_info.gate << " " << Cell_info.source << " " << Cell_info.body << " " << Cell_info.type
         << " " << Cell_info.fin << " " << Cell_info.l << " " << Cell_info.m << " " << Cell_info.common << endl;
}

void BTree::output_data()
{
    for(int i=0; i<nodes.size(); i++)
    {
        cout << nodes[i].Cell_info.name << " " << nodes[i].Cell_info.drain << " " << nodes[i].Cell_info.gate << " " << nodes[i].Cell_info.source << " " << nodes[i].Cell_info.body << " " << nodes[i].Cell_info.type
         << " " << nodes[i].Cell_info.fin << " " << nodes[i].Cell_info.l << " " << nodes[i].Cell_info.m << " " << nodes[i].Cell_info.common << endl;
    }
}

void BTree::initial(vector<Node> data_in)
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

void BTree::output_tree(int node_id) 
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

int BTree::show_root()
{
    return root;
}

void BTree::placement(int *total_width, int *total_length)
{
    clean_contour();
    *total_width = 0;
    *total_length = 0;
    set_coordinate(root, total_width, total_length);
}

void BTree::symmetry_placement(int *total_width, int *total_length)
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

vector<int> BTree::get_contour()
{
    return contour;
}

void BTree::perturb()
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

void BTree::clean_contour()
{
    contour.clear();
}

void BTree::show_tree() 
{
	preorder(root);
}

void BTree::show_all_tree()
{
	for(int i=0; i<nodes.size(); i++)
	{
		cout << nodes[i].Cell_info.name << " id=" << i << endl;
		cout << "parent=" << nodes[i].parent << " left=" << nodes[i].left_child << " right=" << nodes[i].right_child << " x=" << nodes[i].Cell_info.x << " y=" << nodes[i].Cell_info.y << endl;
	}
}

////////////////////private
void BTree::set_coordinate(int node_id, int *total_width, int *total_length)
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

void BTree::move_node(int node1, int node2)
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

void BTree::swap_node(int node1, int node2)
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

void BTree::preorder(int node_id) 
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


/*void BTree::update_symmetry_coordinate(int displacement)
{
    int symmetry_axis = displacement - CELL_DISTANCE/2;
    for(int i=0; i<nodes.size(); i++)
    {
        nodes[i].Cell_info.x = nodes[i].Cell_info.x + displacement;

        string symmetry_transistor = symmetry_group[nodes[i].get_cell_name()];
        All_Node[id_check[symmetry_transistor]].Cell_info.x = 2*symmetry_axis - nodes[i].Cell_info.x;
        All_Node[id_check[symmetry_transistor]].Cell_info.y = nodes[i].Cell_info.y;
    }

    return;
}*/