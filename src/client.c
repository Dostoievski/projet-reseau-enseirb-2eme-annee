/*
 * ./client hostname get distfile  localfile
 * ./client hostname list
 */
/**
 * \file client.c
 * \brief Programme représentant les différentes fonctionalités du client.
 * \author RedNetwork
 * \version 0.1
 * \date 16 Mai 2011
 */
#include "process_client.h"
#include "requests.h"
#include "common.h"
#include <ctype.h>

/**
 * \fn check_answer (struct answer *a)
 * \brief Fonction qui vérifie la réponse réçue par le client.
 *
 * \param a : la réponse reçue.
 */
void check_answer (struct answer *a){
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
 * 'serverfd' est un .
 * 
 */
/**
 * \fn get_file (int serverfd, char *servername, char *distname, char *localname)
 * \brief Fonction pour l'obtention d'un fichier distant.
 *
 * \param serverfd est le nom de socket connecte a la machine 'servername'.
 * \param 'distname' est le nom du fichier a demander, 'localname' est le nom du fichier resultat.
 */
void get_file (int serverfd, char *servername, char *distname, char *localname)
{

  // REQUEST GET
  struct request request_get;
  request_get.kind=REQUEST_GET;
  strcpy(request_get.path,distname);
  // ANSWER GET
  struct answer answer_get;
  // Envoie de la requete au serveur
  if(write(serverfd,&request_get,sizeof(request_get))==-1) {
    perror("erreur envoi requette de get\n");
    exit(EXIT_FAILURE);
  }
  //Lecture de la réponse du serveur a la requete
  if(read(serverfd,&answer_get,sizeof(answer_get))==-1){
    perror("erreur lors de la lecture de la reponse\n");
    exit(EXIT_FAILURE);
  }
  // CHECK ack GET
  check_answer(&answer_get);
  // Création d'n fichier
  int fd= open(localname, O_WRONLY | O_CREAT| O_EXCL, 0644);
  if (fd == -1) {
    perror("Erreur de creation du fichier\n");
    exit(EXIT_FAILURE); 
  }
  else {
    // Récuperation des données
    printf("Debut de la recuperation du fichier\n");
    int taille=answer_get.nbbytes;   
    copy_n_bytes(serverfd,fd,taille);
    printf("Recuperation terminee\n");
    close(fd); 
  }
}

/**
 * \fn dir_file (int serverfd, char *servername)
 * \brief Fonction pour Obtention d'un dir distant.
 *
 * \param serverfd est le nom de socket connecte a la machine 'servername'.
 * \param 'distname' est le nom du fichier ou repertoire distant a lister.
 */
void dir_file (int serverfd, char *servername)
{
   // REQUEST DIR
  struct request request_dir;
  request_dir.kind=REQUEST_DIR;
  // ANSWER DIR
  struct answer answer_dir;
  // Envoie de la requete au serveur
  if(write(serverfd,&request_dir,sizeof(request_dir))==-1) {
    perror("Erreur envoi requete : DIR");
    exit(EXIT_FAILURE);
  }
  // Lecture de la réponse du serveur a la requete****/
  if(read(serverfd,&answer_dir,sizeof(answer_dir)) == -1 ){
    perror("Erreur lors de la lecture de la réponse : DIR");
    exit(EXIT_FAILURE);
  }
  // CHECK ack DIR
  check_answer(&answer_dir);
  // Affichage du resultat de ls -l
  copy_all_bytes(serverfd,1);
}
/*
 *
 */
/**
 * \fn sniff(int serverfd, char * filter_exp)
 * \brief demander au serveur de lui envoyer des packets selon le
 * filtre filter_exp, ensuite passe la main au filtre qui fera
 * l'analyse .
 *
 * \param serverfd est le nom de socket connecte a la machine 'servername'.
 * \param filter_exp est l'expression de filtre
 */
void sniff(int serverfd, char * filter_exp){
  
  struct request request_sniff;
  request_sniff.kind=REQUEST_SNIFF;
  strcpy(request_sniff.path,filter_exp);//the protocole to filter
  struct answer answer_sniff;
  // Envoi de la requete au serveur
  if(write(serverfd,&request_sniff,sizeof(request_sniff))==-1) {
    perror("Erreur envoi requete : SNIFF");
    exit(EXIT_FAILURE);
  }
  // Lecture de la réponse du serveur a la requete****/
  if(read(serverfd,&answer_sniff,sizeof(answer_sniff)) == -1 ){
    perror("Erreur lors de la lecture de la réponse : sniff");
    exit(EXIT_FAILURE);
  }
  // CHECK ack sniff
    check_answer(&answer_sniff);
    int taille=answer_sniff.nbbytes; 
  /* buffer tmp de stokage des packets recus*/
     u_char buffer[taille];
  // Lecture de la réponse du serveur a la requete****/
     while(read(serverfd,buffer,taille) )
       {
       /* on analyse les packets recus : Optimisation*/
       got_packet(NULL, NULL, buffer);
       
       }
     printf("Recuperation terminee\n");
     /* on print les stats*/
     count_protocoles_stats();
}

/**
 * \fn connection (char *serverhost, unsigned int port)
 * \brief connecte le client à la machine serverhost.
 *
 * \param serverhost nom de la machine de serveur.
 * \param port port de connection.
 * \return un socket connecte la machine 'serverhost' sur le port 'port',
 *  ou termine le processus en cas d'echec.
 */
int connection (char *serverhost, unsigned int port)
{
  struct hostent *ip_serveur; //structure pour stocker les informations du host
  
  //On recupere l'adresse IP serveur avec gethostbyname
  ip_serveur = gethostbyname(serverhost);
  
  struct sockaddr_in socket_addr; //structure de socket
  
  if(ip_serveur!=NULL) {
    // Affectation des valeurs de la structure socket_addr
    socket_addr.sin_family=AF_INET;
    socket_addr.sin_port=port;
    socket_addr.sin_addr.s_addr=*(unsigned long *)ip_serveur->h_addr_list[0];
    
    // Création de la socket
    int socket_TCP; 	// descripteur de socket
    socket_TCP=socket(PF_INET,SOCK_STREAM,0); 
    if(socket_TCP == -1){
      perror("Erreur lors de la création du socket TCP");
      exit(EXIT_FAILURE);    	
    }
    
    // Demande de connexion au serveur
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
    fprintf (stderr, "client: hostname get distfilename localfilename\n");
    fprintf (stderr, "client: hostname list\n");
    fprintf (stderr, "client: hostname sniff protocole_name\n");
    exit (2);
}

/**
 * \fn main(int argc, char**argv)
 * \brief fonction main de client.
 */
int main (int argc, char **argv)
{
    char *serverhost;
    int cmde;
    int serverfd;
    
    if (argc < 3)
      usage ();

    /* on initialise la table contenant les stats des protocoles*/
    table_init(protocoles,PROTOCOLES_NUM);
  
    serverhost = argv[1];
    if ((strcmp (argv[2], "get") == 0) && (argc == 5))
      cmde = REQUEST_GET;
    else if ((strcmp (argv[2], "list") == 0) && (argc == 3))
      cmde = REQUEST_DIR;
    else if ((strcmp (argv[2], "sniff") == 0) && (argc == 4))
      cmde=REQUEST_SNIFF;
    else
      usage();
      
    serverfd = connection (serverhost, PORT);
    
    switch (cmde) {
    case REQUEST_GET:
      get_file (serverfd, serverhost, argv[3], argv[4]);
      break;
    case REQUEST_DIR:
      dir_file (serverfd, serverhost);
      break;
    case REQUEST_SNIFF :
      sniff(serverfd,argv[3]);
      /* we print the protocoles stats */
      printf(" \n******************************************************************\nstatisitiques : \n");
      break;
      }
    close (serverfd);
    return 0;
}

