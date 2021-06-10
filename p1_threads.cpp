#include "p1_threads.h"

// This file includes functions that actually perform the
// computation. You can create a thread with these function
// or directly call this function

using namespace std;

vector<studentData>* FileIn(string fname){
    vector<studentData>* s = new vector<studentData>();

    ifstream fin;
    const char* path = fname.c_str();
    fin.open(path);
    if(!fin.is_open()){
        printf("Failed to open file %s\n", path);
        exit(4);
    }
    string student_id;
    string grade;
    double fgrade;

    getline(fin, student_id, '\n');

    // int i = 0;
    // while(i < 20){
    //     i++;
    while(fin.peek() != EOF){
        int digit_count = 0;
        getline(fin, student_id, ',');
        getline(fin, grade, '\n');
        for(int j = 3; j < grade.size(); j++){
            digit_count++;
        }
        fgrade = atof(grade.c_str());
        studentData sd;
        sd.id = student_id;
        sd.grade = fgrade;
        sd.prec = digit_count;
        s->push_back(sd);
    }

    fin.close();

    return s;
}

void StatsOut(string fname, statistics* s){
    ofstream fout;
    const char* path = fname.c_str();

    fout.open(path);
    if(!fout.is_open()){
        printf("Failed to open file %s\n", path);
        exit(5);
    }

    fout << "Average,Median,Std. Dev\n";
    fout << fixed << setprecision(3);
    fout << s->avg << ",";
    fout << s->med << ",";
    fout << s->stdv << "\n";

    fout.close();
}

void sortedOut(outputs* o){

    // outputs* o = (outputs*) param;

    string fname = o->fname;

    vector<studentData>* s = o->s;

    ofstream fout;
    const char* path = fname.c_str();
    fout.open(path);
    if(!fout.is_open()){
        printf("Failed to open file %s\n", path);
        exit(5);
    }

    fout << "Rank,Student ID,Grade\n";

    for(int i = 0; i < s->size(); i++){
        fout << i+1 << "," << s->at(i).id << ",";
        fout << fixed << setprecision(s->at(i).prec) << s->at(i).grade << "\n";
    }

    fout.close();
}

void create_threads(string fname, int num_threads){

    vector<studentData>* s;
    vector<parts*> pa;
    string indir = "./input/" + fname;
    string outdir = "./output/" + fname;
    string norm = ".csv";
    string sort = "_sorted.csv";
    string stats = "_stats.csv";

    string ifname = indir + norm;
    string ofname1 = outdir + sort;
    string ofname2 = outdir + stats;

    s = FileIn(ifname);

    int i, t;
    double sum = 0.0;

    pthread_t tid[num_threads];
    for(i = 0; i < num_threads; i++){
        pa.push_back(new parts());
        pa[i]->part_id = i;
        pa[i]->num_threads = num_threads;
        pa[i]->sum = 0.0;
        if(i == (num_threads-1)){
            pa[i]->isLast = true;
        }
        else{
            pa[i]->isLast = false;
        }
        pa[i]->original = s;
        t = pthread_create(&tid[i], NULL, sorting,(void*) pa[i]);
        if(t != 0){
            printf("Failed to create thread. Error code: %d\n", t);
            exit(5);
        }
    }
    for(i = 0; i < num_threads; i++){
        pthread_join(tid[i], NULL);
    }

    vector<studentData>* result = new vector<studentData>();
    for(i = 0; i < num_threads; i++){
        sum += pa[i]->sum;
        result = merge(result, pa[i]->sorted);
    }

    outputs* o = new outputs();
    o->fname = ofname1;
    o->s = result;

    statistics* st =  new statistics();
    st->s = result;
    st->sum = sum;

    // pthread_t output_thread, stats_thread;
    // pthread_create(&output_thread, NULL, sortedOut, (void*) o);
    // pthread_create(&stats_thread, NULL, CalculateStats, (void*) st);

    // pthread_join(output_thread, NULL);
    // pthread_join(stats_thread, NULL);
    sortedOut(o);
    CalculateStats(st);

    StatsOut(ofname2, st);

    
    delete s;
    delete o;
    delete st;
    delete result;
}

void CalculateStats(statistics* s){
    // statistics* s = (statistics*) param;

    int size = s->s->size();

    double avg = s->sum / ((double) size);
    s->avg = avg;

    if(size % 2 == 0){
        s->med = (s->s->at(size/2).grade + s->s->at(size/2 - 1).grade) / 2;
    }
    else{
        s->med = s->s->at(size/2).grade;
    }

    double var = 0.0;

    for(int i = 0; i < size; i++){
        var += pow((s->s->at(i).grade - avg), 2);
    }
    var /= size;
    s->stdv = sqrt(var);
}

void* sorting(void* param) {
    parts* p = (parts*) param;
    int i = p->part_id;
    int n = p->num_threads;
    int size_orig = p->original->size();
    int subsize = size_orig / n;
    int low = i * subsize;
    int high;
    if(p->isLast){
        high = size_orig - 1;
    }
    else{
        high = (i+1) * subsize - 1;
    }

    vector<studentData>* o = new vector<studentData>();
    for(int j = low; j <= high; j++){
        o->push_back(p->original->at(j));
        p->sum += p->original->at(j).grade;
    }
    p->sorted = mergeSort(o);
    o->clear();
    delete o;

    pthread_exit(0);
}

vector<studentData>* mergeSort(vector<studentData>* S) {
    if(S->size() > 1){
        vector<studentData>* left = new vector<studentData>();
        vector<studentData>* right = new vector<studentData>();
        vector<studentData>* lresult;
        vector<studentData>* rresult;
        vector<studentData>* combined;
        int mid = S->size() / 2;
        for(int i = 0; i < S->size(); i++){
            if(i < mid){
                left->push_back(S->at(i));
            }
            else {
                right->push_back(S->at(i));
            }
        }
        lresult = mergeSort(left);
        rresult = mergeSort(right);
        combined = merge(lresult, rresult);
        return combined;
    }
    return S;
}

vector<studentData>* merge(vector<studentData>* left, vector<studentData>* right) {
    vector<studentData>* result = new vector<studentData>();
    int i = 0;
    int j = 0;

    while((i < left->size()) && (j < right->size())){
        if(left->at(i).grade >= (right->at(j).grade)){
            result->push_back(left->at(i));
            i++;
        }
        else {
            result->push_back(right->at(j));
            j++;
        }
    }

    while(i < left->size()){
        result->push_back(left->at(i));
        i++;
    }

    while(j < right->size()){
        result->push_back(right->at(j));
        j++;
    }

    delete left;
    delete right;

    return result;
}
