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

#define nc 2  // nombre de cabines 

#define np 5 //nombre de piscine

 #define ndp 0

#define npo 1

 int sem, seg ;

   int* num; 

struct sembuf pscab = {scab, -1, 0};
struct sembuf vscab = {scab, 1, 0};
struct sembuf pmutex = {mutex, -1, 0};
struct sembuf vmutex = {mutex, 1, 0};
struct sembuf pspan = {span, -1, 0};
struct sembuf vspan = {span, 1, 0};

void Demander_cabine(){
     semop(sem, &pscab, 1);
}

void Liberer_cabine(){
     semop(sem, &vscab, 1);
}

void Demander_panier(int* Nump){
     semop(sem, &pmutex, 1);
     num[ndp]=num[ndp]+1;
     if(num[npo]==np){
       semop(sem, &vmutex, 1); 
       semop(sem, &pspan, 1);

     }
     num[ndp]=num[ndp]-1;
    num[npo]=num[npo]+1;
     *Nump=num[npo];
     //printf("npo is here %d" ,num[npo]);
     semop(sem, &vmutex, 1);

}

void Liberer_panier(int i,int*
 Nump){
    semop(sem, &pmutex, 1);
    num[npo]=num[npo]-1;
    *Nump=num[npo];
    if(num[ndp]>0){
        printf("nageur %d va liberer un panier et il ya %d demandes en attente\n",i,num[ndp]);
semop(sem, &vspan, 1);
    }
    else{
        semop(sem, &vmutex, 1);
    }
}



int main(int argc, char *argv[]) 
{
     char *ch = argv[1];
     int i=atoi(ch);//recuperation de i
key_t cle;

    union semun { int val;  struct semid_ds *buf;  unsigned short *array;
    } arg;

   

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

  

//--------------------programme du nageur----------------------------
int Nump ;

//printf("TP2 SYC REZZOUG AICHA BELHARDA AYA GROUPE 1 ");
Demander_panier(&Nump);
sleep(4);
Demander_cabine();
Liberer_cabine();
sleep(7);
printf(" \nje suis le nageur numero %d  j'occupe le panier %d \n",i,Nump);
Demander_cabine();
sleep(4);
Liberer_cabine();
Liberer_panier(i,&Nump); //le i est passé en parametres 
printf(" \nje suis le nageur numero %d  j'ai libere un panier et il reste %d paniers libres\n",i,np-Nump);
}