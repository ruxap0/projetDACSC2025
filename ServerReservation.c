#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./protocols/lib_socket.h"
#include "./protocols/CBP.h"
#include <signal.h>
#include <pthread.h>
#include <mysql.h>

int portServeur;
int poolServer;
extern MYSQL* connexion;

void readText(int*, int*);
void handlerSIGINT(int sig);
void traitementConnexion(int sService);
void* fctThread(void* param);

int sEcoute;

int main(int argc,char* argv[])
{
    readText(&portServeur, &poolServer);

    // se connecter 1 fois à la db et use mutex pour les requetes
    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion,"localhost","Student","PassStudent1_","PourStudent",0,0,0) == NULL)
    {
        fprintf(stderr,"(SERVEUR) Erreur de connexion à la base de données...\n");
        exit(1);  
    }


    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = handlerSIGINT;
    sigaction(SIGINT,&A,NULL);

    if((sEcoute = ServerSocket(portServeur)) < 0)
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
    int sService = *((int*)param);
    free(param);

    traitementConnexion(sService);

    close(sService);
    printf("Fermeture de la connexion avec le client %d\n", sService);
    pthread_exit(NULL);
}

void traitementConnexion(int sService)
{
    char requete[256];
    char reponse[256];
    int ret;
    while(1)
    {
        memset(reponse, 0, 256);

        if((ret = Receive(sService, requete)) < 0)
        {
            perror("Erreur de Receive");
            return;
        }
        if(ret == 0) // client déconnecté
            return;

        printf("Requête reçue : %s\n", requete);
        if(CBP(requete, reponse, sService) == false) // requête inconnue
        {
            strcpy(reponse, "KO");
        }

        printf("Réponse envoyée : %s\n", reponse);
        if((ret = Send(sService, reponse, strlen(reponse))) < 0)
        {
            perror("Erreur de Send");
            return;
        }
    }
}

// --------------------------------------------------------------------------------------------------------------//

void readText(int* port, int* pool)
{
    FILE* f = fopen("configserveur.txt", "r");
    if (f == NULL)
    {
        perror("Erreur d'ouverture du fichier configserveur.txt");
        exit(1);
    }
    
    // Lit directement dans les variables pointées
    if (fscanf(f, "PORT=%d\n", port) != 1)
    {
        fprintf(stderr, "Erreur de lecture du PORT\n");
        fclose(f);
        exit(1);
    }
    
    if (fscanf(f, "POOL=%d", pool) != 1)
    {
        fprintf(stderr, "Erreur de lecture du POOL\n");
        fclose(f);
        exit(1);
    }
    
    fclose(f);
    printf("Configuration chargée: PORT=%d, POOL=%d\n", *port, *pool);
}

void handlerSIGINT(int sig)
{
    printf("\nFermeture du serveur...\n");
    close(sEcoute);
    mysql_close(connexion);
    exit(0);
}