#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib_socket.h"
#include <signal.h>

///////////////////////////////////////////////////////////////
//  Serveur         ///////////////////////////////////////////
///////////////////////////////////////////////////////////////

int ServerSocket(int port)
{
    char ptr[SIZE_IP_ADDRESS];
    struct addrinfo hints;
    struct addrinfo *results;

    snprintf(ptr, sizeof(ptr), "%d", port);

    int socServer = socket(AF_INET, SOCK_STREAM, 0);

     // On fournit l'hote et le service
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_PASSIVE;
    
    if (getaddrinfo(NULL, ptr,&hints,&results) != 0)
    {
        perror("Erreur de getaddrinfo()");
        close(socServer);
        exit(1);
    }

    if (bind(socServer,results->ai_addr,results->ai_addrlen) < 0)
    {
        perror("Erreur de bind()");
        exit(1);
    }

    freeaddrinfo(results);

    if(listen(socServer, 2) < 0)
    {
        perror("Erreur de listen()");
        exit(1);
    }

    return socServer;
}

int Accept(int sEcoute,char *ipClient)
{
    int socService;
    struct sockaddr_in adrClient;
    socklen_t adrClientLen = sizeof(struct sockaddr_in);
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];

    if((socService = accept(sEcoute, (struct sockaddr*)&adrClient, &adrClientLen)) == -1)
    {
        perror("Erreur de accept()");
        exit(1);
    }

    // récup de l'adresse IP : 
    if(getnameinfo((struct sockaddr*)&adrClient, adrClientLen, host, NI_MAXHOST, port, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0)
    {
        perror("Erreur de getnameinfo()");
        close(socService);
        exit(1);
    }
    strcpy(ipClient, host);
    return socService;
}

///////////////////////////////////////////////////////////////
//  Client         ////////////////////////////////////////////
///////////////////////////////////////////////////////////////

int ClientSocket(char* ipServeur,int portServeur)
{
    int socClient;
    
    struct addrinfo hints, *results;
    char portstr[SIZE_IP_ADDRESS];

    if((socClient = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Erreur de socket()");
        exit(1);
    }

    snprintf(portstr, sizeof(portstr), "%d", portServeur);

    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_NUMERICHOST;

    if (getaddrinfo(ipServeur, portstr,&hints,&results) != 0)
    {
        perror("Erreur de getaddrinfo()");
        close(socClient);
        exit(1);
    }
    
    if(connect(socClient, results->ai_addr, results->ai_addrlen) == -1)
    {
        perror("Erreur de connect()");
        exit(1);
    }

    return socClient;
}

int Send(int sSocket, char* data, int taille)
{
    if (sSocket < 0 || data == NULL || taille < 0) {
        fprintf(stderr, "Send: Paramètres invalides\n");
        return -1;
    }

    int tailleReseau = htonl(taille);

    char buffer[SIZE_NB_BYTES];
    memcpy(buffer, &tailleReseau, sizeof(int));
    
    int nbBytes = send(sSocket, buffer, sizeof(char) * SIZE_NB_BYTES, 0);
    
    if (nbBytes <= 0)
    {
        // Erreur d'envoi
        return nbBytes;
    }
    
    int totalEnvoye = 0;
    while (totalEnvoye < taille)
    {
        nbBytes = send(sSocket, data + totalEnvoye, taille - totalEnvoye, 0);
        
        if (nbBytes <= 0)
        {
            return nbBytes;
        }
        
        totalEnvoye += nbBytes;
    }
    
    return totalEnvoye;
}


int Receive(int sSocket, char* data)
{
    if (sSocket < 0 || data == NULL) 
    {
        fprintf(stderr, "Receive: Paramètres invalides\n");
        return -1;
    }

    int taille;
    int nbBytes = recv(sSocket, &taille, sizeof(char) * SIZE_NB_BYTES, 0);
    
    if (nbBytes <= 0)
    {
        return nbBytes;
    }
    
    taille = ntohl(taille);

    int totalRecu = 0;
    while (totalRecu < taille)
    {
        nbBytes = recv(sSocket, data + totalRecu, taille - totalRecu, 0);
        
        if (nbBytes <= 0)
        {
            return nbBytes;
        }
        
        totalRecu += nbBytes;
    }
    
    data[totalRecu] = '\0';
    
    return totalRecu;
}
