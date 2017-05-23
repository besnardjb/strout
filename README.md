DESCRIPTION
===========

Strout is a prototype implementation of the structured output DSL allowing (with
the help of MPI Streams) several MPI processes to generate a structured HTML
report only with text commands on their standard output.

INSTALL
======

Load an MPI in your environment and type:

```
make
```

This should create binaries in the ./bin/ directory.


Go to the ./strout/ directory.

Make sure you have the libcairo package installed. Make sure you have
[Node.JS](https://nodejs.org/en/) installed.

Download and compile Node dependencies:

```
cd ./strout/
npm install
```

You may encounter errors read carefully the output. It is highly probable that
it is linked to node-canvas, make sure you have installed all the dependencies
as detailed on their [website](https://github.com/Automattic/node-canvas).

You'll need Gnuplot and Graphviz to be able to generate all kinds of outputs.

```
yum install gnuplot graphviz
```

If you have sufficient rights you can install the "strout" command:

```
cd ./strout/
npm install -g
```

otherwise you may run it with :

```
node ./strout/main.js
```

USE
===

Validated on Linux:

```

mpirun -np 4 ./bin/mandel | node ./strout/main.js

```

If installed:

```
mpirun -np 4 ./bin/mandel | strout
```

There aren't many option for now, by defaut the progam aborts if a report
directory is already present. Once run you'll find the output in this directory.
Target directory can be modified with the *STROUT_DIR* environment variable. 

Licence
=======

This code is CECILL-C a fully LGPL compatible licence.
