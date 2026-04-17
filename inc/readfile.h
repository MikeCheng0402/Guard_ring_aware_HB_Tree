#ifndef _READ_FILE_H
#define _READ_FILE_H
#include <vector>
#include <string>
#include <map>
#include <stdlib.h>
#include "BTree.h"
#include "FHBTree.h"
#include "parameter.h"

using namespace std;

vector<Node> ReadFile_test(char *);
vector<vector<vector<Node> > > ReadFile(char *);
void Read_symmetry(char *);
void Read_proximity(char *);
void CreateOutputFile(char *, FHBTree&);

#endif