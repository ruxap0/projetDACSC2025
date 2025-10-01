#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "lib_socket.h"
typedef struct
{
 char nom[20];
 int age;
 float poids;
} PERSONNE;
int main(int argc,char* argv[])
{
 if (argc != 3)
 {
 printf("Erreur...\n");
 printf("USAGE : ClientTest ipServeur portServeur\n");
 exit(1);
 }
 int sClient;
 if ((sClient = ClientSocket(argv[1],atoi(argv[2]))) == -1)
 {
 perror("Erreur de ClientSocket");
 exit(1);
 }
 // ***** Envoi de texte pur ***************************************
 char texte[80];
 sprintf(texte,"Bonjour, comment vas-tu ?");
 int nbEcrits;
 if ((nbEcrits = Send(sClient,texte,strlen(texte))) == -1)
 {
 perror("Erreur de Send");
 close(sClient);
 exit(1);
 }
 printf("NbEcrits = %d\n",nbEcrits);
 printf("Ecrit = --%s--\n",texte);
 // ***** Reception texte pur **************************************
 char buffer[100];
 int nbLus;
 if ((nbLus = Receive(sClient,buffer)) < 0)
 {
 perror("Erreur de Receive");
 close(sClient);
 exit(1);
 }
 printf("NbLus = %d\n",nbLus);
 buffer[nbLus] = 0;
 printf("Lu = --%s--\n",buffer);
 // ***** Envoi d'une structure ************************************
 PERSONNE p;
 strcpy(p.nom,"Wagner");
 p.age = 49;
 p.poids = 87.21f;
 if ((nbEcrits = Send(sClient,(char*)&p,sizeof(PERSONNE))) < 0)
 {
 perror("Erreur de Send");
 close(sClient);
 exit(1);
 }
 printf("NbEcrits = %d\n",nbEcrits);
 printf("Ecrit = --%s--%d--%f--\n",p.nom,p.age,p.poids);
 // ***** Reception d'une structure ********************************
 if ((nbLus = Receive(sClient,(char*)&p)) < 0)
 {
 perror("Erreur de Receive");
 close(sClient);
 exit(1);
 }
 printf("NbLus = %d\n",nbLus);
 printf("Lu = --%s--%d--%f--\n",p.nom,p.age,p.poids);
 close(sClient);
 exit(0);
}
