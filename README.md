# install-log
Package manager idea for LFS (Linux from scratch). Fork of https://install-log.sourceforge.net with the intent to compile to modern glibc.

## Note: 
Feel free to fork from here, as im likely to follow in the footsteps of those before me; very infequently update any code thats here. I'm not much of a proper software developer, and my expirences with git are far and few between. I just wanted to fork the original because I got tired of having to re-modify the same 10 lines of code within the original because I kept accidently deleting the source whenever I finished installing LFS.

On top of all of that, im lazy as hell, so something something abandon all hope all ye who enter something something. 

# Installation
To compile: 
- ```$ make```

To install:
- ```
  # make install
  # touch {LOGDIR}/.timestamp
  ```
- Replace {LOGDIR} with the install-log directory that was chosen (Defaults to /var/install-logs/)

# Original README
---

install-log is a simple shell script to aid in package management when
installing from source code.  Run "install-log <package-name>" immediately
after you type "make install" (or whatever you do to install something), and
it'll make a list of all the changes to your filesystem to
/var/install-logs/<package-name> for you to review when uninstalling, upgrading,
or just simply taking account of your system.  It's also good for identifying
the owner of a particular file; just grep for it in /var/install-logs/*.

Type "make install" now to install install-log and a sample install-log.rc file.
It'll also test itself by making a log of its own installation.  Confirm the
results by looking at the contents of /var/install-logs.

install-log is hosted on SourceForge (http://install-log.sourceforge.net/).  To
download the latest cvs version:
```
$ export CVSROOT=":pserver:anonymous@cvs.sourceforge.net:/cvsroot/install-log"
# cvs login                      # Only run one time, ever
$ cvs checkout install-log       # To download for the first time
$ cvs update install-log         # To update a previous download
```
Enjoy.
