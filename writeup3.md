# GRUB/Physical Security Writeup

## Background
In the Linux world, GRUB is by far the most commonly used bootloader. It thus comes as no surprise that it is used here.

## How-to
To access the grub menu at boot, despite the __GRUB_FORCE_HIDDEN=TRUE__ and the __TIMEOUT=0__ settings being set, we can just hold shift
Once in the prompt, we can use tab to conveniently complete the boot entry configuration (in this case `live`)

But merely executing it wouldn't advance us any further.
Conveniently, linux provides a boot-time [kernel option](https://www.kernel.org/doc/html/v4.14/admin-guide/kernel-parameters.html) to replace the init process(PID 1), and grub allows parameters to be passed to the kernel!
Let's use it to replace the init daemon by __bash__.
``` bash
grub: live init=/bin/bash
[...]
root@BornToSecHackMe:/# whoami
root
root@BornToSecHackMe:/# id
uid=0(root) gid=0(root) groups=0(root)
```

Congratulation ! You're now root.

## Real world fixes and pitfalls
The lesson to take away from this is that physicial security is paramount, if someone can get a physicial access to a machine it likely can be compromised.
Let's look at some real world scenarios and fixes.
#### GRUB Password
[As seen in GRUB's documentation](https://www.gnu.org/software/grub/manual/grub/grub.html#Security), a mediocre fix could've been to put a bios supervisor password (Not applicable here however) to prevent booting from external medias and use grub's `password_pbkdf2` feature to prevent unauthorized modifications of the kernel boot.
This however doesn't protect the boot medium, which could still be read or altered.
#### Encryption
Encrypting the hard drive with [dm-crypt](https://en.wikipedia.org/wiki/Dm-crypt) [which is certified by the ANSSI](https://www.ssi.gouv.fr/entreprise/certification_cspn/sous-systeme-de-chiffrement-de-disques-dm-crypt-noyau-linux-4-4-2-cryptsetup-1-7-0/) would've foiled any attempt at decyphering or tempering with the root partition, but wouldn't protect against all bootloaders or hardware attacks.
#### The evil maid attack
Not much can be done about this, an attacker could modify firmwares, the hardware, or the bootloader if the device is left unattended.
Putting the bootloader on an external medium can at least protect the bootloader. Or an app like [Snowden's Haven](https://en.wikipedia.org/wiki/Haven_(software)) can be used.