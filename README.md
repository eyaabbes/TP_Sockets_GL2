# TP_Sockets_GL2
but de la partie 3
But de la partie

Développer une application client/serveur en mode connecté TCP où le client envoie 60 fois l’heure courante au serveur.
Observation attendue

Le protocole TCP assure une transmission fiable et ordonnée. Ainsi, même si le serveur traite les messages plus lentement avec sleep(1), il ne devrait pas y avoir de perte de messages au niveau de l’application. Les segments observés avec Wireshark ne correspondent pas nécessairement un-à-un aux appels send() du programme.

Rôle du serveur

Le serveur doit :

    attendre une connexion d’un client
    accepter cette connexion
    recevoir les messages envoyés
    afficher les messages
    compter combien de messages il a reçus

Rôle du client

Le client doit :

    créer une socket TCP
    se connecter au serveur
    envoyer Bonjour
    envoyer 60 fois l’heure courante
    envoyer Au revoir
    afficher combien de messages il a envoyés





    Donc le serveur n’a fait que 12 appels recv() avec des données.
Est-ce qu’il y a une erreur ?
Non, ce n’est pas une erreur

C’est normal en TCP.

Et c’est même une réponse importante à la question du TP :

    Les segments TCP envoyés correspondent-ils exactement aux écritures que votre programme fait sur la socket ?

Réponse :

Non, pas forcément.
Pourquoi le serveur reçoit 12 au lieu de 62 ?

Parce que TCP est un protocole en flux.

Ça veut dire que TCP ne garde pas la notion de “message” comme toi tu l’as dans ton programme.

Quand le client fait plusieurs :

send(...)

le système peut :

    regrouper plusieurs envois ensemble
    les envoyer dans moins de segments
    et le serveur peut les recevoir par blocs avec recv()

Donc :
plusieurs send() côté client

peuvent devenir
un seul recv() côté serveur
  

  Observation

Le client a envoyé 62 messages, mais le serveur n’a compté que 12 réceptions. Cela ne signifie pas qu’il y a eu une perte. En TCP, les données sont transmises sous forme de flux, et plusieurs écritures send() peuvent être regroupées dans une même réception recv().
Conclusion

Les segments TCP et les appels recv() ne correspondent pas nécessairement un à un aux appels send() du programme. TCP garantit la livraison ordonnée et fiable des données, mais ne conserve pas la frontière entre les messages applicatifs.
Donc : y a-t-il des pertes ?
Probablement non

Ici, il n’y a pas de perte.
Le serveur a bien affiché les messages, mais il les a reçus par paquets/groupes.
Si tu veux compter les vrais messages

Il faut découper le contenu reçu selon les retours à la ligne \n.

Parce que chaque message envoyé finit par \n.

Donc :

    recv() reçoit un bloc
    puis on cherche combien de lignes il contient
    et on compte les messages réels

Pourquoi les heures sont presque toutes identiques ?

Parce que le client envoie les 60 messages très vite, dans la même seconde.

Donc il affiche souvent :

Il est 18:11:24 !

plusieurs fois.

C’est normal aussi



ajouter un délai environ 1 SECONDE SLEEP(1)