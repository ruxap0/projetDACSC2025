// protocols/CBP.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include <string.h>
#include <signal.h>
#include <mysql.h>
#include "CBP.h"

extern MYSQL* connexion;

CONNECTED_CLIENT* clients_connectes[NB_MAX_CLIENTS];
int nb_clients = 0;
pthread_mutex_t mutex_clients_connectes = PTHREAD_MUTEX_INITIALIZER;

int clients[NB_MAX_CLIENTS];
int nbClients = 0;
int estPresent(int socket);
int estPresentDB(int id);
void ajoute(int socket);
void retire(int socket);

pthread_mutex_t mutexDB = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutexClientsSockets = PTHREAD_MUTEX_INITIALIZER;

void ajouterClient(int socket, const char* ip, const char* lastName, const char* firstName, int patientId)
{
    pthread_mutex_lock(&mutex_clients_connectes);
    if(nb_clients < NB_MAX_CLIENTS)
    {
        CONNECTED_CLIENT* c = (CONNECTED_CLIENT*)malloc(sizeof(CONNECTED_CLIENT));
        if(c == NULL) { perror("malloc"); exit(1); }
        
        // Toujours initialiser la socket
        c->socket = socket;
        
        // Initialiser l'IP
        if(strcmp(ip, "?") != 0)
        {
            strncpy(c->ip, ip, sizeof(c->ip)-1);
            c->ip[sizeof(c->ip)-1] = '\0';
        }
        else
        {
            c->ip[0] = '\0';
        }
        
        // Initialiser patientId
        c->patientId = patientId;
        
        // Initialiser les noms
        if(strcmp(lastName, "?") != 0)
        {
            strncpy(c->lastName, lastName, sizeof(c->lastName)-1);
            c->lastName[sizeof(c->lastName)-1] = '\0';
            strncpy(c->firstName, firstName, sizeof(c->firstName)-1);
            c->firstName[sizeof(c->firstName)-1] = '\0';
        }
        else
        {
            c->lastName[0] = '\0';
            c->firstName[0] = '\0';
        }

        // Trouver un emplacement libre
        for(int i = 0; i < NB_MAX_CLIENTS; i++)
        {
            if(clients_connectes[i] == NULL)
            {
                clients_connectes[i] = c;
                nb_clients++;
                break;
            }
        }
    }
    else
    {
        fprintf(stderr, "(CBP) Trop de clients connectés, rejet socket %d\n", socket);
    }
    pthread_mutex_unlock(&mutex_clients_connectes);
}

void mettreAJourClient(int socket, const char* lastName, const char* firstName, int patientId)
{
    pthread_mutex_lock(&mutex_clients_connectes);
    for(int i = 0; i < NB_MAX_CLIENTS; i++)
    {
        if(clients_connectes[i] != NULL && clients_connectes[i]->socket == socket)
        {
            strncpy(clients_connectes[i]->lastName, lastName, sizeof(clients_connectes[i]->lastName)-1);
            clients_connectes[i]->lastName[sizeof(clients_connectes[i]->lastName)-1] = '\0';
            
            strncpy(clients_connectes[i]->firstName, firstName, sizeof(clients_connectes[i]->firstName)-1);
            clients_connectes[i]->firstName[sizeof(clients_connectes[i]->firstName)-1] = '\0';
            
            clients_connectes[i]->patientId = patientId;
            break;
        }
    }
    pthread_mutex_unlock(&mutex_clients_connectes);
}

/* Find index in clients_connectes by socket, return -1 if not found */
int trouverClientParSocket(int socket)
{
    int pos = -1;
    pthread_mutex_lock(&mutex_clients_connectes);
        for(int i=0; i<nb_clients; i++)
            if(clients_connectes[i] && clients_connectes[i]->socket == socket) { pos = i; break; }
    pthread_mutex_unlock(&mutex_clients_connectes);
    return pos;
}

/* ----------------- Existing CBP protocol functions (slightly adjusted) ----------------- */

bool CBP(char *requete, char *reponse, int socket)
{
    char *ptr = strtok(requete,"#");

    if(ptr == NULL) { strcpy(reponse, "KO"); return false; }

    // *********************** Login *********************** //
    if(strcmp(ptr, "LOGIN") == 0)
    {
        char* nom = strtok(NULL, "#");
        char* prenom = strtok(NULL, "#");
        char* idstr = strtok(NULL, "#");
        if(idstr == NULL) { strcpy(reponse, "KO"); return true; }
        int id = atoi(idstr);
        char* isNew = strtok(NULL, "#");
        
        if(isNew && strcmp(isNew, "NEW") == 0)
        {
            // vérif s'il n'existe pas déjà en DB
            if(estPresentDB(id) == 1)
            {
                strcpy(reponse, "KO");
                return true;
            }
            else
            {
                char requeteSQL[512];
                snprintf(requeteSQL, sizeof(requeteSQL), "insert into patients (id, last_name, first_name) values (%d, '%s', '%s')", id, nom, prenom);
                pthread_mutex_lock(&mutexDB);
                    if(mysql_query(connexion, requeteSQL))
                    {
                        fprintf(stderr, "(SERVEUR) Erreur de requête SQL (LOGIN NEW)...\n");
                        strcpy(reponse, "KO");
                        pthread_mutex_unlock(&mutexDB);
                        return false;
                    }
                pthread_mutex_unlock(&mutexDB);

                mettreAJourClient(socket, nom, prenom, id);
                    
                strcpy(reponse, "OK");
                // ajouter dans liste sockets si nécessaire
                if(estPresent(socket) == -1)
                {
                    ajouterClient(-2, "?", nom, prenom, id);
                    ajoute(socket);
                }
                return true;
            }
        }
        else
        {
            char requeteSQL[512];
            snprintf(requeteSQL, sizeof(requeteSQL), "select * from patients where id = %d AND last_name = '%s' AND first_name = '%s'", id, nom, prenom);

            pthread_mutex_lock(&mutexDB);
                if(mysql_query(connexion, requeteSQL))
                {
                    fprintf(stderr, "(SERVEUR) Erreur de requête SQL (LOGIN NOT)...\n");
                    pthread_mutex_unlock(&mutexDB);
                    strcpy(reponse, "KO");
                    return false;
                }

                MYSQL_RES* resultatSQL = mysql_store_result(connexion);
            pthread_mutex_unlock(&mutexDB);

            if(resultatSQL == NULL)
            {
                fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (LOGIN NOT)...\n");
                strcpy(reponse, "KO");
                return false;
            }

            if(mysql_num_rows(resultatSQL) > 0)
            {
                mettreAJourClient(socket, nom, prenom, id);
                // l'ajouter dans la liste des clients loggés
                if(estPresent(socket) == -1)
                    ajoute(socket);

                strcpy(reponse, "OK");
                mysql_free_result(resultatSQL);
                return true;
            }
            else
            {
                strcpy(reponse, "KO");
                mysql_free_result(resultatSQL);
                return true;
            }
        }
    }

    if(strcmp(ptr, "LOGOUT") == 0)
    {
        retire(socket);
        strcpy(reponse, "OK");
        return false;
    }

     if(strcmp(ptr, "GET_SPECIALTIES") == 0)
    {
        char requeteSQL[256];

        pthread_mutex_lock(&mutexDB);
            sprintf(requeteSQL, "select id, name from specialties");

            if(mysql_query(connexion, requeteSQL))
            {
                fprintf(stderr, "(SERVEUR) Erreur de requête SQL (GET_SPECIALTIES)...\n");
                exit(1);
            }

            MYSQL_RES* resultatSQL = mysql_store_result(connexion);
        pthread_mutex_unlock(&mutexDB);

        printf("Requête SQL : %s\n", requeteSQL);

        if(resultatSQL == NULL)
        {
            fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (GET_SPECIALTIES)...\n");
            exit(1);
        }

        int nbLignes = mysql_num_rows(resultatSQL);
        
        if(nbLignes > 0)
        {
            strcpy(reponse, "OK");
            for(int i=0 ; i<nbLignes ; i++)
            {
                MYSQL_ROW ligne = mysql_fetch_row(resultatSQL);
                strcat(reponse, "#");
                strcat(reponse, ligne[0]);
                strcat(reponse, "#");
                strcat(reponse, ligne[1]);
            }
        }
        else
        {
            strcpy(reponse, "KO");
        }

        printf("Réponse : %s\n", reponse);

        mysql_free_result(resultatSQL);

        return true;
    }

    if(strcmp(ptr, "GET_DOCTORS") == 0)
    {
        char requeteSQL[256];

        pthread_mutex_lock(&mutexDB);
            sprintf(requeteSQL, "select * from doctors");

            if(mysql_query(connexion, requeteSQL))
            {
                fprintf(stderr, "(SERVEUR) Erreur de requête SQL (GET_DOCTORS)...\n");
                exit(1);
            }

            MYSQL_RES* resultatSQL = mysql_store_result(connexion);
        pthread_mutex_unlock(&mutexDB);

        if(resultatSQL == NULL)
        {
            fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (GET_DOCTORS)...\n");
            exit(1);
        }

        int nbLignes = mysql_num_rows(resultatSQL);
        
        if(nbLignes > 0)
        {
            strcpy(reponse, "OK");
            for(int i=0 ; i<nbLignes ; i++)
            {
                MYSQL_ROW ligne = mysql_fetch_row(resultatSQL);
                strcat(reponse, "#");
                strcat(reponse, ligne[1]);
                strcat(reponse, "#");
                strcat(reponse, ligne[2]);
                strcat(reponse, "#");
                strcat(reponse, ligne[3]);
            }
        }
        else
        {
            strcpy(reponse, "KO");
        }

        mysql_free_result(resultatSQL);

        return true;
    }

    if(strcmp(ptr, "SEARCH_CONSULTATIONS") == 0)
    {
        char* specialty = strtok(NULL, "#");
        char* doctor = strtok(NULL, "#");
        char* startDate = strtok(NULL, "#");
        char* endDate = strtok(NULL, "#");

        char requeteSQL[512];
        sprintf(requeteSQL, ""
            "SELECT c.id, s.name, CONCAT(d.last_name, ' ', d.first_name), c.date, c.hour "
            "FROM consultations c "
            "JOIN doctors d ON c.doctor_id = d.id "
            "JOIN specialties s ON d.specialty_id = s.id "
            "WHERE c.patient_id IS NULL "
            "AND c.date BETWEEN '%s' AND '%s'", startDate, endDate);
        
        if(strcmp(specialty, "--- TOUTES ---") != 0)
            strcat(requeteSQL, " AND s.name = '"), strcat(requeteSQL, specialty), strcat(requeteSQL, "'");
        
        if(strcmp(doctor, "--- TOUS ---") != 0)
            strcat(requeteSQL, " AND CONCAT(d.last_name, ' ', d.first_name) = '"), strcat(requeteSQL, doctor), strcat(requeteSQL, "'");

        printf("Requête SQL : %s\n", requeteSQL);

        pthread_mutex_lock(&mutexDB);
            if(mysql_query(connexion, requeteSQL))
            {
                fprintf(stderr, "(SERVEUR) Erreur de requête SQL (SEARCH_CONSULTATIONS)...\n");
                strcpy(reponse, "KO");
                pthread_mutex_unlock(&mutexDB);
                return false;
            }

            MYSQL_RES* resultatSQL = mysql_store_result(connexion);
        pthread_mutex_unlock(&mutexDB);

        if(resultatSQL == NULL)
        {
            fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (SEARCH_CONSULTATIONS)...\n");
            exit(1);
        }

        int nbLignes = mysql_num_rows(resultatSQL);
        
        if(nbLignes > 0)
        {
            memset(reponse, 0, 256);
            strcpy(reponse, "OK");
            for(int i=0 ; i<nbLignes ; i++)
            {
                MYSQL_ROW ligne = mysql_fetch_row(resultatSQL);
                strcat(reponse, "#");
                strcat(reponse, ligne[0]);
                strcat(reponse, "#");
                strcat(reponse, ligne[1]);
                strcat(reponse, "#");
                strcat(reponse, ligne[2]);
                strcat(reponse, "#");
                strcat(reponse, ligne[3]);
                strcat(reponse, "#");
                strcat(reponse, ligne[4]);
            }
        }
        else
        {
            strcpy(reponse, "KO");
            return false;
        }

        mysql_free_result(resultatSQL);

        return true;
    }

    if(strcmp(ptr, "BOOK_CONSULTATION") == 0)
    {
        int idConsultation = atoi(strtok(NULL, "#"));
        int idPatient = atoi(strtok(NULL, "#"));
        char* reason = strtok(NULL, "#");

        // vérifier que la consultation est toujours dispo
        char requeteSQL[256];
        sprintf(requeteSQL, "select * from consultations where id = %d AND patient_id IS NULL", idConsultation);

        pthread_mutex_lock(&mutexDB);
            if(mysql_query(connexion, requeteSQL))
            {
                fprintf(stderr, "(SERVEUR) Erreur de requête SQL (BOOK_CONSULTATION)...\n");
                strcpy(reponse, "KO");
                pthread_mutex_unlock(&mutexDB);
                return false;
            }

            MYSQL_RES* resultatSQL = mysql_store_result(connexion);
        pthread_mutex_unlock(&mutexDB);

        if(resultatSQL == NULL)
        {
            fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (BOOK_CONSULTATION)...\n");
            return false;
        }

        if(mysql_num_rows(resultatSQL) == 0)
        {
            mysql_free_result(resultatSQL);
            return false;
        }

        mysql_free_result(resultatSQL);

        sprintf(requeteSQL, "update consultations set patient_id = %d, reason = '%s' where id = %d", idPatient, reason, idConsultation);

        pthread_mutex_lock(&mutexDB);
            if(mysql_query(connexion, requeteSQL))
            {
                fprintf(stderr, "(SERVEUR) Erreur de requête SQL (BOOK_CONSULTATION)...\n");
                pthread_mutex_unlock(&mutexDB);
                return false;
            }
        pthread_mutex_unlock(&mutexDB);

        strcpy(reponse, "OK");
        return true;
    }
    return false;
}

/* ***** Gestion de l'état du protocole (sockets list) ************************/
int estPresent(int socket)
{
    int indice = -1;
    
    pthread_mutex_lock(&mutexClientsSockets);
        for(int i=0 ; i<nbClients ; i++)
            if (clients[i] == socket) { indice = i; break; }
    pthread_mutex_unlock(&mutexClientsSockets);

    return indice;
}

void ajoute(int socket)
{
    pthread_mutex_lock(&mutexClientsSockets);
        if(nbClients < NB_MAX_CLIENTS) {
            clients[nbClients] = socket;
            nbClients++;
        }
    pthread_mutex_unlock(&mutexClientsSockets);
}

void retire(int socket)
{
    int pos = estPresent(socket);
    
    if (pos == -1) 
        return;

    pthread_mutex_lock(&mutexClientsSockets);
        for (int i=pos ; i<=nbClients-2 ; i++)
            clients[i] = clients[i+1];
        nbClients--;
    pthread_mutex_unlock(&mutexClientsSockets);
}

void retirerClient(int socket)
{
    retire(socket);

    pthread_mutex_lock(&mutex_clients_connectes);
        for(int i = 0; i < NB_MAX_CLIENTS; i++)
        {
            if(clients_connectes[i] != NULL && clients_connectes[i]->socket == socket)
            {
                free(clients_connectes[i]);
                clients_connectes[i] = NULL;
                nb_clients--;
                break;
            }
        }
    pthread_mutex_unlock(&mutex_clients_connectes);
}

int estPresentDB(int id)
{
    char requeteSQL[256];
    int ret;

    snprintf(requeteSQL, sizeof(requeteSQL), "select * from patients where id = %d", id);
    
    pthread_mutex_lock(&mutexDB);
        if(mysql_query(connexion, requeteSQL))
        {
            fprintf(stderr, "(SERVEUR) Erreur de requête SQL (LOGIN NEW)...\n");
            pthread_mutex_unlock(&mutexDB);
            return 0;
        }

        MYSQL_RES* resultatSQL = mysql_store_result(connexion);

        if(resultatSQL == NULL)
        {
            fprintf(stderr, "(SERVEUR) Erreur de récupération du résultat SQL (LOGIN NEW)...\n");
            pthread_mutex_unlock(&mutexDB);
            return 0;
        }

        if(mysql_num_rows(resultatSQL) > 0)
            ret = 1;
        else
            ret = 0;
    
        mysql_free_result(resultatSQL);
    pthread_mutex_unlock(&mutexDB);
    
    return ret;
} 

/* ***** Fin prématurée **********************************************/
void CBP_Close()
{
    pthread_mutex_lock(&mutexClientsSockets);
        for (int i=0 ; i<nbClients ; i++)  
            close(clients[i]);
    pthread_mutex_unlock(&mutexClientsSockets);
}
