#ifndef TCP_H
#define TCP_H

#define SIZE_NB_BYTES 4
#define SIZE_IP_ADDRESS 16

// ServerSocket() est une fonction qui sera appelée par le processus serveur (celui
// qui se mettra donc en attente d’une connexion). Elle prend en entrée le port sur
// lequel le processus souhaite attendre et retourne la socket d’écoute ainsi créé.
// Pour ce faire, cette fonction
// - fait un appel à socket() pour créer la socket
// - construit l’adresse réseau de la socket par appel à getaddrinfo()
// - fait appel à bind() pour lier la socket à l’adresse réseau
// - fait appel à listen() pour démarrer la machine à états TCP
int ServerSocket(int port);

// Accept() est une fonction qui sera appelée par le processus serveur. Elle prend en
// premier paramètre la socket créée par l’appel de ServerSocket() et retourne la
// socket de service obtenue par connexion avec un client. Pour ce faire, cette fonction
// - fait appel à accept()
// - récupère éventuellement l’adresse IP distante du client qui vient de se
// connecter. Cette adresse IP est placée dans ipClient si celui-ci est non NULL.
// S’il est non NULL, ipClient doit pointer vers une zone mémoire capable de
// recevoir une chaîne de caractères de la taille d’une adresse IP (Exemple : « 192.168.228.167 »)
int Accept(int sEcoute,char *ipClient);

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
int ClientSocket(char* ipServeur,int portServeur);

// Send() est une fonction qui sera utilisée par le processus client et le processus
// serveur afin d’envoyer des données. Cette fonction reçoit en paramètre
// - la socket de service
// - l’adresse mémoire d’un « paquet de bytes » que l’on désire envoyer
// - la taille de ce paquet de bytes
// et retourne le nombre de bytes qui ont été envoyés à des fins de « test d’erreur ».
int Send(int sSocket,char* data,int taille);

// Receive() est une fonction qui sera utilisée par le processus client et le processus
// serveur afin de recevoir un « paquet de données » envoyé par la fonction Send().
// Elle reçoit en paramètre
// - la socket de service
// - l’adresse d’un buffer de réception qui va revoir les données lues sur le réseau
// et retourne le nombre de bytes qui ont été lus
int Receive(int sSocket,char* data);

#endif