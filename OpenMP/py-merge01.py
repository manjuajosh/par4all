from __future__ import with_statement
from pyps import *
import openmp

with workspace("merge01.c") as w:
	w.all_functions.openmp(verbose=True)
