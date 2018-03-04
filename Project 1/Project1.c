#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    
    // CREATING PIPE
    pid_t fd[2];
    pipe(fd);
    
    // CREATING FORK
    int child;
    child = fork();
    
    // CHILD 
    if(child == 0) {
        dup2(fd[0], 0); // creates  a  copy  of the file descriptor
        close(fd[0]); // close file descriptor
        close(fd[1]); // close file descriptor
        execl("/bin/more", "more", NULL); // replaces the current process image with a new one
    }
    
    // PARENT
    else {
        int number;
        int file_d = open(argv[1], O_RDONLY); // open the file
        if(file_d < 0) {
            printf("Error opening the file. \n");
        }
        
        // Create buffer
        char buffer[BUFSIZ];
        dup2(fd[1],1);
        
        // Ensure process runs at least once
        int counter = 0;
        if(counter == 0) {
            number = read(file_d, buffer, BUFSIZ);
            write(1, buffer, number);   
        }
        else {
           while(number == BUFSIZ) {
            number = read(file_d, buffer, BUFSIZ);
            write(1, buffer, number); 
           } // end while
        }
        
        // Close the file, pipe, and wait for child to close
        close(file_d);
        close(1); 
        close(fd[0]);
        close(fd[1]);
        wait(NULL);
        exit(0);
    }
    
    return 0;
}