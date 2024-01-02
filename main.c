#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/shm.h>
#include <unistd.h>

#define N 10 /* Nombre dde nageurs */

/* Indices des différents sémaphores */

#define scab 0

#define mutex 1

#define span 2

#define ndp 0

#define npo 1

#define nc 2  // nombre de cabines 

#define np 5 //nombre de piscine




int main(){

    key_t cle;

    union semun { int val;  struct semid_ds *buf;  unsigned short *array;
    } arg;

    int sem, seg ;

   int* num; 

    /* Initialisation des différents segments */

    /* ====================== Sémaphores  ====================== */

    cle = ftok("./main.c",1);

    if(cle == -1){
        exit(1);
    }

    sem = semget(cle, 3, IPC_CREAT | 0666);

    if (sem == -1){
        exit(2);
    }

    arg.val = 0;

    semctl(sem, span, SETVAL, arg); //init span 0 

  

    arg.val = 1;

    semctl(sem, mutex, SETVAL, arg);

    arg.val = nc;

    semctl(sem, scab, SETVAL, arg);



    /* ====================== num   ====================== */

    cle = ftok("./main.c",2);

    if (cle == -1){
        exit(3);
    }

    seg = shmget(cle, 2*sizeof(int), IPC_CREAT | 0666);

    if (seg == -1){
        exit(4);
    }

    num = (int *) shmat(seg, NULL, 0);

    if (num == NULL){
        exit(5);
    }

    num[ndp] = 0;

   num[npo] = 0;

    /* Programme du processus père (compilation des programmes enseignant et étudiant et création des processus: enseignant et N étudiants qui vont exécuter respectivement les programmes penseignant et petudiant)*/

    int p;

    p = fork();

    if (p == 0){
        execlp("gcc","gcc","Pgme_Nageur.c","-o","Pgme_Nageur",NULL);
        exit(6);
    }
    else if(p <0){
        exit(7);
    }

   
    while(wait(NULL)!=-1);

  

    for (int i=0; i < N; i++){
        char ch[20]; 
    sprintf(ch, "%d", i);
        p = fork();
        if (p == 0){
            execlp("./Pgme_Nageur","Pgme_Nageur",ch,NULL);
            exit(8);
        }
        else if(p<0){
            exit(9);
        }
    }

    while(wait(NULL)!=-1);

    /* Destruction et détachement des différents segments */

    /* ====================== Sémaphores  ====================== */

    semctl(sem, 3, IPC_RMID, 0);

    /* ====================== seg  ====================== */

    shmctl(seg, IPC_RMID, 0);
    shmdt(num);

   

    return 0;
}