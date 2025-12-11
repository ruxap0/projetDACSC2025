// protocols/CBP.h
#ifndef CBP_H
#define CBP_H

#include <mysql.h>

#define NB_MAX_CLIENTS 100  // adapt to your config

typedef struct {
    int socket;
    char ip[64];
    char lastName[64];
    char firstName[64];
    int patientId;
    time_t connectionTime;
} CONNECTED_CLIENT;

/* Protocol handler */
bool CBP(char *requete, char *reponse, int socket);
void CBP_Close(void);

/* Shared client management - implemented in protocols/CBP.c */
extern CONNECTED_CLIENT* clients_connectes[NB_MAX_CLIENTS];
extern int nb_clients;
extern pthread_mutex_t mutex_clients_connectes;

void ajouterClient(int socket, const char* ip, const char* lastName, const char* firstName, int patientId);
void mettreAJourClient(int socket, const char* lastName, const char* firstName, int patientId);
void retirerClient(int socket);
int trouverClientParSocket(int socket);

/* Socket-level helpers (if used elsewhere) */
int estPresent(int socket);
void ajoute(int socket);
void retire(int socket);

/* DB helper */
int estPresentDB(int id);

#endif /* CBP_H */
