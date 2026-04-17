#include <iostream>
#include <fstream>
#include <time.h>
#include "readfile.h"
#include "FHBTree.h"
#include "SA.h"

using namespace std;

int main(int argc, char *argv[])
{
    clock_t t1, t2;
    t1 = clock();

    FHBTree fhbtree;
    char *spice_file = nullptr;
    char *symmetry_file = nullptr;
    char *proximity_file = nullptr;
    char *output_file = nullptr;

    if(argc == 3)
    {
        spice_file = argv[1];
        output_file = argv[2];
    }
    else if(argc == 5)
    {
        spice_file = argv[1];
        symmetry_file = argv[2];
        proximity_file = argv[3];
        output_file = argv[4];
    }
    else
    {
        cout << "Command Error!!!" << endl;
        cout << "./exe_file spice_file output_file" << endl;
        cout << "./exe_file spice_file symmetry_file proximity_file output_file" << endl;
    }

    if(spice_file != nullptr && output_file != nullptr)
    {
        srand(SEED);
        if(symmetry_file != nullptr && proximity_file != nullptr)
        {
            Read_symmetry(symmetry_file);
            Read_proximity(proximity_file);
        }
        cout << "Reading file completed" << endl;
        fhbtree.initial(ReadFile(spice_file));
        cout << "The initiation completed" << endl;
        fhbtree.placement(&fhbtree.overall_width, &fhbtree.overall_height);
        cout << "The first placement completed" << endl;
        Simulated_Annealing(fhbtree);
        cout << "Simulated annealing completed" << endl;
        fhbtree.update_final_coordinate();
        cout << "Update final coordinate" << endl;
        CreateOutputFile(output_file, fhbtree);

        /*cout << "output tree" << endl;
        fhbtree.output_tree(fhbtree.show_root());*/
    }

    t2 = clock();
    cout << "\nSpend " << (t2-t1)/((double)(CLOCKS_PER_SEC)) << " s" << endl;
    

    return 0;
}
