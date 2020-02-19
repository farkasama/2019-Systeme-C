#SUJET DISPONIBLE EN PDF

# Sujet de base

## 1 Files de messages

Le but du projet est d’implémenter les files de messages pour une communications entre
les processus sur la même machine (pas de communication par réseau). Vous devez faire une
implémentation en utilisant la mémoire partagée entre les processus, en particulier une solution
qui implémente les files de messages à l’aide de files de messages existantes (POSIX ou System V)
ne sera pas acceptée.
L’accès parallèle à la file de messages doit être possible avec une protection appropriée soit avec
des sémaphores POSIX soit avec les mutexes/conditions.


### 1.1 Fonctions à implémenter

1.1.1 msgconnect

MESSAGE *msg_connect( const char *nom, int options,
size_t nb_msg, size_t len_max )

msg_connect() permet soit de se connecter à une file de message existante soit créer et se
connecter à une nouvelle file de messages.
nom est le nom de la file ou NULL pour une file anonyme, cf. section 4.
nb_msg est la capacité de la file c’est à dire le nombre minimal de messages qu’on peut stocker
avant que la file devienne pleine. (Si on stocke les messages de fa ̧con compacte on pourra stocker
plus que nb_msg messages, section 6.2, vous devez garantir que la file pourra stocker au moins
nb_messages.)
len_max est la longueur maximale d’un message.
options est un "OR" bit-à-bit de constantes suivantes :
— O_RDWR,O_RDONLY,O_WRONLY avec la même signification que pouropen. Exactement une
de ces constantes doit être spécifiée.
— O_CREAT pour demander la création de la file,
— O_EXCL avec O_CREAT indique qu’il faut créer la file seulement si elle n’existe pas, si la file
existe msg_connect doit échouer.
Si options ne contient pas O_CREAT alors la fonctionmsg_connectn’aura que deux paramètres
nom et options,msg_connect sera implémenté comme une fonction à nombre variable d’arguments.
msg_connect retourne un pointeur vers un objet MESSAGE qui identifie la file de messages et
sera utilisé par d’autres fonctions, voir section 3 pour la description de type MESSAGE.
En cas d’échec msg_connect retourne NULL.

1.1.2 msgdisconnect

int msg_disconnect(MESSAGE *file)

déconnecte le processus de la file de messages (la file n’est pas détruite, maisMESSAGE *devient
inutilisable).
msg_disconnectretourne 0 si OK et−1 en cas d’erreur.

1.1.3 msgunlink

int msg_unlink(const char *nom)

supprime la file de messages. La suppression a lieu quand le dernier processus se déconnecte
de la file. Par contre une foismsg_unlinkexécutée par un processus, toutes les tentatives de
msg_connectdoiventéchouer.
Valeur de retour : 0 si OK,−1 siéchec.msg_unlinkest juste une petite fonction qui fait appel
àshm_unlinkpour supprimer l’objet mémoire implémentant la file de messages.

2.1.4 msgsend et msgtrysend

int msg_send(MESSAGE *file, const void *msg, size_t len)
int msg_trysend(MESSAGE *file, const void *msg, size_t len)


msg_sendenvoie le message dans la file.msgest le pointeur vers le message à envoyer etlenla
longueur du message en octets.
msg_sendbloque le processus appelant jusqu’à ce que le message soit envoyé. Il y a essentielle-
ment deux situations dans lesquelles le processus appelant peut être bloqué :

(1) la file est pleine,

(2) quand plusieurs processus envoient des messages seulement un seul peut effectivement mettre le
message dans la file, les autres doivent attendre (comportement à implémenter avec sémaphores
ou mutexes/conditions pour garantir l’intégrité de données).
msg_trysend()est une version non bloquante demsg_send. Si la file est pleinemsg_trysend
retourne immédiatement−1 et met la valeurEAGAINdanserrno.
On vous laisse choisir ce que doit fairemsg_trysendlorsque la file n’est pas vide mais qu’il y
a juste au même moment un autre processus qui envoie un message (e.g. bloquage, bloquage mais
pendant un temps limité ou bien retour immédiat de−1).
Si la longueur du message est plus grande que la longueur maximale supportée par la file les
deux fonctions retournent immédiatement−1 et mettentEMSGSIZEdanserrno.
En général, les deux fonctions retournent 0 si OK et−1 en cas d’erreur.

1.1.5 msgreceive et msgtryreceive

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len)
ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len)

msg_receivelit le premier message sur la file, le met à l’adressemsget le supprime de la file.
La file de messages est une file FIFO, si onécrit les messages a,b,c dans cette ordre alors les
lectures successives retournent a,b,c dans le même ordre.
lenest utilisé pour indiquer la longueur du tamponmsg. Si cette longueur est inférieure à la
taille maximale d’un message les deux fonctions retournent−1 et mettentEMSGSIZEdanserrno.
msg_receivebloque le processus appelant si la file est vide ou quand il y a un autre processus qui
lit le message. Commemsg_receivemodifie la file on ne peut pas effectuer deux lectures en même
temps : ce comportement doit être assuré en utilisant des sémaphores ou des mutexes/conditions.
msg_tryreceiveest une version non bloquante demsg_receive. Si la file est videmsg_tryreceive
retourne−1 et metEAGAINdanserrno.
Par contre si la file n’est pas vide mais qu’un autre processus est en train de lire un message
alors on vous laisse choisir ce que faitmsg_tryreceive(attente, attente limitée en temps, ou retour
immédiat comme pour la file vide).
En cas de réussite les deux fonctions retournent le nombre d’octets du message lu. En cas
d’échec elles retournent−1.

1.1.6 L’état de la file

size_t msg_message_size(MESSAGE *)
size_t msg_capacite(MESSAGE *)
size_t msg_nb(MESSAGE *)

Ces fonctions retournent respectivement la taille maximale d’un message, le nombre maximal
de messages dans la file, le nombre de messages actuellement dans la file.


## 2 Structures de données

### 2.1 MESSAGE

```
Le typeMESSAGEest une structure qui contient au moins leséléments suivants :
```
1. le type d’ouverture de la file de messages (lecture,écriture, lecture etécriture). Cette in-
    formation est nécessaire pour les opérationsmsg_sendetmsg_receive. Il faut vérifier que
    l’opération est autorisée, par exemplemsg_senddoitéchouer si la file aété ouverte seulement
    en lecture,
2. le pointeur vers la mémoire partagée qui contient la file.
    Si vous trouvez d’autres informations pertinentes à stocker dans la structureMESSAGEn’hésitez
pas à les ajouter.

### 2.2 Organisation de la mémoire partagée

```
La file de message contient deux sortes d’informations.
Au début on mettra une structure en-tête qui contient des informations générales sur l’état de
la file de messages, cf. section 2.2.2.
L’en-tête est suivi du tableau de messages.
(La structure en-tête et le tableau de messages peuvent faire partie d’une seule structure, c’est
à vous de fixer les détails de l’implémentation.)
Commen ̧cons par le tableau de messages.
```
```
2.2.1 Tableau de messages
```
Le plus simple est de stocker les messages dans un tableau. Cela peut être un tableau circulaire.
Dans ce cas il suffit maintenir dans l’en-tête deux indicesfirstetlast.firstest l’indice du
premierélément de la file, celui qui sera lu parmsg_receive.lastest l’indice de premierélément
libre de tableau, celui quemsg_sendutilisera pour placer le nouveau message.
Dans le tableau circulaire
— soitf irst < lastet leséléments de la file occupent les casesf irst, f irst+ 1,... , last−1,
— soitlast≤f irstet la file occupe les casesf irst, f irst+ 1,... , n− 1 , 0 , 1 ,... , last−1 o`un
est les nombres d’éléments dans le tableau.
Donc l’arithmétique des indices se fera modulon.
En particulier sif irst==lastalors le tableau est plein.
Pour distinguer le tableau plein de tableau vide on pourra supposer que f irst== −1 si le
tableau est vide.
Ce n’est pas la seule possibilité pour implémenter le tableau circulaire, faites ce qui vous
convient.
Vous pouvez aussi utiliser un tableau non circulaire tel que on ait toujoursf irst≤last. Les
́eléments de la file sont dans les casesf irst, f irst+ 1,... , last−1. Dans ce cas quand on place
le nouveauélément à la case de l’indicen−1 (nle nombre d’éléments de tableau) etf irst > 0
alors il faudra faire un shift de tous leséléments pour que le premierélément de la file se retrouve
à l’indice 0 etc.
Dans chaque message il faut stocker sa longueur donc chaque message est une structure qui
contient au moins le nombre d’octets dans le message (nécessaire pour la valeur de retour de
msg_receive) et le message lui-même.


2.2.2 En-tête

```
Dans l’en-tête de la file de message on stockera au moins les informations suivantes :
```
(1) la longueur maximale d’un message,

(2) la capacité de la file (le nombre minimal de messages que la file peut stocker),

(3) les deux indicesfirstetlastdans le tableau de messages,

(4) les sémaphores ou variables mutex/conditions nécessaires,

(5) tout autre information utile pour votre implémentation et qui m’échappe maintenant...

## 3 Files anonyme

Une file anonyme est créé par msg_connectdont le premier paramètre est NULL. Chaque
msg_connect(NULL,...)ouvre une nouvelle file anonyme. La file anonyme peut être partagée
uniquement par un processus qui a effectuémsg_connectà ses enfants engendrés après la création
de la file.
Le paramètreoptionsest ignoré pour une file anonyme, la file anonyme sera toujours ouverte
en lecture etécriture.
Commemsg_connect(NULL,...)ouvre chaque fois une nouvelle file anonyme les paramètres
nb_msgetlen_maxsont toujours obligatoires pourmsg_connect(NULL,...).
La file anonyme ne supporte pasmsg_unlink, elle disparaît automatiquement quand le dernier
processus se déconnecte avecmsg_disconnect.

## 4 Gestion d’accès parallèles

Les sémaphores ou variables mutex permettent gérer les accès parallèles de plusieurs processus
à une file de messages. Une solution triviale est bien sûr de bloquer l’accès à la file à tous les autres
processus pour chaque opération. Franchement, c’est sans intérêt comme solution.
Si la file est ni vide ni pleine un processus qui ajoute un message et un processus qui lit
un message n’utilisent pas la même partie de la mémoire partagée qui contient la file. Donc, en
principe, pas de raison qu’unécrivain bloque un lecteur et vice-versa.
Si plusieurs processus ajoutent des messages dans la file, quand le premier processus a changé
la valeur delastpour réserver la place pour son message pas la peine de faire poireauter le suivant
qui peut déjà commencer à exécutermsg_send()en même temps que le premier copie le message
dans la mémoire.
Une remarque similaire s’applique à des processus qui lisent les messages.

## 5 Organisation du code

Toutes les fonctions demandés doivent être regroupées dans un fichiermsg_file.caccompagné
demsg_file.hde telle sorte que chaque programme qui utilise les files de messages puisse faire
juste un include demsg_file.hpour être compilé (l’édition de liens ajouteramsg_file.oobtenu
par la compilation demsg_file.cmais c’est une autre histoire).
Les programmes de test doivent être dans des fichiers séparés.


```
Les noms de fichiersmsg_file.cetmsg_file.hne sont pas à modifier (si nous nous décidons
àécrire nos propres programmes pour tester vos fonctions alors il faut que include demsg_file.h
marche dans nos programmes).
```
## Deuxième partie

# Extensions

```
Le sujet de base est simple. Bien fait, il ne rapporte guère plus que la moyenne. Pour compter
sur une note plus généreuse, on vous propose des extensions. Faire au moins une des extensions
améliore sensiblement la note.
```
### 6.1 Lecture/écriture par lots

Implémenter la lecture et l’écriture de plusieurs messages d’un seul coup : analogue àreadvet
writevmais appliqués aux files de messages et non pas aux fichiers.
Les lectures/écritures par lots doivent être atomiques,écrire 10 messages d’un coup ce n’est par
́ecrire 10 messages dans une boucle un par un. Quand onécrit un lot denmessages alors
— il n’y a pas d’écriture partielle, soit onécritnmessages soit rien,
— lesnmessages doivent être consécutifs dans la file, il ne faut pas qu’ils soient intercalés avec
d’autre messages.
Pour la lecture d’un lot denmessages on suppose qu’une lecture partielle est possible : si la
file contientm < nmessages alors on lira seulement lesmmessages disponibles.
Les messages lus par lot doivent être consécutifs dans la file.

### 6.2 Compacter le tableau de messages

```
Si on déclare que chaque message contient au maximum 200 octets mais qu’en réalité les mes-
sages qui arrivent contiennent en moyenne 10 octets il y aura un gaspillage de place lors du stockage
de chaque messages dans un tableau. Pouréviter le gaspillage, il suffit de stocker les messages l’un
après l’autre sans"trou"entre eux mais en utilisant juste la quantité d’octets nécessaire. Comme
chaque message est stocké avec sa longueur ce n’est pas un problème. Juste une modification :
firstetlastne sont plus les indices dans le tableau de messages mais les adresses relatives du
premier octet du premier message et du premier octet libre.
Adresses relatives veut dire que ce ne sont pas des pointeurs, les adresse virtuelles après chaque
mmapsont différentes même simmapest effectué sur le même objet mémoire. Donc il faut stocker
dansfirstle décalage entre l’adresse du premier octet du premier message et l’adresse du début
de la mémoire partagée. La même remarque s’applique àlast.
```
### 6.3 Notifications

```
Un processus peut s’enregistrer sur la file de messages pour recevoir un signal quand un message
arrive dans la file. Donc au lieu demsg_receiveoumsg_tryreceive, un processus s’enregistre et
poursuit son exécution.
```

Quand le processus s’enregistre il doit indiquer quel signal il veut recevoir. Un seul processus
peut être enregistré à un moment donné, une tentative d’enregistrement d’un autre processus doit
́echouer.
Seul le processus enregistré peut annuler son enregistrement ce qui permet à un autre processus
de s’enregistrer.
Le signal est envoyé uniquement quand les conditions suivantes sont satisfaites :
— un message arrive dans la file vide et
— il n’y a aucun processus suspendu en attente d’un message.
Quand le signal de notification est envoyé le processus enregistré doit être automatiquement dé-
enregistré.


