QNX Document to refer

- "Conventions for Recursive Makefiles and Directories",
  https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent.html

- "Examples of creating Makefiles",
  https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_Examples.html

QNX will generate binary locally.
Please add non-revisioned file into .gitignore
For example:
--------- Cut start ---------
*.o
*.a
*.so
*.pinfo
YourExecutionFile
--------- Cut stop ---------

Add Makefile.dnm will let this folder as Do-Not-Make

cf. https://www.qnx.com/developers/docs/7.0.0/#com.qnx.doc.neutrino.prog/topic/make_convent_recurse.mk.html


