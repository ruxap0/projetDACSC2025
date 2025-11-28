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
                // Formater l'heure de connexion
                char timeStr[64];
                struct tm *tm_info = localtime(&clients_connectes[i]->connectionTime);
                strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);
                
                // Construire la ligne pour ce client
                // Format: #IP#Nom#Prénom#ID#Logged#Heure
                sprintf(buffer, "#%s#%s#%s#%d#%s#%s",
                    clients_connectes[i]->ip,
                    clients_connectes[i]->lastName,
                    clients_connectes[i]->firstName,
                    clients_connectes[i]->patientId,
                    clients_connectes[i]->isLogged ? "OUI" : "NON",
                    timeStr);
                
                strcat(reponse, buffer);
            }
        }
        else
        {
            strcpy(reponse, "KO#Aucun client connecté");
        }
        
        pthread_mutex_unlock(&mutex_clients_connectes);
        return true;
    }
    
    // Autres commandes ACBP à ajouter ici...
    
    return false;
}