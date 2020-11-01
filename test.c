#include "src/shrmem.h"
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h> 
#include <sys/types.h>
#include <string.h>

void handler(int signum){}

int main(void){

    pid_t pid = fork();
    if(pid != 0){
        init_shrmem();                                  
        char* ptr = get_shrmem(1, 256*sizeof(char));   
        sprintf(ptr, "The quick brown fox");
        printf("[SENT] %s\n", ptr);

        __atomic_thread_fence(__ATOMIC_ACQUIRE); 
        signal(SIGCONT, &handler);
        
        kill(pid, SIGCONT);  
        pause();

        //WAKE UP
        printf("[RECEIVED] %s\n", ptr);


        deinit_shrmem();
    }
    else{
        
        signal(SIGCONT, &handler);
        pause();

        // WAKE UP
        init_shrmem();
        char* ptr = get_shrmem(1, 256*sizeof(char));
        strcat(ptr, " jumps over the lazy dog.");
        deinit_shrmem();

        __atomic_thread_fence(__ATOMIC_ACQUIRE);
        kill(0, SIGCONT);
    }
    return 0;
}