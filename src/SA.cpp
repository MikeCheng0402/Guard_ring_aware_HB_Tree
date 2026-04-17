#include <iostream>
#include <math.h>
#include <algorithm>
#include <time.h>
#include "SA.h"

using namespace std;

void Simulated_Annealing(FHBTree &current_tree) 
{
	double best_cost = current_tree.get_area();
	double old_cost = current_tree.get_area();
	double T0 = fabs(best_cost * (All_Node.size() - number_of_symmetry_node/2) / log(0.95));             //initial temprature
    int Op = 0;                      //M：M1、M2、M3
    int MT = 0;                     //移動次數
    int uphill = 0;                 //uphill
    int reject = 0;                 //reject
    int N = K*(All_Node.size() - number_of_symmetry_node/2)*2/3;           //N=K*n
	double T = T0; 
    int iteration = 0;
    int stage = 1;
    double average_delta_cost = 0;

    time_t t1, t2, t3;
    t1 = time(NULL);
	 
	FHBTree new_tree;
	FHBTree best_tree;

	new_tree = current_tree;
	best_tree = current_tree;

    vector<int> record;
    record.resize(2);
    int round;

    left_check = &(new_tree.FH_node[1].TH_Btree.TH_node[1].left_child);

	do{
        reject = 0;             //初始化
        MT = 0;
        uphill = 0;

        do{
            Op = rand()%100;   //任意做M1、M2或M3
            round = rand()%10 + 1;
            for(int i=0; i<record.size(); i++)
                record[i] = -1;
            int current_round = 0;

            // cout <<"##################################################" << endl;
            while(current_round < round)
            {
                if(Op <= PROBABILITY_FHBTREE)
                {
                    // cout << "Do FHBTree perturbation" << endl;
                    current_round = round;
                    new_tree.perturb();
                }
                else if(PROBABILITY_FHBTREE < Op && Op <= PROBABILITY_THBTREE)
                {
                    // cout << "Do THBTree perturbation" << endl;
                    current_round = round;
                    new_tree.perturb_THBTtree();
                }
                else if(PROBABILITY_THBTREE < Op && Op <= PROBABILITY_HBTREE)
                {
                    // cout << "Do HBTree perturbation" << endl;
                    new_tree.perturb_HBTree(record);
                }
                else if(PROBABILITY_HBTREE < Op && Op <= PROBABILITY_ASFBTREE)
                {
                    // cout << "Do ASF_Tree perturbation" << endl;
                    new_tree.perturb_ASF_Tree(record);
                }

                // cout << "Perturbation is completed" << endl;

                MT++;               //移動次數++
                new_tree.clean_contour();
                new_tree.clean_overall_width_height();
                // cout << "Do placement" << endl;
                new_tree.placement(&new_tree.overall_width, &new_tree.overall_height);

                // cout << "Placement is completed" << endl;

                double new_cost = new_tree.get_area();              //移動後的cost
                double delta_cost = new_cost - old_cost;            //cost的變化
                double probability = (double)rand()/RAND_MAX;       //隨機在0到1之間取值

                double aspect_ratio = (double)new_tree.overall_width / (double)new_tree.overall_height;
                
                if(delta_cost<=0 || probability<exp(-1*delta_cost/T))   //如果cost變化為負或是probability小於exp(-delta_cost/T)
                {
                    if(delta_cost > 0)                      //如果cost變化量大於0，uphill++
                        uphill++;
                    current_tree = new_tree;                //update NPE

                    old_cost = current_tree.get_area();   
                    if(new_cost < best_cost /*&& aspect_ratio >= 0.5 && aspect_ratio <= 2.0*/)    //如果新的花費比best花費更小
                    {
                        best_tree = new_tree;               //update best
                        best_cost = new_tree.get_area();    //update best_cost
                        cout << "Best area is " << best_cost << endl;
                        cout << "Aspect ratio is " << aspect_ratio << endl;

                        t3 = time(NULL);
                    }
                }
                else
                    reject++;

                current_round++;
            }

            new_tree = current_tree;

            t2 = time(NULL);

            if((t2-t1)>=TIME_LIMIT || (t2-t3)>=TIME_LIMIT2)         //檢查是否超時
                break;

        } while(uphill<=N && MT<=2*N);
        
        if(T>30)        //若T小於30，則讓其快速冷卻
            T = RATIO*T;
        else
            T = FROZEN_RATIO*T;
        
    } while((reject/MT)<=0.95 && T>=EPSILON && (t2-t1)<TIME_LIMIT && (t2-t3)<TIME_LIMIT2);

    // cout << "width = " << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].Cell_info.width << endl;
    // cout << "height = " << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].Cell_info.length << endl;
    // cout << "contour = " << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].ASF_BTree.contour.size() << endl;
    // cout << best_tree.FH_node[0].Cell_info.x << " " << best_tree.FH_node[0].Cell_info.y << endl;
    // cout << best_tree.FH_node[0].TH_Btree.TH_node[0].Cell_info.x << " " << best_tree.FH_node[0].TH_Btree.TH_node[0].Cell_info.y << endl;
    // cout << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].Cell_info.x << " " << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].Cell_info.y << endl;
    // cout << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].ASF_BTree.nodes[0].Cell_info.x << " " << best_tree.FH_node[0].TH_Btree.TH_node[0].H_BTree.Hnodes[0].ASF_BTree.nodes[0].Cell_info.y << endl;

    current_tree = best_tree;
	cout << "Best Area = " << current_tree.get_area() << endl;
}