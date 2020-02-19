### Universit ́e Paris Diderot Projet Syst`emes avanc ́es Master 1 2018-

## 1 Modalit ́es

Le projet doit ˆetre r ́ealis ́e en binˆome ( ́eventuellement, en monˆome). Les soutenances auront
lieu en mai, la date pr ́ecise sera communiqu ́ee ult ́erieurement. Pendant la soutenance, les membres
d’un binˆome devront chacun montrer leur maˆıtrise de la totalit ́e du code.
Chaque ́equipe doit cr ́eer un d ́epˆot git priv ́e sur le [gitlab de l’UFR]
[http://moule.informatique.univ-paris-diderot.fr:8080/](http://moule.informatique.univ-paris-diderot.fr:8080/)

d`es le d ́ebut de la phase de codageet y donner acc`es en tant que Reporter `a tous les enseignants
de cours de Syst`emes avanc ́es : Wieslaw Zielonka, Benjamin Bergougnoux et Ilias Garnier. Le
d ́epˆot devra contenir un fichierequipedonnant la liste des membres de l’ ́equipe (nom, pr ́enom,
num ́ero ́etudiant et pseudo(s) sur le gitlab). Vous ˆetes cens ́es utiliser gitlab de mani`ere r ́eguli`ere
pour votre d ́eveloppement. Le d ́epˆot doit ˆetre cr ́e ́eavant le 10 avril au plus tard. Au moment
de la cr ́eation de d ́epˆot vous devez envoyer un mail `azielonka@irif.fravec la composition de
votre ́equipe (l’objet du mail doit ˆetreprojet sys2, c’est important si vous ne voulez pas que
votre mail se perde).
Le guide de connexion externe et la pr ́esentation du r ́eseau de l’UFR se trouvent sur :
[http://www.informatique.univ-paris-diderot.fr/wiki/doku.php/wiki/howto_connect](http://www.informatique.univ-paris-diderot.fr/wiki/doku.php/wiki/howto_connect)
[http://www.informatique.univ-paris-diderot.fr/wiki/doku.php/wiki/linux](http://www.informatique.univ-paris-diderot.fr/wiki/doku.php/wiki/linux)

Le projet doit ˆetre accompagn ́e d’unMakefileutilisable. Les fichiers doivent ˆetre compil ́es avec
les options-Wall -gsans donner lieu `a aucun avertissement (et sans erreurs bien ́evidemment).
La soutenance se fera `a partir du code d ́epos ́e sur gitlab etsur les machines de l’UFR(salles
2031 et 2032) : au d ́ebut de la soutenance vous aurez `a cloner votre projet `a partir de gitlab et le
compiler avecmake.
Vous devez fournir un jeu de tests permettant de v ́erifier que vos fonctions sont capables d’ac-
complir les tˆaches demand ́ees, en particulier quand plusieurs processus lanc ́es en parall`ele envoient
et r ́eceptionnent les messages.

## Premi`ere partie

# Sujet de base

## 2 Files de messages

Le but du projet est d’impl ́ementer les files de messages pour une communications entre
les processus sur la mˆeme machine (pas de communication par r ́eseau). Vous devez faire une
impl ́ementation en utilisant la m ́emoire partag ́ee entre les processus, en particulier une solution
qui impl ́emente les files de messages `a l’aide de files de messages existantes (POSIX ou System V)
ne sera pas accept ́ee.
L’acc`es parall`ele `a la file de messages doit ˆetre possible avec une protection appropri ́ee soit avec
des s ́emaphores POSIX soit avec les mutexes/conditions.


### 2.1 Fonctions `a impl ́ementer

2.1.1 msgconnect

MESSAGE *msg_connect( const char *nom, int options,
size_t nb_msg, size_t len_max )

msg_connect()permet soit de se connecter `a une file de message existante soit cr ́eer et se
connecter `a une nouvelle file de messages.
nomest le nom de la file ou NULL pour une file anonyme, cf. section 4.
nb_msgest la capacit ́e de la file c’est `a dire le nombre minimal de messages qu’on peut stocker
avant que la file devienne pleine. (Si on stocke les messages de fa ̧con compacte on pourra stocker
plus quenb_msgmessages, section 6.2, vous devez garantir que la file pourra stocker au moins
nb_messages.)
len_maxest la longueur maximale d’un message.
optionsest unORbit-`a-bit de constantes suivantes :
— O_RDWR,O_RDONLY,O_WRONLYavec la mˆeme signification que pouropen. Exactement une
de ces constantes doit ˆetre sp ́ecifi ́ee.
— O_CREATpour demander la cr ́eation de la file,
— O_EXCLavecO_CREATindique qu’il faut cr ́eer la file seulement si elle n’existe pas, si la file
existemsg_connectdoit ́echouer.
Sioptionsne contient pasO_CREATalors la fonctionmsg_connectn’aura que deux param`etres
nometoptions,msg_connectsera impl ́ement ́e comme une fonction `a nombre variable d’arguments.
msg_connectretourne un pointeur vers un objetMESSAGEqui identifie la file de messages et
sera utilis ́e par d’autres fonctions, voir section 3 pour la description de typeMESSAGE.
En cas d’ ́echecmsg_connectretourneNULL.

2.1.2 msgdisconnect

int msg_disconnect(MESSAGE *file)

d ́econnecte le processus de la file de messages (la file n’est pas d ́etruite, maisMESSAGE *devient
inutilisable).
msg_disconnectretourne 0 si OK et−1 en cas d’erreur.

2.1.3 msgunlink

int msg_unlink(const char *nom)

supprime la file de messages. La suppression a lieu quand le dernier processus se d ́econnecte
de la file. Par contre une foismsg_unlinkex ́ecut ́ee par un processus, toutes les tentatives de
msg_connectdoivent ́echouer.
Valeur de retour : 0 si OK,−1 si ́echec.msg_unlinkest juste une petite fonction qui fait appel
`ashm_unlinkpour supprimer l’objet m ́emoire impl ́ementant la file de messages.

2.1.4 msgsend et msgtrysend

int msg_send(MESSAGE *file, const void *msg, size_t len)
int msg_trysend(MESSAGE *file, const void *msg, size_t len)


msg_sendenvoie le message dans la file.msgest le pointeur vers le message `a envoyer etlenla
longueur du message en octets.
msg_sendbloque le processus appelant jusqu’`a ce que le message soit envoy ́e. Il y a essentielle-
ment deux situations dans lesquelles le processus appelant peut ˆetre bloqu ́e :

(1) la file est pleine,

(2) quand plusieurs processus envoient des messages seulement un seul peut effectivement mettre le
message dans la file, les autres doivent attendre (comportement `a impl ́ementer avec s ́emaphores
ou mutexes/conditions pour garantir l’int ́egrit ́e de donn ́ees).
msg_trysend()est une version non bloquante demsg_send. Si la file est pleinemsg_trysend
retourne imm ́ediatement−1 et met la valeurEAGAINdanserrno.
On vous laisse choisir ce que doit fairemsg_trysendlorsque la file n’est pas vide mais qu’il y
a juste au mˆeme moment un autre processus qui envoie un message (e.g. bloquage, bloquage mais
pendant un temps limit ́e ou bien retour imm ́ediat de−1).
Si la longueur du message est plus grande que la longueur maximale support ́ee par la file les
deux fonctions retournent imm ́ediatement−1 et mettentEMSGSIZEdanserrno.
En g ́en ́eral, les deux fonctions retournent 0 si OK et−1 en cas d’erreur.

2.1.5 msgreceive et msgtryreceive

ssize_t msg_receive(MESSAGE *file, void *msg, size_t len)
ssize_t msg_tryreceive(MESSAGE *file, void *msg, size_t len)

msg_receivelit le premier message sur la file, le met `a l’adressemsget le supprime de la file.
La file de messages est une file FIFO, si on ́ecrit les messages a,b,c dans cette ordre alors les
lectures successives retournent a,b,c dans le mˆeme ordre.
lenest utilis ́e pour indiquer la longueur du tamponmsg. Si cette longueur est inf ́erieure `a la
taille maximale d’un message les deux fonctions retournent−1 et mettentEMSGSIZEdanserrno.
msg_receivebloque le processus appelant si la file est vide ou quand il y a un autre processus qui
lit le message. Commemsg_receivemodifie la file on ne peut pas effectuer deux lectures en mˆeme
temps : ce comportement doit ˆetre assur ́e en utilisant des s ́emaphores ou des mutexes/conditions.
msg_tryreceiveest une version non bloquante demsg_receive. Si la file est videmsg_tryreceive
retourne−1 et metEAGAINdanserrno.
Par contre si la file n’est pas vide mais qu’un autre processus est en train de lire un message
alors on vous laisse choisir ce que faitmsg_tryreceive(attente, attente limit ́ee en temps, ou retour
imm ́ediat comme pour la file vide).
En cas de r ́eussite les deux fonctions retournent le nombre d’octets du message lu. En cas
d’ ́echec elles retournent−1.

2.1.6 L’ ́etat de la file

size_t msg_message_size(MESSAGE *)
size_t msg_capacite(MESSAGE *)
size_t msg_nb(MESSAGE *)

Ces fonctions retournent respectivement la taille maximale d’un message, le nombre maximal
de messages dans la file, le nombre de messages actuellement dans la file.


## 3 Structures de donn ́ees

### 3.1 MESSAGE

```
Le typeMESSAGEest une structure qui contient au moins les ́el ́ements suivants :
```
1. le type d’ouverture de la file de messages (lecture, ́ecriture, lecture et ́ecriture). Cette in-
    formation est n ́ecessaire pour les op ́erationsmsg_sendetmsg_receive. Il faut v ́erifier que
    l’op ́eration est autoris ́ee, par exemplemsg_senddoit ́echouer si la file a ́et ́e ouverte seulement
    en lecture,
2. le pointeur vers la m ́emoire partag ́ee qui contient la file.
    Si vous trouvez d’autres informations pertinentes `a stocker dans la structureMESSAGEn’h ́esitez
pas `a les ajouter.

### 3.2 Organisation de la m ́emoire partag ́ee

```
La file de message contient deux sortes d’informations.
Au d ́ebut on mettra une structure en-tˆete qui contient des informations g ́en ́erales sur l’ ́etat de
la file de messages, cf. section 3.2.2.
L’en-tˆete est suivi du tableau de messages.
(La structure en-tˆete et le tableau de messages peuvent faire partie d’une seule structure, c’est
`a vous de fixer les d ́etails de l’impl ́ementation.)
Commen ̧cons par le tableau de messages.
```
```
3.2.1 Tableau de messages
```
Le plus simple est de stocker les messages dans un tableau. Cela peut ˆetre un tableau circulaire.
Dans ce cas il suffit maintenir dans l’en-tˆete deux indicesfirstetlast.firstest l’indice du
premier ́el ́ement de la file, celui qui sera lu parmsg_receive.lastest l’indice de premier ́el ́ement
libre de tableau, celui quemsg_sendutilisera pour placer le nouveau message.
Dans le tableau circulaire
— soitf irst < lastet les ́el ́ements de la file occupent les casesf irst, f irst+ 1,... , last−1,
— soitlast≤f irstet la file occupe les casesf irst, f irst+ 1,... , n− 1 , 0 , 1 ,... , last−1 o`un
est les nombres d’ ́el ́ements dans le tableau.
Donc l’arithm ́etique des indices se fera modulon.
En particulier sif irst==lastalors le tableau est plein.
Pour distinguer le tableau plein de tableau vide on pourra supposer que f irst== −1 si le
tableau est vide.
Ce n’est pas la seule possibilit ́e pour impl ́ementer le tableau circulaire, faites ce qui vous
convient.
Vous pouvez aussi utiliser un tableau non circulaire tel que on ait toujoursf irst≤last. Les
́el ́ements de la file sont dans les casesf irst, f irst+ 1,... , last−1. Dans ce cas quand on place
le nouveau ́el ́ement `a la case de l’indicen−1 (nle nombre d’ ́el ́ements de tableau) etf irst > 0
alors il faudra faire un shift de tous les ́el ́ements pour que le premier ́el ́ement de la file se retrouve
`a l’indice 0 etc.
Dans chaque message il faut stocker sa longueur donc chaque message est une structure qui
contient au moins le nombre d’octets dans le message (n ́ecessaire pour la valeur de retour de
msg_receive) et le message lui-mˆeme.


3.2.2 En-tˆete

```
Dans l’en-tˆete de la file de message on stockera au moins les informations suivantes :
```
(1) la longueur maximale d’un message,

(2) la capacit ́e de la file (le nombre minimal de messages que la file peut stocker),

(3) les deux indicesfirstetlastdans le tableau de messages,

(4) les s ́emaphores ou variables mutex/conditions n ́ecessaires,

(5) tout autre information utile pour votre impl ́ementation et qui m’ ́echappe maintenant...

## 4 Files anonyme

Une file anonyme est cr ́e ́e par msg_connectdont le premier param`etre est NULL. Chaque
msg_connect(NULL,...)ouvre une nouvelle file anonyme. La file anonyme peut ˆetre partag ́ee
uniquement par un processus qui a effectu ́emsg_connect`a ses enfants engendr ́es apr`es la cr ́eation
de la file.
Le param`etreoptionsest ignor ́e pour une file anonyme, la file anonyme sera toujours ouverte
en lecture et ́ecriture.
Commemsg_connect(NULL,...)ouvre chaque fois une nouvelle file anonyme les param`etres
nb_msgetlen_maxsont toujours obligatoires pourmsg_connect(NULL,...).
La file anonyme ne supporte pasmsg_unlink, elle disparaˆıt automatiquement quand le dernier
processus se d ́econnecte avecmsg_disconnect.

## 5 Gestion d’acc`es parall`eles

Les s ́emaphores ou variables mutex permettent g ́erer les acc`es parall`eles de plusieurs processus
`a une file de messages. Une solution triviale est bien sˆur de bloquer l’acc`es `a la file `a tous les autres
processus pour chaque op ́eration. Franchement, c’est sans int ́erˆet comme solution.
Si la file est ni vide ni pleine un processus qui ajoute un message et un processus qui lit
un message n’utilisent pas la mˆeme partie de la m ́emoire partag ́ee qui contient la file. Donc, en
principe, pas de raison qu’un ́ecrivain bloque un lecteur et vice-versa.
Si plusieurs processus ajoutent des messages dans la file, quand le premier processus a chang ́e
la valeur delastpour r ́eserver la place pour son message pas la peine de faire poireauter le suivant
qui peut d ́ej`a commencer `a ex ́ecutermsg_send()en mˆeme temps que le premier copie le message
dans la m ́emoire.
Une remarque similaire s’applique `a des processus qui lisent les messages.

## 6 Organisation du code

Toutes les fonctions demand ́es doivent ˆetre regroup ́ees dans un fichiermsg_file.caccompagn ́e
demsg_file.hde telle sorte que chaque programme qui utilise les files de messages puisse faire
juste un include demsg_file.hpour ˆetre compil ́e (l’ ́edition de liens ajouteramsg_file.oobtenu
par la compilation demsg_file.cmais c’est une autre histoire).
Les programmes de test doivent ˆetre dans des fichiers s ́epar ́es.


```
Les noms de fichiersmsg_file.cetmsg_file.hne sont pas `a modifier (si nous nous d ́ecidons
`a ́ecrire nos propres programmes pour tester vos fonctions alors il faut que include demsg_file.h
marche dans nos programmes).
```
## Deuxi`eme partie

# Extensions

```
Le sujet de base est simple. Bien fait, il ne rapporte gu`ere plus que la moyenne. Pour compter
sur une note plus g ́en ́ereuse, on vous propose des extensions. Faire au moins une des extensions
am ́eliore sensiblement la note.
```
### 6.1 Lecture/ ́ecriture par lots

Impl ́ementer la lecture et l’ ́ecriture de plusieurs messages d’un seul coup : analogue `areadvet
writevmais appliqu ́es aux files de messages et non pas aux fichiers.
Les lectures/ ́ecritures par lots doivent ˆetre atomiques, ́ecrire 10 messages d’un coup ce n’est par
́ecrire 10 messages dans une boucle un par un. Quand on ́ecrit un lot denmessages alors
— il n’y a pas d’ ́ecriture partielle, soit on ́ecritnmessages soit rien,
— lesnmessages doivent ˆetre cons ́ecutifs dans la file, il ne faut pas qu’ils soient intercal ́es avec
d’autre messages.
Pour la lecture d’un lot denmessages on suppose qu’une lecture partielle est possible : si la
file contientm < nmessages alors on lira seulement lesmmessages disponibles.
Les messages lus par lot doivent ˆetre cons ́ecutifs dans la file.

### 6.2 Compacter le tableau de messages

```
Si on d ́eclare que chaque message contient au maximum 200 octets mais qu’en r ́ealit ́e les mes-
sages qui arrivent contiennent en moyenne 10 octets il y aura un gaspillage de place lors du stockage
de chaque messages dans un tableau. Pour ́eviter le gaspillage, il suffit de stocker les messages l’un
apr`es l’autre sanstrouentre eux mais en utilisant juste la quantit ́e d’octets n ́ecessaire. Comme
chaque message est stock ́e avec sa longueur ce n’est pas un probl`eme. Juste une modification :
firstetlastne sont plus les indices dans le tableau de messages mais les adresses relatives du
premier octet du premier message et du premier octet libre.
Adresses relatives veut dire que ce ne sont pas des pointeurs, les adresse virtuelles apr`es chaque
mmapsont diff ́erentes mˆeme simmapest effectu ́e sur le mˆeme objet m ́emoire. Donc il faut stocker
dansfirstle d ́ecalage entre l’adresse du premier octet du premier message et l’adresse du d ́ebut
de la m ́emoire partag ́ee. La mˆeme remarque s’applique `alast.
```
### 6.3 Notifications

```
Un processus peut s’enregistrer sur la file de messages pour recevoir un signal quand un message
arrive dans la file. Donc au lieu demsg_receiveoumsg_tryreceive, un processus s’enregistre et
poursuit son ex ́ecution.
```

Quand le processus s’enregistre il doit indiquer quel signal il veut recevoir. Un seul processus
peut ˆetre enregistr ́e `a un moment donn ́e, une tentative d’enregistrement d’un autre processus doit
́echouer.
Seul le processus enregistr ́e peut annuler son enregistrement ce qui permet `a un autre processus
de s’enregistrer.
Le signal est envoy ́e uniquement quand les conditions suivantes sont satisfaites :
— un message arrive dans la file vide et
— il n’y a aucun processus suspendu en attente d’un message.
Quand le signal de notification est envoy ́e le processus enregistr ́e doit ˆetre automatiquement d ́e-
enregistr ́e.


