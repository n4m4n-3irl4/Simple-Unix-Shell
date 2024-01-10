#include <stdio.h>
#include <stdlib.h>
#include <threads.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <ctype.h>
#include <signal.h>
#include <time.h>

struct process_details{
    int is_pipe;
    int num_commands;
    pid_t process_pid[100];
    time_t process_start[100];
    time_t process_end[100];
    long double process_exec[100];
    char command[100];
};

char dp[100][300];
int dp_ind = 0;
struct process_details exec_history[100];
int proc_no=0;

void print(struct process_details hehe[],int size){
    for(int i=0;i<size;i++){
        if(hehe[i].num_commands==1){
            printf("\nNum: %d  Command: %s  Pid: %d  \nStart: %sExecutionTime: %Lfs\n",i+1,hehe[i].command,hehe[i].process_pid[0],ctime(&hehe[i].process_start[0]),ctime(&hehe[i].process_end[0]), hehe[i].process_exec[0]);
        }
        else if(hehe[i].is_pipe==1){
            printf("\nNum: %d  Command: %s\n",i+1,hehe[i].command);
            for(int j=0;j<hehe[i].num_commands;j++){
                printf("Pid(subprocess %d): %d  \nStart: %sExecutionTime: %Lfs\n",j+1,hehe[i].process_pid[j],ctime(&hehe[i].process_start[j]),ctime(&hehe[i].process_end[j]),hehe[i].process_exec[j]);
            }
        }
    }
}

void process_file(char *file_inp)
{
    FILE *file;
    FILE *bashCom;
    char command[100];
    bashCom = popen("/bin/bash","w");
    if(bashCom == NULL)
    {
        perror("error");
        exit(1);
    }
    file = fopen(file_inp,"r");

    if(file == NULL)
    {
        perror("error");
        exit(1);
    }

    while (fgets(command,sizeof(command),file))
    {
        fprintf(bashCom,"%s\n",command);
    }

    if(pclose(bashCom)== -1)
    {
        perror("error");
        exit(1);
    }

    if(fclose(file) == EOF)
    {
        perror("error");
        exit(1);
    }

    int status=system("make clean");
    if(status==-1){
        perror("system");
        exit(1);
    }    

}


void trim(char *str)
{
    int start = 0;
    int end = strlen(str) - 1;
    while (str[start]&&isspace(str[start])) {
        start++;
    }
    while(end>= start&&isspace(str[end])) {
        end--;
    }
    for (int i=0; i<= end-start;i++) {
        str[i]=str[start + i];
    }

    str[end-start+1] = '\0';
}

static void sig_handler(int signum){
    if(signum==SIGINT){
        char *mssg="\nExiting the shell.\n";
        print(exec_history,proc_no);
        char *command="make clean";
        int status=system(command);
        write(STDOUT_FILENO,mssg,strlen(mssg));
        if(status==-1) {
            perror("system");
            exit(1);
        };
        exit(0);
    }
}

void process()
{
    // duplicate stdin to take future inputs
    int org_stdin = dup(STDIN_FILENO);
    struct sigaction sig;
    sig.sa_handler=sig_handler;
    if(sigaction(SIGINT, &sig,NULL)==-1) {
        perror("sigaction");
        exit(1);
    }

    do
    {
        printf("[n4m4n_3irl4@archlinux]$ ");
        char command_inp[100];
        char dup_inp[100];
        fgets(command_inp, sizeof(command_inp), stdin);
        command_inp[strcspn(command_inp, "\n")] = '\0';
        for(int i = 0; i < 100; i++)
            dup_inp[i] = command_inp[i];
        // printf("%s",command_inp);
        trim(command_inp);
        if(strlen(command_inp) == 0)
        {
            continue;
        }

        for(int i = 0; i < 100; i++)
        {
            dp[dp_ind][i] = dup_inp[i];
        }
        dp_ind++;
        if(strcmp(command_inp,"history") == 0)
        {
            clock_t s=clock();
            time_t start;
            time(&start);
            exec_history[proc_no].process_start[0] = start;
            for(int i = 0; i < 100; i++)
            {
                exec_history[proc_no].command[i] = dup_inp[i];
            }
            
            int hi_st = fork();
            if(hi_st == 0)exit(0);
            if(hi_st > 0)
            {
                wait();
                clock_t e=clock();
                time_t end;
                time(&end);
                exec_history[proc_no].process_pid[0] = hi_st;
                exec_history[proc_no].process_end[0] = end;
                exec_history[proc_no].is_pipe = 0;
                exec_history[proc_no].num_commands = 1;
                exec_history[proc_no].process_exec[0]=(double)(e-s)/CLOCKS_PER_SEC;
                proc_no++;
                
            }
            // printf("YES");
            for(int i = 0; i < dp_ind; i++)
                printf("%d %s\n",i+1,dp[i]);
            continue;
        }

        command_inp[strcspn(command_inp, "\n")] = '\0';
        int pipeCnt = 0;

        for(int i = 0; i < 100; i++)
        {
            if(command_inp[i] == '|')
                pipeCnt++;
        }

        int fd[pipeCnt][2]; 
        pid_t children[pipeCnt+1];
        if(pipeCnt == 0)
        {
            clock_t s=clock();
            time_t start;
            time(&start);
            for(int i = 0; i < 100; i++)
            {
                exec_history[proc_no].command[i] = dup_inp[i];
            }
            // exec_history[proc_no].command = command_inp;
            exec_history[proc_no].process_start[0]=start;
            exec_history[proc_no].is_pipe=0;
            exec_history[proc_no].num_commands=1;
            int st = fork();
            if(st < 0)
                printf("Something wrong\n");
            else if(st == 0)
            {
               
                char *token = strtok(command_inp, " ");
                char *command[20];
                int ind = 0;

                while (token != NULL) {
                    command[ind] = token;
                    ind++;
                    token = strtok(NULL, " ");
                }
                
                command[ind] = NULL; 
                char path[100] = "/usr/bin/";
                if(command_inp[0] == '.' && command_inp[1] == '/')
                {
                    char chr_path[100] = "";
                    int chrind = 0;
                    for(int i = 2; i < 100; i++)
                    {
                        chr_path[chrind] = command[0][i];
                        chrind++;
                    }

                    if(execv(chr_path,command)==-1) perror("execv");
                }
                else
                {
                    ind = 0;
                    for(int i = 9; i < 100; i++)
                    {
                        path[i] = command[0][ind];
                        ind++;
                    }
                    if(execv(path,command)==-1) perror("execv");
                }

                exit(1);

            }
            else
            {
                int status;
                wait(&status);
                clock_t e=clock();
                time_t end;
                time(&end);
                for(int i=0;i<100;i++){
                exec_history[proc_no].command[i]=dup_inp[i];
                }
                exec_history[proc_no].process_pid[0]=st;
                exec_history[proc_no].process_end[0]=end;
                exec_history[proc_no].process_exec[0]=(double)(e-s)/CLOCKS_PER_SEC;
                
            }
        }
        else
        {
            // for(int i=0;i<strlen(command_inp);i++){
            //     printf("%c",command_inp[i]);
            // }printf("\n");
            // printf("pipes:%d\n",pipeCnt);
            char* brktok = strtok(command_inp,"|");
            char* commands[100];
            int ind=0;
            while (brktok != NULL)
            {
                commands[ind] = brktok;
                brktok = strtok(NULL,"|");
                ind++;
            }

            for(int i = 0; i < ind; i++)
            {
                trim(commands[i]);
                commands[i][strcspn(commands[i], "\n")] = '\0';
                // printf("%s\n",commands[i]);
            }
            // for(int i=0;i<ind;i++) printf("Index: %d : %s\n",i,commands[i]);
            char *split[ind][100];
            int counter[100];
            for(int i=0;i<ind;i++){
                int j=0;
                char *token = strtok(commands[i], " ");
                while(token!=NULL){
                    split[i][j]=token;
                    token=strtok(NULL," ");
                    j++;
                }
                split[i][j]=NULL;j++;
                counter[i]=j;
            }

            // for(int i=0;i<ind;i++){
            //     for(int j=0;j<counter[i];j++){
            //         printf("Index: %d %d %s\n",i,j,split[i][j]);
            //     }
            // }

            // printf("ind: %d\n",ind);

            //pipescnt stores the number of pipe operators
            //ind stores the number of instructions (pipe operators+1)
            //split has the 2d array of instructions
            int prev_fd = STDIN_FILENO;
            for(int i=0;i<=pipeCnt;i++){
                exec_history[proc_no].is_pipe=1;
                exec_history[proc_no].num_commands=pipeCnt+1;
                if(pipe(fd[i])==-1){
                    perror("pipe");
                    exit(1);
                };
                clock_t s=clock();
                time_t start;
                time(&start);
                exec_history[proc_no].process_start[i]=start;
                for(int i = 0; i < 100; i++)
                {
                exec_history[proc_no].command[i] = dup_inp[i];
                }

                children[i]=fork();
                // printf("hello\n")
                if(children[i]==0){
                    // printf("%d Child\n",i);
                    // if(i==0) {dup2(fd[0][0],STDIN_FILENO);close(fd[0][0]);}
                    if(i<pipeCnt){
                        if(close(fd[i][0])==-1) {
                            perror("close");
                            exit(1);
                        }
                        if(dup2(fd[i][1],STDOUT_FILENO)==-1){
                            perror("dup2");
                            exit(1);
                        };
                    }
                    // if (i > 0) {
                    //     close(fd[i-1][1]);
                    //     dup2(fd[i-1][0], STDIN_FILENO);
                    // }
                    // printf("exec\n");
                    char ch_path[100];
                    if(split[i][0][0]=='.'&& split[i][0][1]=='/'){
                        char ch_path[100]="";
                        int temp=0;
                        for(int j=2;j<100;j++){
                            ch_path[temp]=split[i][0][j];
                            temp++;
                        }
                        // printf("%s\n",ch_path);
                        if(execv(ch_path, split[i])==-1){
                            perror("execv");
                        };
                    }
                    else{
                        
                        char ch_path[100]="/usr/bin/";
                        int temp=0;
                        for(int k=9;k<100;k++){
                            ch_path[k]=split[i][0][temp];
                            temp++;
                        }
                        // printf("lmaooo\n");
                        // printf("%s\n",ch_path);
                        // printf("hehehe\n");
                        if(execv(ch_path, split[i])==-1){
                            perror("execv");
                        }
                    }
                    exit(0);
                }
                
                else{
                    if (i < pipeCnt) {
                        if(close(fd[i][1])==-1){
                            perror("close");
                            exit(1);
                        };
                    }
                    if (i > 0) {
                        if(close(fd[i - 1][0])==-1){
                            perror("close");
                            exit(1);
                        };
                    }
                    
                    // prev_fd = fd[i][0];
                    if(dup2(fd[i][0],STDIN_FILENO)==-1){
                        perror("dup2");
                        exit(1);
                    };
                    
                    waitpid(children[i],NULL,0);
                    clock_t e=clock();
                    time_t end;
                    time(&end);
                    exec_history[proc_no].process_end[i]=end;
                    exec_history[proc_no].process_exec[i]=(double)(e-s)/CLOCKS_PER_SEC;
                    exec_history[proc_no].process_pid[i]=children[i];

                    // close(fd[i][1]); 
                }

            }
        }
        // reset stdin)
        
        proc_no++;
        
        if(dup2(org_stdin,STDIN_FILENO)==-1){
            perror("dup2");
            exit(1);
        };
        
    }
    while (1);
    
}


int main(int argc,char* argv[])
{
    // printf("%s\n",argc);
    if(argc == 1)
        process();
    else
        process_file(argv[1]);

    return 0;
}