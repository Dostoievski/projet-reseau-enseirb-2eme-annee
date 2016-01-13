(voir les 3 choses à faire recommandées!!)
# On a : #
  * socket entre 1 serveur et 1 client (éventuellement plusieurs clients (fork(processus) ou thread http://www.life.fr/~marquet/ens/sem/tubes/tubes002.html)
  * partie de capture serveur
  * partie de filtrage client (Le filtrage n'a pas besoin d'analyser les données du paquet, uniquement les entêtes)
  * partie d'analyse client (L'analyse a besoin de récupérer et lire dans les données du paquet (exemple : mot de passe)
> ou il a besoin de plusieurs paquets pour faire une analyse statistique)

# A faire : #
  * multi client (fichier ds un premier temps)
  * multi-client : temps réel

  * ->1ère possibilité : le serveur envoie TOUS les paquets et le client les reçoit et fait le filtrage et l'analyse.

  * ->2ème possibilité :
    * RECOMMANDE :
      * le serveur fait le filtrage personnalisé par client, le client fait l'analyse des paquets qu'il va recevoir .

= Configuration :
  * RECOMMANDE : lister les IPs du réseau du serveur (par les IPs extérieurs) (192.168.X.Y)(besoin du masque réseau).
  * lister les réseux privés (10.X.X.X , 172.16.X.X, 192.168.n.X..)

= Réparation : (filtrage)
  * messages d'erreurs d'ICMP (traceroute, TTL croissants)

= Optimisation : (analyse)
  * Taux de protocoles : affichage d'une liste avec des pourcentages
  * -----------P2P : 98%
  * -----------HTTP : 67%
  * -----------POP3 : 34%
  * -----------SSH : 62 %
  * -----------FTP : ..
  * -----------SSH :
  * -----------IMAP :

=Protection / Sécurité :
  * Affichage d'une alerte qd un user envoie son mot de passe en clair lors de l'utilisation par exemple :

  * RECOMMANDE : en FTP (en mode nn sécurisé, le plus simple)
  * -----------en HTTP
  * -----------en POP3 (avec Thunderbird)
  * -----------en IMAP







