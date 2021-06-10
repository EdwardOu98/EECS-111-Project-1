#include "p1_process.h"

using namespace std;

void get_statistics(std::string class_name[], int num_processes, int num_threads) {

    // You need to create multiple processes here (each process
    // handles at least one file.)	
	
    // Each process should use the sort function which you have defined  		
    // in the p1_threads.cpp for multithread sorting of the data. 
	
    // Your code should be able to get the statistics and generate
    // the required outputs for any number of child processes and threads.
    int status, i;
    int vec_size = 5 / num_processes;
    int index = 0;
    vector< vector<string> > subs(num_processes);
    
    for(i = 0; i < num_processes; i++){
        if(i == (num_processes - 1)){
            while(index < 5){
                subs[i].push_back(class_name[index]);
                index++;
            }
        }
        else {
            while(index < (i+vec_size)){
                subs[i].push_back(class_name[index]);
                index++;
            }
        }
    }

    for(i = 0; i < num_processes; i++){
        pid_t pid = fork();
        if(pid < 0){
            printf("Fork failed!\n");
            exit(10);
        }
        else if(pid == 0){
            printf("Child process is created. (pid: %d)\n", getpid());
            // printf("Subarray content: \n");
            for(int p = 0; p < subs[i].size(); p++){
                create_threads(subs[i][p], num_threads);
                // printf("%s\n", subs[i][p].c_str());
            }
            // Sort the files
            // Calculate the stats
            exit(0);
        }
    }

    // printf("...\n");

    int j = num_processes;
    while(j > 0){
        pid_t cid = wait(&status);
        if(j == num_processes){
            printf("...\n");
        }
        printf("Child process is terminated. (pid: %d)\n", (int) cid);
        j--;
    }
}
