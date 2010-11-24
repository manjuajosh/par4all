from __future__ import with_statement # this is to work with python2.5
from pyps import *
from subprocess import call
from os import remove
from shutil import rmtree

with workspace("cat.c",deleteOnClose=True) as w:
	w.fun.main.display()
	binary=w.compile(rep="toto")
	call("./"+binary)
	rmtree("toto")
	
	w.fun.main.run(["sed","-e",'s/cats/dogs/'])
	w.fun.main.display()
	binary=w.compile(rep="toto")
	call("./"+binary)
	rmtree("toto")



