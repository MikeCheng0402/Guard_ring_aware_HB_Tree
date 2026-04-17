#include <iostream>
#include <vector>
#include <map>
#include <utility>
#include <fstream>
#include <string>

#define CELL_DISTANCE       140
#define GR_TO_GR            400
#define GUARD_RING_WIDTH    451
#define GUARD_RING_HEIGHT   240
#define WELL_TO_GUARD_RING  140

using namespace std;

struct Cell
{
    string name;
    int x, y, width, height;
};

vector<pair<Cell, Cell> > cell_information;
map<string, pair<Cell, Cell>* > cell_list;
int cell_number;
int total_width, total_height;

void read_file(char *input_file)
{
    ifstream ifile(input_file, ios::in);
    string word;

    cout << "Reading input file" << endl;

    if(!ifile)
    {
        cout << "The Spice file " << input_file << " can not be opened!!!" << endl;
        ifile.close();
        return;
    }
    else
    {
        ifile >> cell_number;
        ifile >> total_width >> total_height;
        cell_information.resize(cell_number/2);
        for(int i=0; i<cell_number/2; i++)
        {
            ifile >> word;
            ifile >> cell_information[i].first.x;
            ifile >> cell_information[i].first.y;
            ifile >> cell_information[i].first.width;
            ifile >> cell_information[i].first.height;
            ifile >> cell_information[i].first.name;

            cell_list[cell_information[i].first.name] = &cell_information[i];
        }

        for(int i=0; i<cell_number/2; i++)
        {
            ifile >> word;
            ifile >> cell_information[i].second.x;
            ifile >> cell_information[i].second.y;
            ifile >> cell_information[i].second.width;
            ifile >> cell_information[i].second.height;
            ifile >> cell_information[i].second.name;
        }

        ifile.close();
    }
}

void move()
{
    int move_num = 0;
    cout << "How many cells do you want to move: ";
    cin >> move_num;

    cout << "Enter the cells you want to move: ";
    string move_list[move_num];
    for(int i=0; i<move_num; i++)
    {
        cin >> move_list[i];
    }

    cout << "Enter the distance x, y: ";
    int move_x, move_y;
    cin >> move_x >> move_y;

    for(int i=0; i<move_num; i++)
    {
        cell_list[move_list[i]]->first.x += move_x;
        cell_list[move_list[i]]->first.y += move_y;
        cell_list[move_list[i]]->second.x += move_x;
        cell_list[move_list[i]]->second.y += move_y;
    }
    return;
}

void swap()
{
    cout << "Which two cells do you want to swap: ";
    string swap1, swap2;
    cin >> swap1 >> swap2;

    int x_tmp, y_tmp;
    x_tmp = cell_list[swap1]->first.x;
    y_tmp = cell_list[swap1]->first.y;
    //cout << x_tmp << " " << y_tmp << endl;
    cell_list[swap1]->first.x = cell_list[swap2]->first.x;
    cell_list[swap1]->first.y = cell_list[swap2]->first.y;
    cell_list[swap2]->first.x = x_tmp;
    cell_list[swap2]->first.y = y_tmp;

    x_tmp = cell_list[swap1]->second.x;
    y_tmp = cell_list[swap1]->second.y;
    //cout << x_tmp << " " << y_tmp << endl;
    cell_list[swap1]->second.x = cell_list[swap2]->second.x;
    cell_list[swap1]->second.y = cell_list[swap2]->second.y;
    cell_list[swap2]->second.x = x_tmp;
    cell_list[swap2]->second.y = y_tmp;

    return;
}

void neighbour()
{
    cout << "Move A next to B: ";
    string swap1, swap2;
    cin >> swap1 >> swap2;

    cout << "Where 1.top 2.bottom 3.left 4.right: ";
    int where = 0;
    cin >> where;

    cout << "1.Together or 2.seperate: ";
    int type;
    cin >> type;

    int locate_x, locate_y;
    int deplace_x, deplace_y;
    if(type == 1)
    {
        if(where == 1)
        {
            locate_x = cell_list[swap2]->second.x;
            locate_y = cell_list[swap2]->second.y + cell_list[swap2]->second.height + CELL_DISTANCE;
        }
        else if(where == 2)
        {
            locate_x = cell_list[swap2]->second.x;
            locate_y = cell_list[swap2]->second.y - CELL_DISTANCE - cell_list[swap1]->second.height;
        }
        else if(where == 3)
        {
            locate_x = cell_list[swap2]->second.x - CELL_DISTANCE - cell_list[swap1]->second.width;
            locate_y = cell_list[swap2]->second.y;
        }
        else if(where == 4)
        {
            locate_x = cell_list[swap2]->second.x + cell_list[swap2]->second.width + CELL_DISTANCE;
            locate_y = cell_list[swap2]->second.y;
        }
        deplace_x = cell_list[swap1]->second.x - locate_x;
        deplace_y = cell_list[swap1]->second.y - locate_y;
    }
    else if(type == 2)
    {
        if(where == 1)
        {
            locate_x = cell_list[swap2]->second.x;
            locate_y = cell_list[swap2]->second.y + cell_list[swap2]->second.height + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_HEIGHT + GR_TO_GR;
        }
        else if(where == 2)
        {
            locate_x = cell_list[swap2]->second.x;
            locate_y = cell_list[swap2]->second.y - 2*WELL_TO_GUARD_RING - 2*GUARD_RING_HEIGHT - GR_TO_GR - cell_list[swap1]->second.height;
        }
        else if(where == 3)
        {
            locate_x = cell_list[swap2]->second.x - 2*WELL_TO_GUARD_RING - 2*GUARD_RING_WIDTH - GR_TO_GR - cell_list[swap1]->second.width;
            locate_y = cell_list[swap2]->second.y;
        }
        else if(where == 4)
        {
            locate_x = cell_list[swap2]->second.x + cell_list[swap2]->second.width + 2*WELL_TO_GUARD_RING + 2*GUARD_RING_WIDTH + GR_TO_GR;
            locate_y = cell_list[swap2]->second.y;
        }
        deplace_x = cell_list[swap1]->second.x - locate_x;
        deplace_y = cell_list[swap1]->second.y - locate_y;
    }

    cell_list[swap1]->first.x -= deplace_x;
    cell_list[swap1]->first.y -= deplace_y;
    cell_list[swap1]->second.x -= deplace_x;
    cell_list[swap1]->second.y -= deplace_y;
}

void sharing()
{
    cout << "Which two cells do you want to do sharing: ";
    string share1, share2;
    cin >> share1 >> share2;

    int x_middle;
    x_middle = (cell_list[share1]->second.x + cell_list[share1]->second.width + cell_list[share2]->second.x)/2;

    int share1_width = x_middle - cell_list[share1]->second.x;
    cell_list[share1]->first.width = cell_list[share1]->first.width + (share1_width - cell_list[share1]->second.width);
    cell_list[share1]->second.width = share1_width;

    int share2_width = cell_list[share2]->second.x + cell_list[share2]->second.width - x_middle;
    cell_list[share2]->first.width = cell_list[share2]->first.width + (share2_width - cell_list[share2]->second.width);
    cell_list[share2]->first.x = cell_list[share2]->first.x - (cell_list[share2]->second.x - x_middle);
    cell_list[share2]->second.width = share2_width;
    cell_list[share2]->second.x = x_middle;

    return;
}

void CreateOutputFile(char *filename)
{
    ofstream ofile(filename, ios::out);

    if(ofile.is_open())
    {
        ofile << cell_number << endl;

        total_width = 0;
        total_height = 0;
        for(int i=0; i<cell_information.size(); i++)
        {
            if(total_width < cell_information[i].first.x + cell_information[i].first.width)
                total_width = cell_information[i].first.x + cell_information[i].first.width;
            if(total_height < cell_information[i].first.y + cell_information[i].first.height)
                total_height = cell_information[i].first.y + cell_information[i].first.height;
        }

        if(total_width > total_height)
            ofile << total_width << " " << total_width << endl;
        else
            ofile << total_height << " " << total_height << endl;

        for(int i=0; i<cell_information.size(); i++)
        {
            ofile << 0 << " ";
            ofile << cell_information[i].first.x << " " << cell_information[i].first.y << " ";
            ofile << cell_information[i].first.width << " " << cell_information[i].first.height << " ";
            ofile << cell_information[i].first.name << endl;
        }
         for(int i=0; i<cell_information.size(); i++)
        {
            ofile << 1 << " ";
            ofile << cell_information[i].second.x << " " << cell_information[i].second.y << " ";
            ofile << cell_information[i].second.width << " " << cell_information[i].second.height << " ";
            ofile << cell_information[i].second.name << endl;
        }
    }
    ofile.close();

    return;
}

int main(int argc, char *argv[])
{
    cout << "Begin" << endl;
    read_file(argv[argc-2]);

    int option = 0;

    while(1)
    {
        cout << "Option: 1.move 2.swap 3.neighbour 4.sharing 5.end" << endl;
        cout << "Enter: ";
        cin >> option;

        if(option == 1)
        {
            move();
        }
        else if(option == 2)
        {
            swap();
        }
        else if(option == 3)
        {
            neighbour();
        }
        else if(option == 4)
        {
            sharing();
        }
        else
        {
            break;
        }

        CreateOutputFile(argv[argc-1]);
    }

    return 0;
}