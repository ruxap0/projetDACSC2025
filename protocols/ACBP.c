#include "ACBP.h"
#include "CBP.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>

extern CONNECTED_CLIENT* clients_connectes[NB_MAX_CLIENTS];
extern int nb_clients;
extern pthread_mutex_t mutex_clients_connectes;

bool ACBP(char* requete, char* reponse, int socket)
{
    // Récupérer les clients connectés sur le serveur CBP
    if(strcmp(requete, "LIST_CLIENTS") == 0)
    {
        pthread_mutex_lock(&mutex_clients_connectes);
        
        if(nb_clients > 0)
        {
            strcpy(reponse, "OK");
            char buffer[512];
            
            for(int i = 0; i < nb_clients; i++)
            {
                sprintf(buffer, "%d", clients_connectes[i]->patientId);
                buffer[sizeof(buffer)-1] = '\0';
                if(strcmp(clients_connectes[i]->ip, "?") != 0)
                {
                    strcat(reponse, "#");
                    strcat(reponse, clients_connectes[i]->ip);
                    strcat(reponse, "#");
                    strcat(reponse, clients_connectes[i]->lastName);
                    strcat(reponse, "#");
                    strcat(reponse, clients_connectes[i]->firstName);
                    strcat(reponse, "#");
                    strcat(reponse, buffer);
                }
            }
        }
        else
        {
            strcpy(reponse, "KO#Aucun client connecté");
        }
        
        pthread_mutex_unlock(&mutex_clients_connectes);
        return true;
    }
    
    return false;
}