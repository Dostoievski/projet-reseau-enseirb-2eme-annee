/**
 * \file common.c
 * \brief Programme représentant des fonctions untilisées à la fois par le serveur et le client.
 * \author RedNetwork
 * \version 0.1
 * \date 16 Mai 2011
 */
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>
#include <errno.h>
#include "common.h"
#include <ctype.h>
#include <sys/types.h>
#define MSGSIZE 512

/**
 * \fn int copy_n_bytes (int from, int to, int nbbytes)
 * \brief Copie `nbbytes' octets depuis le descripteur `from' sur le descripteur `to'.
 *  La lecture est faite par bloc de MSGSIZE octets.
 *  L'ecriture s'attend
 *  a ne pas pouvoir tout ecrire d'un seul coup.
 * \param from : on lit n octets de from.
 * \param to : on ecrit n octets dans to.
 * \param nbbytes : le nombre de octets à copier.
 * \return En cas d'echec, affiche un message explicatif.
 * Renvoie 0 si ok, -1 en cas d'erreur.
 */
int copy_n_bytes (int from, int to, int nbbytes)
{
    char buf[MSGSIZE];
    char *p;
    while (nbbytes != 0) {
        int ecrit;
        int lu;
        lu = read (from, buf, ((nbbytes < MSGSIZE) ? nbbytes : MSGSIZE));
        if (lu == -1) {
            perror ("bad read");
            return -1;
        } else if (lu == 0) {
            fprintf (stderr, "EOF (connection closed)\n");
            return -1;
        }
        nbbytes -= lu;
        p = buf;
        while (lu != 0) {
            ecrit = write (to, p, lu);
            if (ecrit == -1) {
                perror ("bad write");
                return -1;
            }
            p += ecrit;
            lu -= ecrit;
        }
    }
    return 0;
}

/**
 * \fn int copy_all_bytes (int from, int to)
 * \brief Copie tous les octets depuis le descripteur `from' sur le descripteur `to'.
 *  La lecture est faite par bloc de MSGSIZE octet.
 *  L'ecriture s'attend a ne pas pouvoir tout ecrire d'un seul coup.
 * \param from : on lit n octets de from.
 * \param to : on ecrit n octets dans to.
 * \return En cas d'echec, affiche un message explicatif.
 * Renvoie 0 si ok, -1 en cas d'erreur.
 */
int copy_all_bytes (int from, int to)
{
    char buf[MSGSIZE];
    char *p;
    while (1) {
        int ecrit;
        int lu;
        lu = read (from, buf, MSGSIZE);
        if (lu == -1) {
            perror ("bad read");
            return -1;
        } else if (lu == 0) {
            return 0;
        }
        p = buf;
        while (lu != 0) {
            ecrit = write (to, p, lu);
            if (ecrit == -1) {
                perror ("bad write");
                return -1;
            }
            p += ecrit;
            lu -= ecrit;
        }
    }
    /* NOTREACHED */
    return 0;
}

/**
 * \fn void perror2 (char *s1, char *s2)
 * \brief Fonctionalité pareille à perror mais avec deux chaine de caracteres.
 *
 * \param s1 : Echec ouverture du fichier.
 * \param s2 : filename
 */
void perror2 (char *s1, char *s2)
{
    if (s2 == NULL) {
        perror (s1);
    } else if (s1 == NULL) {
        perror (s2);
    } else {
        fprintf (stderr, "%s ", s1);
        perror (s2);
    }
}

/*zero init fucntion */
void
table_init ( int * tab, int size){
  int k =0;
  for(k=0; k <size; k++) tab[k]=0;
}

/* add the client fd on the corresponding table*/
int add_client(int *tab, int tab_size, int clientfd){
  int i;
  for (i=0; i< tab_size; i++){
    if (tab[i]==0){ //si case vide
      tab[i]= clientfd;
    return 1;
    }
  }
  return -1;
}



