/**
 * \file server.c
 * \brief Programme représentant les différentes fonctionalités du serveur.
 * \author RedNetwork
 * \version 0.1
 * \date 16 Mai 2011
 */
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/wait.h>

#include "requests.h"
#include "common.h"
#include "sniffer.h"

/* Mettre 1 avec un debugger. */
#if 0
#define fork() (0)
#endif

/**
 * \fn get_callername (int fd)
 * \brief returns the hostname (or its ip address) on the other side
 * of the  connected socket fd. The name is contained in a static area.
 * (note: this is rather tricky)
 * This is used only to print nice messages about what's going on.
 *
 * \param fd : cnnected socket.
 */
char *get_callername (int fd)
{
    struct sockaddr_in sins;
    int len;
    struct hostent *sp;

    len = sizeof (sins);
    if (getpeername (fd, (struct sockaddr *)&sins, &len) == -1) {
        perror ("getpeername");
        return "Noname";
    }
    sp = gethostbyaddr ((char *)&(sins.sin_addr.s_addr), 4, AF_INET);
    if (sp == NULL) {
        char *rep;
        rep = inet_ntoa (sins.sin_addr);
        fprintf (stderr, "can't reverse address for %s\n", rep);
        return rep;
    }
    return sp->h_name;
} /* get_callername */

/**
 * \fn get_file
 * \brief  Envoi d'un fichier (requete GET).
 * \param 'clientfd' est un socket connecte a un client.
 * \param 'filename' est le nom du fichier a envoyer.
 */
void get_file (int clientfd, char *filename)
{
  // ANSWER GET
  struct answer answer_get;
  // Buffer
  struct stat buffer;
 
  // Ouverture du fichier local en lecture seule
  int fd = open(filename, O_RDONLY);
 
   if (fd == -1) {  // Si erreur lors de l'ouverture du fichier

    printf("Erreur lors de l\'ouverture du fichier : GET");
    // Envoi d'une réponse : erreur
    answer_get.ack=ANSWER_ERROR;
    answer_get.errnum=errno;
    if (write(clientfd,&answer_get,sizeof(answer_get)) == -1){   
      printf("Erreur lors de l\'envoi d'une réponse: GET"); 
      exit(EXIT_FAILURE);      
    }
    exit(EXIT_FAILURE); 
  }
  else {    // Si fichier ouvert correctement

    // ack
    answer_get.ack=ANSWER_OK;
    fstat(fd,&buffer);
    answer_get.nbbytes=buffer.st_size;
    if (write(clientfd,&answer_get,sizeof(answer_get)) == -1){   
      printf("Erreur lors de l\'envoi d\'une réponse: GET"); 
      exit(EXIT_FAILURE);       
    }}
  // Copie des donnees à partir du fichier
    if(copy_all_bytes(fd,clientfd) == -1){   
      printf("Erreur lors de la copie des données: GET"); 
      exit(EXIT_FAILURE);       
    }
    // Fin de la copie et fermeture du fichier
    close(fd);
} /* get_file */

/**
 * \fn dir_file
 * \brief ls sur un fichier/repertoire (requete DIR).
 * \param 'clientfd' est un socket connecte a un client.
 * \param 'pathname' est le nom du fichier ou repertoire a lister.
 */
void dir_file (int clientfd, char *pathname)
{

  struct answer answer_dir;   
   
  if(pathname != "") { //si le pathname est non vide

    // Envoi d'une réponse
    answer_dir.ack=ANSWER_OK;
    if (write(clientfd,&answer_dir,sizeof(answer_dir)) == -1){   
      printf("Erreur lors de l\'envoi d'une réponse: DIR"); 
      exit(EXIT_FAILURE);      
    }
  }   
  else {
    // Envoi d'une réponse erreur
    answer_dir.ack=ANSWER_ERROR;
    answer_dir.errnum=errno;    
    if (write(clientfd,&answer_dir,sizeof(answer_dir)) == -1){   
      printf("Erreur lors de l\'envoi d\'une reponse: DIR"); 
      exit(EXIT_FAILURE);       
    }
  }

  // Redirection de la sortie standard
  dup2(clientfd,1);
  // Redirection de la sortie d'erreur
  dup2(clientfd,2);

  // Execution de ls -l
  printf("ls -l du repertoire : %s\n",pathname);
  execlp("ls","ls","-l",pathname,NULL);

} /* dir_file */

/**
 * \fn sniff
 * \brief inscrit l'ID du client dans le tab des protocoles correspondant.
 * \param 'clientfd' est un socket connecte a un client.
 * \param 'pathname' est le nom du fichier ou repertoire a sniffer.
 */
void sniff(int clientfd, char * pathname){

  struct answer answer_sniff;
  fprintf(stderr,"server.c sniff 1\n");   
  //si le protocole existe, on envoie l'ACK et on ajoute l'ID du client dans le tab correspondant
  if ((strcmp (pathname, "ip") == 0)){
    if(add_client(IP,MAXCLIENTS,clientfd) <0){
      fprintf(stderr,"erreur add client a ip : requete sniff");
      exit(1);
    }
  }

  else if ((strcmp (pathname, "tcp") == 0)){
    //add client 
    if(add_client(TCP,MAXCLIENTS,clientfd) <0){
      fprintf(stderr,"erreur add client a tcp : requete sniff");
      exit(1);
    }
  }
  else if ((strcmp (pathname, "udp") == 0)){
    //add client
    if(add_client(UDP,MAXCLIENTS,clientfd) <0){
      fprintf(stderr,"erreur add client a udp : requete sniff");
      exit(1);
    }
  }
  else if ((strcmp (pathname, "icmp") == 0))
    {
      //add client
      if(add_client(ICMP,MAXCLIENTS,clientfd) <0){
	fprintf(stderr,"erreur add client a icmp : requete sniff");
	exit(1);
      }
    }
  else
    ;
  
}/*end sniff*/

/**
 * \fn handle_request
 * \brief 
 *
 */
void* handle_request (void * fd)
{
  int f = *((int *)fd);
  struct request r;  // Structure request
  struct answer a;  // Structure answer
  
  printf ("Process %d, handling connection from %s\n",
	  getpid(), get_callername (f));
  
  if (recv(f, (void *)(&r), sizeof(r), 0) == -1){
    printf("Erreur lors de la lecture de la requete sur le descripteur f");
    exit(EXIT_FAILURE);
  } 
  
  switch(r.kind)
    {
    case REQUEST_GET:
      get_file(f, r.path);
      break;
    case REQUEST_DIR:
      dir_file(f, "./");//pour l'instant, on affiche le dossier courant
      break;
    case REQUEST_SNIFF:
      check_protocole(r.path);
      sniff(f, r.path);//path est le protocole que le client souhaite filtrer
      break;
    default:
      a.ack=ANSWER_UNKNOWN;     // Requete inconnue
      perror("Requete inconnue.\n");
      exit(EXIT_FAILURE);
      break;
    }
}

/**
 * \fn traitement_sig
 * \brief Traitement de la terminaison d'un fils. 
 *
 */
void traitement_sig(int sig)
{
  int pid;
  int status;

  if (sig == SIGCHLD)
  {
    pid = wait(&status);
    printf("Le processus fils %d termine son execution avec un statut %d\n", pid, status);
  }
}

/**
 * \fn main
 * \brief main principal de serveur. 
 *
 */
int main ()
{
    struct sockaddr_in soc_in;
    struct sockaddr_in soc_client;
    int val;
    int size;
    int ss;                     /* socket d'ecoute */
    int client;			/* socket client   */

    /* on commence par initialiser toutes les tables*/
    table_init(IP,MAXCLIENTS);
    table_init(TCP,MAXCLIENTS);
    table_init(UDP,MAXCLIENTS);
    table_init(ICMP,MAXCLIENTS);



    /***********************************************/
    /* nbr of packets to capture*/
    int num_packet = -1; 

    /* on lance le thread qui sniffera pendant tout le temps*/
    pthread_t worker;
    pthread_create(&worker,NULL,start_sniffing,(void *) &num_packet);
    /****************************************/

    /* Création d'un socket 'ss' : famille IP et type TCP */
	ss=socket(AF_INET,SOCK_STREAM,0);
	if(ss==-1){
	   printf("La création du socket coté serveur a echoué\n");
	   exit(EXIT_FAILURE);
	}

    /* Force la réutilisation de l'adresse si non allouée */
    val = 1;
    if (setsockopt(ss,SOL_SOCKET,SO_REUSEADDR,(char*)&val,sizeof(val)) == -1) {
        perror ("setsockopt");
        exit (EXIT_FAILURE);
    }

    /*
     * Nomme localement le socket :
     * socket inet, port local PORT, adresse IP locale quelconque
     */
    soc_in.sin_family=AF_INET;
    soc_in.sin_port=PORT;
    soc_in.sin_addr.s_addr=INADDR_ANY;

    if(bind(ss,(struct sockaddr *) &soc_in, sizeof(soc_in)) == -1){
      printf("ERROR bind: code d'erreur %d\n",errno);
      exit(EXIT_FAILURE);
    }
    /* Prepare le socket a la reception de connexions */
    // Listen
    if(listen(ss,10)==-1){
      printf("ERROR listen: code d'erreur %d\n",errno);
      exit(EXIT_FAILURE);
    }
    printf("le socket est en écoute\n ");

    // Recuperation du signal
    
    signal(SIGCHLD,traitement_sig);

    while (1) {

        // structure destinée à recueillir les renseignements sur l'appelant
      size=sizeof(soc_client);

      // Etablissement d'une connexion
      client=accept(ss,(struct sockaddr *) &soc_client,&size);
      fprintf(stderr,"connexion...\n");

      if(client == -1){
	// Si pas de connexion
	continue;
      }
      else {
	fprintf(stderr,"connexion acceptée\n"); 
      }
      
      printf("traitement de la requete\n"); 
      pthread_t worker;
      pthread_create(&worker,NULL,handle_request,(void*)&client);
    }
    return 0;
}
