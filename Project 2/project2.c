#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>


// table chopsticks
#define LEFT (i + 1 % 5)
#define RIGHT i 

// run lock philo have chopstick
// run unlock putting down
// if philo id is even go right left
// if philo id is odd go left right 

char *filename[5] = {"/tmp/file1", "/tmp/file2", 
                     "/tmp/file3", "/tmp/file4", "/tmp/file5"}; // file names
int semaphore; // file descriptor 
int philo; // create parent philosopher
int ran; // declare integer for random number

// functions below main
int lock();
int unlock();
void philosopher();
void thinking();
int hungry();
void eat();
void sated();

int main () {
    
    // delete all files before creating process 
    for(int j = 0; j < 5; j++){
        unlock(j, semaphore);
    }

    printf("*** DINNING PHILOSOPHERS PROBLEM. ***\n");
    // create philosophers 
    // printf("ParentID: %d\n", getppid()); - see the parent ID
    for(int i = 0; i < 5; i++) {
        // create random number for sleep()
        ran = (rand() % 5) + 1; 
        philo = fork();
        //child philosophers
        if(philo == 0) {
            //printf("Philo: %d\n", i); - print philosopher number
            philosopher(i);
            exit(0);
        }
        //parent philosopher
        else {
             sleep(ran);
        }
    }
    
    wait(NULL); // use this to control c instead of kill -9

  return (0);
}

int lock(int i, int semaphore) {
    semaphore = creat(filename[i], 0); // create fd (semaphore)
    close(semaphore); // close fd (sempahore)
    if(semaphore <= -1) {
        //printf("There is an error \n");
        return 0; // return 0 if the file cannot created 
    }
    if(semaphore >= 0) {
        //printf("Success");
        return 1; // return 1 if the file is created 
    }
}   

int unlock(int i, int semaphore) {
    if(semaphore >= 0) {
        unlink(filename[i]); // if the file exists delete it
    } 
}

void philosopher(int i) {
    while(1) { 
        thinking(i);
        sleep(ran);
        int tmp = hungry(i);
        if(tmp == 1) { // if hungry returns 1 then allow philo to eat and be sated
            eat(i);
            sleep(ran);
            sated(ran);
            sleep(ran);
        }
    }
}

void thinking(int i) {
    printf("Philosopher %d is thinking. \n", i);
    sleep(ran);
}

int hungry(int i) {
    printf("Philosopher %d is hungry. \n", i);  
    
    // *** logic for deadlock-free ***
    int even = (i + 1) % 2;
    int x, y;
    if(even == 0) {
        x = lock(RIGHT, semaphore);
        y = lock(LEFT, semaphore);
    }
    else {
        x = lock(LEFT, semaphore);
        y = lock(RIGHT, semaphore);
    }
    // *** logic for deadlock-free ***
    
    // if philo picks up both chopsticks
    if(x == 1 && y == 1){
        return 1;
    }
    // only got one chopstick unlock stick left
    else if(x == 0 && y == 1) {
        unlock(RIGHT, semaphore);
        printf("Philosopher %d only got right chopstick. Cannot eat. \n", i);
        return 0;
    }
    // only got one chopstick unlock stick right 
    else if(y == 0 && x == 1) {
        unlock(LEFT, semaphore);
        printf("Philosopher %d only got left chopstick. Cannot eat. \n", i);
        return 0;
    }
    // if can't pick up either chopstick
    else {
        printf("Philosopher %d cannot pick up any chopsticks. Cannot eat. \n", i);
        return 0;
    }
    sleep(ran);
}

void eat(int i) {
    printf("Philosopher %d is eating. \n", i);
    sleep(ran);
}

void sated(int i) {
    printf("Philosopher %d is sated. \n", i);
    // unlock both chopsticks
    unlock(LEFT, semaphore);
    unlock(RIGHT, semaphore);
    sleep(ran);
}
