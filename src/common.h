/*
 * Copie `nbbytes' octets depuis le descripteur `from' sur le descripteur `to'.
 * En cas d'echec, affiche un message explicatif.
 * Renvoie 0 si ok, -1 en cas d'erreur.
 */
int copy_n_bytes (int from, int to, int nbbytes);

/*
 * Copie depuis le descripteur `from' sur le descripteur `to' 
 * jusqu'a ce qu'une lecture sur `from' renvoie end-of-file.
 * En cas d'echec, affiche un message explicatif.
 * Renvoie 0 si ok, -1 en cas d'erreur.
 */
int copy_all_bytes (int from, int to);

/*
 * Comme perror, mais avec deux chaines de caracteres.
 * perror2 ("Echec ouverture du fichier", filename);
 */
void perror2 (char *s1, char *s2);


/*a zero init fucntion */
void
table_init ( int * tab, int size);

/* adds the client fd to the corresponding table*/
int add_client(int *, int, int);



  






