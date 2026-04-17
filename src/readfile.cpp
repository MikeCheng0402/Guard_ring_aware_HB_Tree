#include <iostream>
#include <fstream>
#include "readfile.h"

using namespace std;

vector<Node> ReadFile_test(char * filename)
{
    ifstream ifile(filename, ios::in);
    string word;

    vector<Node> file_data;
    int id = 0;

    if(!ifile)
    {
        cout << "The Spice file " << filename << " can not be opened!!!" << endl;
        ifile.close();
        return file_data;
    }
    else
    {
        ifile >> word;
        while (word != ".end")
        {
            while(word != "nodeEnd" && word != ".end")
                ifile >> word;
            
            while(word != ".end")
            {
                string name, drain, source, gate, body, type;
                int fin, l, m;
                bool common;

                ifile >> word;
                if(word == ".ends") 
                    break;
                
                name = word;
                ifile >> drain >> gate >> source >> body >> type;

                ifile >> word;
                word = word.substr(4, word.size()-4);
                fin = atoi(word.c_str());

                ifile >> word;
                word = word.substr(2, word.size()-3);
                l = int(atof(word.c_str()) * 1000);

                ifile >> word;
                word = word.substr(2, word.size()-2);
                m = atoi(word.c_str());

                ifile >> word >> word;
                word = word.substr(7, word.size()-7);
                common = atoi(word.c_str());


                Cell cell_temp(name, drain, source, gate, body, type, fin, l, m, 0, common);

                cout << name << " " << drain << " " << gate << " " << source << " " << body << " " << type
         << " " << fin << " " << l << " " << m << " " << common << endl;

                Node node_temp(id, cell_temp);
                file_data.push_back(node_temp);
                cout << id << " " << file_data.size() << endl;

                id++;
            }
        }

        ifile.close();
    }

    return file_data;
}

vector<vector<vector<Node> > >ReadFile(char *filename)
{
    ifstream ifile(filename, ios::in);
    string word;

    vector<vector<vector<Node> > > data_tmp;
    //vector<vector<Node> >file_data;
    int id = 0;

    if(!ifile)
    {
        cout << "The Spice file " << filename << " can not be opened!!!" << endl;
        ifile.close();
        return data_tmp;
    }
    else
    {
        ifile >> word;
        while(word != ".end")
        {
            //vector<vector<Node> > block_data;
            while(1)
            {
                //vector<Node> node_data;
                while(word != "nodeEnd" && word != ".end")
                {
                    ifile >> word;
                }

                if(word == ".end")
                    break;

                ifile >> word;
                while(word != ".ends")
                {
                    string name, drain, source, gate, body, type;
                    int fin, l, m;
                    bool common, symmetry;
                    
                    name = word;
                    ifile >> drain >> gate >> source >> body >> type;

                    ifile >> word;
                    word = word.substr(4, word.size()-4);
                    fin = atoi(word.c_str());

                    ifile >> word;
                    word = word.substr(2, word.size()-3);
                    l = int(atof(word.c_str()) * 1000);

                    ifile >> word;
                    word = word.substr(2, word.size()-2);
                    m = atoi(word.c_str());

                    ifile >> word >> word;
                    word = word.substr(7, word.size()-7);
                    common = atoi(word.c_str());
                    
                    if(symmetry_check[name] == 2)
                        symmetry = 1;
                    else
                        symmetry = 0;

                    Cell cell_temp(name, drain, source, gate, body, type, fin, l, m, symmetry, common);

                    Node node_temp(id, cell_temp);

                    All_Node.push_back(node_temp);
                    id_check[node_temp.get_cell_name()] = id;

                    id++;
                    ifile >> word;
                }
            }
        }

        if(proximity_group.empty())
        {
            vector<string> pmos_group;
            vector<string> nmos_group;

            for(int i=0; i<All_Node.size(); i++)
            {
                const string cell_name = All_Node[i].get_cell_name();
                if(symmetry_check[cell_name] == 1)
                    continue;

                if(!All_Node[i].Cell_info.type.empty() && All_Node[i].Cell_info.type[0] == 'p')
                    pmos_group.push_back(cell_name);
                else
                    nmos_group.push_back(cell_name);
            }

            vector<vector<string> > default_group;
            if(!pmos_group.empty())
                default_group.push_back(pmos_group);
            if(!nmos_group.empty())
                default_group.push_back(nmos_group);

            if(!default_group.empty())
                proximity_group.push_back(default_group);
        }

        cout << "Organizing input file is completed" << endl;
        for(int i=0; i<proximity_group.size(); i++)
        {
            vector<vector<Node> > mos_type_tmp;
            for(int j=0; j<proximity_group[i].size(); j++)
            {
                vector<Node> mos_tmp;
                for(int k=0; k<proximity_group[i][j].size(); k++)
                {
                    mos_tmp.push_back(All_Node[id_check[proximity_group[i][j][k]]]);
                }
                mos_type_tmp.push_back(mos_tmp);
            }
            data_tmp.push_back(mos_type_tmp);
        }

        ifile.close();
    }

    /*for(int i=0; i<data_tmp.size(); i++)
    {
        for(int j=0; j<data_tmp[i].size(); j++)
        {
            for(int k=0; k<data_tmp[i][j].size(); k++)
            {
                cout << data_tmp[i][j][k].get_cell_name() << endl;
            }
        }
    }*/

    return data_tmp;
}

void Read_symmetry(char *filename)
{
    ifstream ifile(filename, ios::in);
    string word;
    number_of_symmetry_node = 0;

    while(ifile >> word)
    {
        string transistor1 = word;
        symmetry_check[word] = 1;
        number_of_symmetry_node++;

        ifile >> word;
        string transistor2 = word;
        symmetry_check[word] = 2;
        number_of_symmetry_node++;

        symmetry_group[transistor1] = transistor2;
        symmetry_group[transistor2] = transistor1;
    }

    ifile.close();

    return ;
}

void Read_proximity(char *filename)
{
    ifstream ifile(filename, ios::in);
    string word;

    ifile >> word;

    while(word == ".proximity_group")
    {
        vector<vector<string> > mos_type_tmp;
        ifile >> word;
        if(word == ".pmos")
        {
            vector<string> mos_tmp;
            while(ifile >> word)
            {
                if(word == ".end_pmos")
                    break;
                if(symmetry_check[word] != 1)
                {
                    mos_tmp.push_back(word);
                }
            }
            mos_type_tmp.push_back(mos_tmp);
            ifile >> word;
        }


        if(word == ".nmos")
        {
            vector<string> mos_tmp;
            while(ifile >> word)
            {
                if(word == ".end_nmos")
                    break;
                if(symmetry_check[word] != 1)
                {
                    mos_tmp.push_back(word);
                }
            }
            mos_type_tmp.push_back(mos_tmp);
            ifile >> word;
        }
        
        proximity_group.push_back(mos_type_tmp);
    }

    /*for(int i=0; i<proximity_group.size(); i++)
    {
        cout << "proximity group " << i << endl;
        for(int j=0; j<proximity_group[i].size(); j++)
        {
            cout << "mos type " << j << endl;
            for(int k=0; k<proximity_group[i][j].size(); k++)
            {
                cout << proximity_group[i][j][k] << endl;
            }
            cout << endl;
        }
    }*/

    ifile.close();

    return ;
}

void CreateOutputFile(char *filename, FHBTree &fhbtree)
{
    ofstream ofile(filename, ios::out);

    if(ofile.is_open())
    {
        ofile << All_Node.size() + guard_ring_info.size() << endl;

        if(fhbtree.overall_width > fhbtree.overall_height)
            ofile << fhbtree.overall_width << " " << fhbtree.overall_width << endl;
        else
            ofile << fhbtree.overall_height << " " << fhbtree.overall_height << endl;

        // ofile << "0 0 0 " << fhbtree.overall_width << " " << fhbtree.overall_height << " ." << endl;
        /*for(int i=0; i<guard_ring_info.size(); i++)
        {
            ofile << 0 << " ";
            ofile << guard_ring_info[i].first.first << " " << guard_ring_info[i].first.second << " ";
            ofile << guard_ring_info[i].second.first << " " << guard_ring_info[i].second.second << " ";
            ofile << " ." << endl;
        }*/
        for(int i=0; i<All_Node.size(); i++)
        {
            ofile << 0 << " ";
            ofile << guard_ring_info[All_Node[i].get_cell_name()].first.first << " " << guard_ring_info[All_Node[i].get_cell_name()].first.second << " ";
            ofile << guard_ring_info[All_Node[i].get_cell_name()].second.first << " " << guard_ring_info[All_Node[i].get_cell_name()].second.second << " ";
            ofile << All_Node[i].get_cell_name() << endl;
        }
        for(int i=0; i<All_Node.size(); i++)
        {
            ofile << 1 << " ";
            ofile << All_Node[i].Cell_info.x << " " << All_Node[i].Cell_info.y << " ";
            ofile << All_Node[i].Cell_info.width << " " << All_Node[i].Cell_info.length << " ";
            ofile << All_Node[i].get_cell_name() << endl;
        }
    }
    ofile.close();

    return;
}
