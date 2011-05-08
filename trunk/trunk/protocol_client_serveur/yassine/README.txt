			/**************  README **************/


Le mini-ftp est composé de deux interfaces : une interface client et une interface serveur. 

Vous avez à votre disposition quatre requêtes qui permettent la communication entre les deux interfaces :

get --> Récupérer un fichier présent sur le serveur.
put --> Déposer un fichier dans le serveur.
del --> Détruire un fichier sur le serveur.
dir --> Effectuer ''ls'' d'un répertoire présent sur le serveur.

Dans notre cas, on travaille en local (localhost). Chaque interface doit donc être lancée sur un terminal indépendant.





/******* INTERFACE SERVEUR *******/


L'interface serveur est lancée en premier. En effet, pour permettre l'exécution des requêtes un serveur doit être préalablement à l'écoute.

Le serveur est lancé avec la commande :
./miniftpd

Une fois le serveur lancé, le message suivant apparaîtra sur votre écran :
Le socket est en écoute


Ensuite, la connexion est établit dès que vous lancez une requête côté client.

Au fur est à mesure de l'exécution des messages s'afficheront pour vous signaler le déroulement des opération : établissement ou non de la connexion, traitement des requêtes...

A noter qu'à la fin de l'exécution de chaque requête, un message s'affiche sur l'écran côté serveur. Ce message contient le PID du processus utilisé pour effectuer le traitement.
Exemple :   Process 3784, handling connection from localhost.

Le serveur reste en écoute permanent, vous pouvez lancer plusieurs requêtes d'affiliées.

Pour arrêter l'exécution du serveur, il faut se placer au niveau du terminal où il a été lancé et appuyer sur Ctrl+z.

Si vous relancer le serveur dans le même terminal, l'erreur suivante s'affichera sur votre écran :
ERROR bind: code d'erreur 98
Donc pour contourner cet erreur il convient de faire un kill -9 de tous les processus miniftpd. Soit en récupérant leurs PID avec la commande ps, soit en récupérant directement ceux affichés sur l'écran après l'exécution de chaque requête.




/******* INTERFACE CLIENT *******/


vous devez d'abord ouvrir un nouveau terminal. Pour exécuter les requêtes il faut entrer une des lignes de commandes suivantes :

./miniftp hostname get distfilename localfilename
./miniftp hostname put localfilename distfilename
./miniftp hostname del distfilename
./miniftp hostname dir distpathname

hostname --> le nom de l'hôte. Si vous travaillez en local utilisez : localhost
localfilename --> le nom du fichier local
distfilename --> le nom du fichier distant
disthpathname --> le chemin du répertoire distant

Des messages s'afficheront au fur et à mesure de l'exécution de chaque requête pour vous tenir informé de leur état d'avancement ainsi que l'issue de leur exécution (succès ou échec).




/******* SPECIFICATIONS TECHNIQUES *******/


L'archive contient les fichiers sources ainsi que les exécutables qui ont été compilés sous les machines de l'école (salle I101). Le Makefile qui a servi à la compilation est aussi fourni.

Vous trouverez aussi un autre Makefile (Makefile_old) celui-là a été fourni par notre encadrant.
