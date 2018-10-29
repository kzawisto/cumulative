
import os
from setuptools import setup

from setuptools.extension import Extension
from Cython.Build import cythonize

dirname, filename = os.path.split(os.path.abspath(__file__))

import fnmatch
import os

matches = []
for root, dirnames, filenames in os.walk(dirname +'/src'):
    for filename in fnmatch.filter(filenames, '*.cxx'):
        matches.append(os.path.join(root, filename))
setup(
    name="ks2d",
    version="0.0.3",
    description=("ks2d"),
    license="prop.",
    ext_modules=cythonize(Extension(
        "ks2d.ext", include_dirs = ["."],sources=["pyiface/ext.pyx"] + matches,language="c++",
        extra_compile_args=["-std=c++14","-g","-O2"], extra_link_args=["-std=c++14","-g","-O2"]
        ))
    )