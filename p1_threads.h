#ifndef __P1_THREADS
#define __P1_THREADS

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <pthread.h>
#include <vector>
#include <string>
#include <cmath>
#include <iomanip>

#include "p1_process.h"

using namespace std;

typedef struct {
    string id;
    int prec;
    double grade;
} studentData;

typedef struct {
    int part_id;
    int num_threads;
    bool isLast;
    vector<studentData>* original;
    vector<studentData>* sorted;
    double sum;
} parts;

typedef struct {
    vector<studentData>* s;
    string fname;
} outputs;

typedef struct {
    vector<studentData>* s;
    double sum;
    double avg;
    double med;
    double stdv;
} statistics;

vector<studentData>* FileIn(string fname);

void StatsOut (string fname, statistics* s);

void sortedOut (outputs* o);

void CalculateStats (statistics* s);

void* FindSum (void* param);

void create_threads(string fname, int num_threads);

void* sorting(void* param);

vector<studentData>* mergeSort(vector<studentData>* S);

vector<studentData>* merge(vector<studentData>* left, vector<studentData>* right);

#endif
