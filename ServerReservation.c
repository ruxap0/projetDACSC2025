#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./protocols/lib_socket.h"
#include "./protocols/CBP.h"
#include "./protocols/ACBP.h"
#include <signal.h>
#include <pthread.h>
#include <mysql.h>

#define taille_file_attente 20

// Mutex pour la file d'attente des sockets clients CBP
pthread_mutex_t mutex_file_sockets = PTHREAD_MUTEX_INITIALIZER;

// File d'attente circulaire pour les sockets clients CBP
int sockets_clients[NB_MAX_CLIENTS];
int indice_lecture = 0, indice_ecriture = 0;
pthread_cond_t cond_client_accept = PTHREAD_COND_INITIALIZER;

// Pool de threads workers
pthread_t* poolThreads;

// Configuration du serveur
int portServeur;
int poolServer;
int portServeurAdmin;

// Connexion MySQL
MYSQL* connexion;

// Sockets d'écoute
int sEcoute_cbp;
int sEcoute_acbp;

// Prototypes
void readText(int*, int*, int*);
void handlerSIGINT(int sig);
void traitementConnexion(int sService);
void traitementConnexionACBP(int sService);
void* fctThreadCBP(void* param);
void* fctThreadWorker(void* param);
void* fctThreadACBP(void* param);

int main(int argc, char* argv[])
{
    // Lecture de la configuration
    readText(&portServeur, &poolServer, &portServeurAdmin);

    // Initialisation des mutex et conditions
    pthread_mutex_init(&mutex_file_sockets, NULL);
    pthread_cond_init(&cond_client_accept, NULL);

    // Initialisation de la file d'attente
    for(int i = 0; i < NB_MAX_CLIENTS; i++)
        sockets_clients[i] = -1;

    // Installation du handler SIGINT
    struct sigaction A;
    A.sa_flags = 0;
    sigemptyset(&A.sa_mask);
    A.sa_handler = handlerSIGINT;
    sigaction(SIGINT, &A, NULL);

    // Connexion à la base de données MySQL
    connexion = mysql_init(NULL);
    if (mysql_real_connect(connexion, "localhost", "Student", "PassStudent1_", "PourStudent", 0, 0, 0) == NULL)
    {
        fprintf(stderr, "(SERVEUR) Erreur de connexion à la base de données...\n");
        exit(1);  
    }
    printf("Connexion à la base de données réussie\n");

    // Création du pool de threads workers pour CBP
    poolThreads = (pthread_t*)malloc(poolServer * sizeof(pthread_t));
    for(int i = 0; i < poolServer; i++)
    {
        if(pthread_create(&poolThreads[i], NULL, fctThreadWorker, NULL) != 0)
        {
            perror("Erreur de création du thread worker");
            exit(1);
        }
    }
    printf("Pool de %d threads workers créé\n", poolServer);

    // Création du thread accepteur CBP
    pthread_t threadCBP;
    if(pthread_create(&threadCBP, NULL, fctThreadCBP, NULL) != 0)
    {
        perror("Erreur de création du thread CBP");
        exit(1);
    }

    // Création du thread accepteur ACBP
    pthread_t threadACBP;
    if(pthread_create(&threadACBP, NULL, fctThreadACBP, NULL) != 0)
    {
        perror("Erreur de création du thread ACBP");
        exit(1);
    }

    // Attente des threads (ils tournent indéfiniment)
    pthread_join(threadCBP, NULL);
    pthread_join(threadACBP, NULL);

    return 0;
}

// Thread accepteur pour le protocole CBP (pool de threads)
void* fctThreadCBP(void* param)
{
    if((sEcoute_cbp = ServerSocket(portServeur)) < 0)
    {
        perror("Erreur de ServerSocket CBP");
        exit(1);
    }
    printf("Serveur CBP en écoute sur le port %d\n", portServeur);

    int sService_cbp;
    char ipClient[16];

    while(1)
    {
        printf("Attente d'une connexion CBP...\n");

        if((sService_cbp = Accept(sEcoute_cbp, ipClient)) < 0)
        {
            perror("Erreur d'Accept CBP");
            close(sEcoute_cbp);
            exit(1);
        }

        printf("Connexion CBP acceptée avec %s sur socket %d\n", ipClient, sService_cbp);

        // Ajouter le client à la liste des clients connectés
        ajouterClient(sService_cbp, ipClient, "?", "?", -1);

        // Ajouter la socket à la file d'attente pour les workers
        pthread_mutex_lock(&mutex_file_sockets);
        
        sockets_clients[indice_ecriture] = sService_cbp;
        indice_ecriture = (indice_ecriture + 1) % NB_MAX_CLIENTS;
        
        // Signaler qu'un nouveau client est disponible
        pthread_cond_signal(&cond_client_accept);
        
        pthread_mutex_unlock(&mutex_file_sockets);
    }

    return NULL;
}

// Thread worker du pool (traite les connexions CBP)
void* fctThreadWorker(void* param)
{
    int sService;

    while(1)
    {
        // Attendre qu'une socket soit disponible dans la file
        pthread_mutex_lock(&mutex_file_sockets);
        
        while(indice_lecture == indice_ecriture)
            pthread_cond_wait(&cond_client_accept, &mutex_file_sockets);
        
        // Récupérer la socket
        sService = sockets_clients[indice_lecture];
        sockets_clients[indice_lecture] = -1;
        indice_lecture = (indice_lecture + 1) % NB_MAX_CLIENTS;
        
        pthread_mutex_unlock(&mutex_file_sockets);

        // Traiter la connexion
        printf("[Worker %lu] Traitement du client socket %d\n", pthread_self(), sService);
        traitementConnexion(sService);
    }

    return NULL;
}

// Thread accepteur pour le protocole ACBP (à la demande)
void* fctThreadACBP(void *param)
{
    if((sEcoute_acbp = ServerSocket(portServeurAdmin)) < 0)
    {
        perror("Erreur de ServerSocket ACBP");
        exit(1);
    }
    printf("Serveur ACBP en écoute sur le port %d\n", portServeurAdmin);

    int sService_acbp;
    char ipClientACBP[16];

    while(1)
    {
        printf("Attente d'une connexion ACBP...\n");

        if((sService_acbp = Accept(sEcoute_acbp, ipClientACBP)) < 0)
        {
            perror("Erreur d'Accept ACBP");
            close(sEcoute_acbp);
            exit(1);
        }

        printf("Connexion ACBP acceptée avec %s sur socket %d\n", ipClientACBP, sService_acbp);

        // Traiter directement la connexion ACBP (serveur de requêtes)
        traitementConnexionACBP(sService_acbp);
        
        // Fermer la socket après traitement
        close(sService_acbp);
    }

    return NULL;
}

// Traitement d'une connexion CBP (serveur de connexions)
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
            perror("Erreur de Receive CBP");
            retirerClient(sService);
            close(sService);
            return;
        }
        
        if(ret == 0) // client déconnecté
        {
            printf("Client déconnecté (socket %d)\n", sService);
            retirerClient(sService);
            close(sService);
            return;
        }

        printf("[Socket %d] Requête reçue : %s\n", sService, requete);
        
        if(CBP(requete, reponse, sService) == false)
        {
            strcpy(reponse, "KO");
            Send(sService, reponse, strlen(reponse));
            retirerClient(sService);
            close(sService);
            return;
        }

        printf("[Socket %d] Réponse envoyée : %s\n", sService, reponse);
        
        if((ret = Send(sService, reponse, strlen(reponse))) < 0)
        {
            perror("Erreur de Send CBP");
            retirerClient(sService);
            close(sService);
            return;
        }
    }
}

// Traitement d'une connexion ACBP (serveur de requêtes)
void traitementConnexionACBP(int sService)
{
    char requete[256];
    char reponse[1024]; // Plus grand pour la liste des clients
    int ret;

    memset(reponse, 0, sizeof(reponse));

    if((ret = Receive(sService, requete)) < 0)
    {
        perror("Erreur de Receive ACBP");
        return;
    }
    
    if(ret == 0) // client déconnecté immédiatement
        return;

    printf("Requête ACBP reçue : %s\n", requete);
    
    if(ACBP(requete, reponse, sService) == false)
    {
        strcpy(reponse, "KO");
    }

    printf("Réponse ACBP envoyée : %s\n", reponse);
    
    if((ret = Send(sService, reponse, strlen(reponse))) < 0)
    {
        perror("Erreur de Send ACBP");
        return;
    }
}

// Lecture du fichier de configuration
void readText(int* port, int* pool, int* portAdmin)
{
    FILE* f = fopen("configserveur.txt", "r");
    if (f == NULL)
    {
        perror("Erreur d'ouverture du fichier configserveur.txt");
        exit(1);
    }
    
    if (fscanf(f, "PORT=%d\n", port) != 1)
    {
        fprintf(stderr, "Erreur de lecture du PORT\n");
        fclose(f);
        exit(1);
    }
    
    if (fscanf(f, "POOL=%d\n", pool) != 1)
    {
        fprintf(stderr, "Erreur de lecture du POOL\n");
        fclose(f);
        exit(1);
    }

    if(fscanf(f, "PORT_ADMIN=%d", portAdmin) != 1)
    {
        fprintf(stderr, "Erreur de lecture du PORT_ADMIN\n");
        fclose(f);
        exit(1);
    }
    
    fclose(f);
    printf("Configuration chargée: PORT=%d, POOL=%d, PORT_ADMIN=%d\n", *port, *pool, *portAdmin);
}

// Handler pour SIGINT (Ctrl+C)
void handlerSIGINT(int sig)
{
    printf("\nFermeture du serveur...\n");

    // Fermer toutes les connexions clients
    extern CONNECTED_CLIENT* clients_connectes[NB_MAX_CLIENTS];
    extern int nb_clients;
    extern pthread_mutex_t mutex_clients_connectes;
    
    pthread_mutex_lock(&mutex_clients_connectes);
    for(int i = 0; i < nb_clients; i++) {
        close(clients_connectes[i]->socket);
        free(clients_connectes[i]);
    }
    pthread_mutex_unlock(&mutex_clients_connectes);
    
    // Fermer les sockets d'écoute
    close(sEcoute_acbp);
    close(sEcoute_cbp);
    
    // Fermer la connexion MySQL
    mysql_close(connexion);
    
    exit(0);
}