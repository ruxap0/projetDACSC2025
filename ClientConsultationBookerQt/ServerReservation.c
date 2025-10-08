#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "lib_socket.h"
#include <signal.h>
#include <pthread.h>

void handlerSIGINT(int sig);
void traitementConnexion(int sService);
void* fctThread(void* param);

int sEcoute;

int main(int argc,char* argv[])
{
    if (argc != 2)
    {
       printf("Erreur...\n");
       printf("USAGE : Serveur portServeur\n");
       exit(1);
    }

    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = handlerSIGINT;
    sigaction(SIGINT,&A,NULL);

    if((sEcoute = ServerSocket(atoi(argv[1]))) < 0)
    {
        perror("Erreur de ServerSocket");
        exit(1);
    }

    int sService;
    pthread_t th;
    char ipClient[50];

    while(1)
    {
        printf("Attente d'une connexion...\n");

        if((sService = Accept(sEcoute, ipClient)) < 0)
        {
            perror("Erreur d'Accept");
            close(sEcoute);
            exit(1);
        }

        printf("Connexion acceptée avec %s sur %d", ipClient, sService);

        int *p = (int*)malloc(sizeof(int));
        *p = sService;
        pthread_create(&th, NULL, fctThread, (void*)p);
    }

    return 0;
}

void* fctThread(void* param)
{
    
}