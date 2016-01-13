# Introduction #

L’objectif de ce projet est de développer un outil d’analyse de trafic sur un réseau en temps réel. Cet outil sera paramétrable par l’utilisateur et permettra de remonter des alarmes dans le cas où des données suspectes sont détectées. L’outil doit aussi permettre de calculer des statistiques et de caractériser le comportement des utilisateurs du réseau.

# Première partie du projet : Définir le protocole entre le serveur et le client #
Pour ce faire, deux solutions nous ont été avancées :

**Solution 1:**

> -Le serveur écrit des fichiers contenant les paquets captés(on aura donc une liste des paquets).

> -FTP like

**Solution 2:**

> -Le client fait une requête de préfiltrage au serveur.

> -Le serveur capture les paquets correspondants et les écrit dans un fichier(un fichier par client).

> -Le client récupère le fichier et l'analyse.

**Solution choisie**

Nous avons opté pour la première solution.Pour ce qui concerne la première partie du projet, le travail se divise en deux étapes:

1)Créer le serveur:

> -capturer des paquets

> -écriture de ces paquets au format pcap dans des fichiers(par exemple un fichier est créé chauqe minute)

> -partie connection avec le client:

> -Sockets

> -petit protocole List GET

2)client:

> -partie connection avec le serveur:

> -les sockets

> - petit protocole List Get

> -partie filtrage

> -partie analyse