#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <mysql.h>
#include "CBP.h"

//***** Etat du protocole : liste des clients loggés ****************
int clients[NB_MAX_CLIENTS];
int nbClients = 0;
int estPresent(int socket);
int estPresentDB(int id);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexDB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexClients = PTHREAD_MUTEX_INITIALIZER;

MYSQL* connexion;

bool CBP(char *requete, char *reponse, int socket)
{
    char *ptr = strtok(requete,"#");
    

    // *********************** Login *********************** //
    if(strcmp(ptr, "LOGIN") == 0)
    {
        char* nom = strtok(NULL, "#");
        char* prenom = strtok(NULL, "#");
        int id = atoi(strtok(NULL, "#"));
        char* isNew = strtok(NULL, "#");
        
        if(strcmp(isNew, "NEW") == 0)
        {
            // vérif s'il n'existe pas déjà en DB
            if(estPresentDB(id) == 1)
            {
                strcpy(reponse, "KO");
                return true;
            }
            else
            {
                char requeteSQL[256];
                sprintf(requeteSQL, "insert into patients (id, last_name, first_name) values (%d, '%s', '%s')", id, nom, prenom);
                pthread_mutex_lock(&mutexDB);
                    if(mysql_query(connexion, requeteSQL))
                    {
                        fprintf(stderr, "(SERVEUR) Erreur de requête SQL (LOGIN NEW)...\n");
                        strcpy(reponse, "KO");
                        pthread_mutex_unlock(&mutexDB);
                        return false;
                    }
                pthread_mutex_unlock(&mutexDB);
                
                // l'ajouter dans la liste des clients loggés
                if(estPresent(socket) == -1)
                    ajoute(socket);

                strcpy(reponse, "OK");
                return true;
            }
        }
        else
        {
            char requeteSQL[256];
            sprintf(requeteSQL, "select * from patients where id = %d AND last_name = '%s' AND first_name = '%s'", id, nom, prenom);

            pthread_mutex_lock(&mutexDB);
                if(mysql_query(connexion, requeteSQL))
                {
                    fprintf(stderr, "(SERVEUR) Erreur de requête SQL (LOGIN NOT)...\n");
                    exit(1);
                }

                MYSQL_RES* resultatSQL = mysql_store_result(connexion);

                if(resultatSQL == NULL)
                {
                    fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (LOGIN NOT)...\n");
                    exit(1);
                }

                if(mysql_num_rows(resultatSQL) > 0)
                {
                    // l'ajouter dans la liste des clients loggés
                    if(estPresent(socket) == -1)
                        ajoute(socket);

                    strcpy(reponse, "OK");
                    mysql_free_result(resultatSQL);
                    pthread_mutex_unlock(&mutexDB);
                    return true;
                }
                else
                {
                    strcpy(reponse, "KO");
                    mysql_free_result(resultatSQL);
                    pthread_mutex_unlock(&mutexDB);
                    return true;
                }
        }
    }

    if(strcmp(ptr, "LOGOUT") == 0)
    {
        // le retirer de la liste des clients loggés
        retire(socket);
        strcpy(reponse, "OK");
        return true;
    }

    if(strcmp(ptr, "GET_SPECIALTIES") == 0)
    {

    }

    if(strcmp(ptr, "GET_DOCTORS") == 0)
    {

    }

    if(strcmp(ptr, "SEARCH_CONSULTATIONS") == 0)
    {

    }

    if(strcmp(ptr, "BOOK_CONSULTATION") == 0)
    {

    }

    return false;
}

//***** Gestion de l'état du protocole ******************************
int estPresent(int socket)
{
    int indice = -1;
    
    pthread_mutex_lock(&mutexClients);
        for(int i=0 ; i<nbClients ; i++)
            if (clients[i] == socket) { indice = i; break; }
    pthread_mutex_unlock(&mutexClients);

    return indice;
}

void ajoute(int socket)
{
    pthread_mutex_lock(&mutexClients);
        clients[nbClients] = socket;
        nbClients++;
    pthread_mutex_unlock(&mutexClients);
}
void retire(int socket)
{
    int pos = estPresent(socket);
    
    if (pos == -1) 
        return;

    pthread_mutex_lock(&mutexClients);
        for (int i=pos ; i<=nbClients-2 ; i++)
            clients[i] = clients[i+1];
        nbClients--;
    pthread_mutex_unlock(&mutexClients);
}

int estPresentDB(int id)
{
    char requeteSQL[256];
    int ret;

    sprintf(requeteSQL, "select * from patients where id = %d", id);
    
    pthread_mutex_lock(&mutexDB);
        if(mysql_query(connexion, requeteSQL))
        {
            fprintf(stderr, "(SERVEUR) Erreur de requête SQL (LOGIN NEW)...\n");
            exit(1);
        }

        MYSQL_RES* resultatSQL = mysql_store_result(connexion);

        if(resultatSQL == NULL)
        {
            fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (LOGIN NEW)...\n");
            exit(1);
        }

        if(mysql_num_rows(resultatSQL) > 0)
            ret = 1;
        else
            ret = 0;
    
        mysql_free_result(resultatSQL);
    pthread_mutex_unlock(&mutexDB);
    
    return ret;
        
} 

//***** Fin prématurée **********************************************
void CBP_Close()
{
    pthread_mutex_lock(&mutexClients);
        for (int i=0 ; i<nbClients ; i++)  
            close(clients[i]);
    pthread_mutex_unlock(&mutexClients);
}
