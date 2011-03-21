/*

 */

/*
 * miniftp hostname get distfile  localfile
 * miniftp hostname list
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <sys/wait.h>

#include "requests.h"
#include "common.h"

/*
 * check_answer ne rend la main que si  a->ack est ANSWER_OK,
 * sinon le processus se termine avec un message d'erreur.
 */

void check_answer (struct answer *a)
{
    switch (a->ack) {
      case ANSWER_OK:
        break;
      case ANSWER_UNKNOWN:
        fprintf (stderr, "?? unknown request ?\n");
        exit (1);
      case ANSWER_ERROR:
        if (a->errnum != 0) {
            errno = a->errnum;
            perror ("Server rejection");
        } else {
            fprintf (stderr, "Server rejection\n");
        }
        exit (1);
      default:
        fprintf (stderr, "Bad answer from server %d\n", a->ack);
        exit (1);
    }
}

/*
 * Obtention d'un fichier distant.
 * 'serverfd' est un socket connecte a la machine 'servername'.
 * 'distname' est le nom du fichier a demander, 'localname' est le nom du
 * fichier resultat.
 */
void get_file (int serverfd, char *servername, char *distname, char *localname)
{

   
  /* identification des reponses */
  int retour;
  int id;
  /* Creation d'un buffer de requete et de réponse */
  struct request requete;   
  struct answer reponse;
                     
  /* Remplissage des champs de la  requete */
  requete.kind = REQUEST_GET;
  strcpy(requete.path, distname);  
  
  /* Envoi de la requete vers le serveur, identification d erreur */
  if ((retour = write(serverfd, &requete, sizeof(struct request))) == -1)
  {
   fprintf(stdout,"[%i] : Erreur d'ecriture de la requete vers le socket serveur\n",getpid());
      exit(EXIT_FAILURE);
  }

  /* lecture de la réponse du serveur et identification d erreur*/
  if ((retour = read(serverfd, &reponse, sizeof(struct answer))) == -1 )
  	{
	/* arreter le programme */
		  fprintf(stdout, "[%i] : Erreur de lecture de l'acquittement\n", getpid()); 
		  exit(EXIT_FAILURE);
 	}

  /* Verification de l'acquittement */
  check_answer(&reponse);
	  
  /* Creation du fichier  */
  id = open(localname, O_WRONLY | O_CREAT | O_EXCL, 0644);
  if (id == -1)
	    {
	      fprintf(stdout, "[%i] : Impossible de creer le fichier localement\n", getpid());
	      exit(EXIT_FAILURE);
	    }
  /* Ecriture du fichier en copiant le buffer*/	  
  copy_n_bytes(serverfd, id, reponse.nbbytes);
	  
  close(id);
  fprintf(stdout, "[%i] ack recu et ecriture du fichier local effectue\n", getpid());
  exit(EXIT_SUCCESS); 

}


/*
 * Obtention d'une liste des fichiers log que le serveur a créé.
 * 'serverfd' est un socket connecte a la machine 'servername'.
 * 'distname' est le nom du fichier ou repertoire distant a lister.
 */
void list (int serverfd, char *servername)
{
  int retour;

  //création d'un buffer  requete et réponse
  struct request requete;
  struct answer reponse;
  //remplissage des champs de la requete
  requete.kind = REQUEST_LIST;
  
  //envoie de la requete
    
  /* Envoi de la requete vers le serveur, identification d erreur */
  if ((retour = write(serverfd, &requete, sizeof(struct request))) == -1)
  {
   fprintf(stdout,"[%i] : Erreur d'ecriture de la requete vers le socket serveur\n",getpid());
      exit(EXIT_FAILURE);
  }

  /* lecture de la réponse du serveur et identification d erreur*/
  if ((retour = read(serverfd, &reponse, sizeof(struct answer))) == -1 )
  	{
	/* arreter le programme */
		  fprintf(stdout, "[%i] : Erreur de lecture de l'acquittement\n", getpid()); 
		  exit(EXIT_FAILURE);
 	}

  /* Verification de l'acquittement */
  check_answer(&reponse);

  //on affiche la liste envoyée par le serveur
  dup2(serverfd,STDOUT_FILENO);
  
}


/*
 * Retourne un socket connecte la machine 'serverhost' sur le port 'port',
 * ou termine le processus en cas d'echec.
 */
int connection (char *serverhost, unsigned int port)
{
	/*typedef struct hostent {
  	char FAR      *h_name;
  	char FAR  FAR **h_aliases;
  	short         h_addrtype;
  	short         h_length;
  	char FAR  FAR **h_addr_list;
	} HOSTENT, *PHOSTENT, FAR *LPHOSTENT;*/

	/*struct sockaddr_in {
	    short   sin_family;		// must be AF_INET
	    u_short sin_port;
	    struct  in_addr sin_addr;
	    char    sin_zero[8];
	}; */


	struct hostent *ip_serveur; //structure pour stocker les informations du host

  	//On recupere l'adresse IP serveur avec gethostbyname
  	ip_serveur = gethostbyname(serverhost);
	
	struct sockaddr_in socket_addr;       //structure de socket
	
	if(ip_serveur!=NULL) {
		//Affectation des valeurs de la structure socket_addr
   		socket_addr.sin_family=AF_INET;
    		socket_addr.sin_port=port;
    		socket_addr.sin_addr.s_addr=*(unsigned long *)ip_serveur->h_addr_list[0];
	
    		/*Creation de la socket*/
		int socket_TCP; 	// descripteur de socket
    		socket_TCP=socket(PF_INET,SOCK_STREAM,0); 
		if(socket_TCP == -1){
		perror("Erreur lors de la création du socket TCP");
		exit(EXIT_FAILURE);    	
		}

		/*Demande de connexion au serveur*/
		int c_descriptor;
    		c_descriptor=connect(socket_TCP,(struct sockaddr *) &socket_addr, sizeof(socket_addr));
		if(c_descriptor == -1){
		perror("Erreur lors de la connection");
		exit(EXIT_FAILURE);
    		}
		else {
     		printf("connexion réussie\n");
      		return socket_TCP;
		}
	}

    /*
    return le_socket_connecte_au_serveur;
    */
}


void usage (void)
{
    fprintf (stderr, "miniftp: hostname get distfilename localfilename\n");
    fprintf (stderr, "miniftp: hostname list\n");
    exit (2);
}

int main (int argc, char **argv)
{
    char *serverhost;
    int cmde;
    int serverfd;

    if (argc < 3)
      usage ();

    serverhost = argv[1];
    
     if ((strcmp (argv[2], "get") == 0) && (argc == 5))
      cmde = REQUEST_GET;
     else if ((strcmp (argv[2], "list") == 0) && (argc == 3))
      cmde = REQUEST_LIST;
    else
      usage();

    serverfd = connection (serverhost, PORT);

    switch (cmde) {
      case REQUEST_GET:
        get_file (serverfd, serverhost, argv[3], argv[4]);
        break;
    case REQUEST_LIST:
        list(serverfd, serverhost);
        break;
    }

    close (serverfd);
    return 0;
}

