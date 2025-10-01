#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

///////////////////////////////////////////////////////////////
//  Serveur         ///////////////////////////////////////////
///////////////////////////////////////////////////////////////

int ServerSocket(int port)
{
    // ServerSocket() est une fonction qui sera appelée par le processus serveur (celui
    // qui se mettra donc en attente d’une connexion). Elle prend en entrée le port sur
    // lequel le processus souhaite attendre et retourne la socket d’écoute ainsi créé.
    // Pour ce faire, cette fonction
    // - fait un appel à socket() pour créer la socket
    // - construit l’adresse réseau de la socket par appel à getaddrinfo()
    // - fait appel à bind() pour lier la socket à l’adresse réseau
    // - fait appel à listen() pour démarrer la machine à états TCP

    char ptr[16];
    struct addrinfo hints;
    struct addrinfo *results;

    snprintf(ptr, sizeof(ptr), "%d", port);

    int socServer = socket(AF_INET, SOCK_STREAM, 0);

     // On fournit l'hote et le service
    memset(&hints,0,sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_NUMERICSERV | AI_NUMERICHOST;
    
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
    // Accept() est une fonction qui sera appelée par le processus serveur. Elle prend en
    // premier paramètre la socket créée par l’appel de ServerSocket() et retourne la
    // socket de service obtenue par connexion avec un client. Pour ce faire, cette fonction
    // - fait appel à accept()
    // - récupère éventuellement l’adresse IP distante du client qui vient de se
    // connecter. Cette adresse IP est placée dans ipClient si celui-ci est non NULL.
    // S’il est non NULL, ipClient doit pointer vers une zone mémoire capable de
    // recevoir une chaîne de caractères de la taille d’une adresse IP (Exemple : « 192.168.228.167 »)

    int socService;
    struct sockaddr_in adrClient;
    socklen_t adrClientLen = sizeof(struct sockaddr_in);
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];

    if((socService = accept(sEcoute, NULL, NULL)) == -1)
    {
        perror("Erreur de accept()");
        exit(1);
    }

    // Recuperation d'information sur le client connecte     
    getpeername(socService,(struct sockaddr*)ipClient,&adrClientLen);
    getnameinfo((struct sockaddr*)ipClient ,adrClientLen, host,NI_MAXHOST, port,NI_MAXSERV, NI_NUMERICSERV | NI_NUMERICHOST);

    return socService;
}

///////////////////////////////////////////////////////////////
//  Client         ////////////////////////////////////////////
///////////////////////////////////////////////////////////////

int ClientSocket(char* ipServeur,int portServeur)
{
    // ClientSocket() est une fonction qui sera appelée par le processus client (celui qui
    // souhaite se connecter sur un serveur). Elle prend en entrée l’adresse IP (sous
    // forme d’une chaîne de caractère du type « 192.168.228.169 ») et le port (sous
    // forme d’un int) du serveur sur lequel on désire se connecter. Elle retourne la
    // socket de service qui va lui permettre de communiquer avec le serveur. Pour ce
    // faire, cette fonction
    // - fait appel à socket() pour créer la socket
    // - construit l’adresse réseau de la socket (avec l’IP et le port du serveur) par
    // appel à la fonction getaddrinfo()
    // - fait appel à connect() pour se connecter sur le serveur

    int socClient;
    
    struct addrinfo hints, *results;
    char portstr[16];

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
    // Convertir la taille du format hôte vers le format réseau
    int tailleReseau = htonl(taille);
    printf("CYKAAAAAAA\n\n\n");
    // Convertir l'int en 4 bytes
    char buffer[4];
    memcpy(buffer, &tailleReseau, sizeof(int));
    
    // Envoyer d'abord la taille (4 bytes)
    int nbBytes = send(sSocket, buffer, sizeof(char) * 4, 0);
    
    if (nbBytes <= 0)
    {
        // Erreur d'envoi
        return nbBytes;
    }
    
    // Envoyer les données
    int totalEnvoye = 0;
    while (totalEnvoye < taille)
    {
        nbBytes = send(sSocket, data + totalEnvoye, taille - totalEnvoye, 0);
        
        if (nbBytes <= 0)
        {
            // Erreur d'envoi
            return nbBytes;
        }
        
        totalEnvoye += nbBytes;
    }
    
    return totalEnvoye;
}


int Receive(int sSocket, char* data)
{
    int taille;
    int nbBytes = recv(sSocket, &taille, sizeof(char) * 4, 0);
    
    if (nbBytes <= 0)
    {
        // Erreur de réception ou connexion fermée
        return nbBytes;
    }
    
    // Convertir la taille du format réseau vers le format hôte
    taille = ntohl(taille);
    
    // Recevoir les données
    int totalRecu = 0;
    while (totalRecu < taille)
    {
        nbBytes = recv(sSocket, data + totalRecu, taille - totalRecu, 0);
        
        if (nbBytes <= 0)
        {
            // Erreur ou connexion fermée
            return nbBytes;
        }
        
        totalRecu += nbBytes;
    }
    
    // Ajouter le caractère de fin de chaîne
    data[totalRecu] = '\0';
    
    return totalRecu;
}
