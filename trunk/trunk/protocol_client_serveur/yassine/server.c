
#include <stdio.h>
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


/* Mettre 1 avec un debugger. */
#if 0
#define fork() (0)
#endif


/* get_callername(fd) returns the hostname (or its ip address) on the other side
 * of the  connected socket fd. The name is contained in a static area.
 * (note: this is rather tricky)
 * This is used only to print nice messages about what's going on.
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



/*
 * Envoi d'un fichier (requete GET).
 * 'clientfd' est un socket connecte a un client.
 * 'filename' est le nom du fichier a envoyer.
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





/*
 * ls sur un fichier/repertoire (requete DIR).
 * 'clientfd' est un socket connecte a un client.
 * 'pathname' est le nom du fichier ou repertoire a lister.
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



/*
 * Lit une requete sur le descripteur 'f', et appelle la procedure
 * correspondante pour gerer cette requete.
 */
void handle_request (int f)
{
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
    default:
      a.ack=ANSWER_UNKNOWN;     // Requete inconnue
      perror("Requete inconnue.\n");
      exit(EXIT_FAILURE);
      break;
  }
}

/* Traitement de la terminaison d'un fils.
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

int main ()
{
    struct sockaddr_in soc_in;
    struct sockaddr_in soc_client;
    int val;
    int size;
    int ss;                     /* socket d'ecoute */
    int client;			/* socket client   */


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
      
      int pid=fork();
     
      if(pid == -1){
	printf("Erreur lors de la creation du processus fils\n");
	exit(EXIT_FAILURE);
      }
      else{
	if(pid == 0){
	  printf("traitement de la requete\n");
	  handle_request(client); 
	
// Lancement du traitant des requetes
	  close(client);	       // Fermer le socket fils	  
	}
	else{
	  close(client);   // Fermer le socket fils
	}
      } 
    }  
    return 0;
}
