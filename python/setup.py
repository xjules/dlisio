#!/usr/bin/env python3

import os
from setuptools import setup, Extension
from setuptools.command.build_ext import build_ext

class get_pybind_include(object):
    def __init__(self, user=False):
        self.user = user

    def __str__(self):
        # postpone importing pybind11 until building actually happens
        import pybind11
        return pybind11.get_include(self.user)


class BuildExt(build_ext):
    """
    A custom build extension for adding compiler-specific, taken from
    https://github.com/pybind/python_example/blob/master/setup.py
    """
    c_opts = {
        'msvc': ['/EHsc'],
        'unix': ['-std=c++11'],
    }

    def build_extensions(self):
        ct = self.compiler.compiler_type
        opts = self.c_opts.get(ct, [])

        distver = self.distribution.get_version()
        if ct == 'unix':
            opts.append('-DVERSION_INFO="{}"'.format(distver))
            opts.append('-fvisibility=hidden')
        elif ct == 'msvc':
            opts.append('/DVERSION_INFO=\\"{}\\"'.format(distver))

        for ext in self.extensions:
            ext.extra_compile_args = opts
        build_ext.build_extensions(self)

def getversion():
    if os.path.isdir('.git'):
        print('Getversion == True')
        return True

    return {
        'relative_to': os.path.dirname(os.path.abspath(__file__)),
    }

setup(
    name = 'dlisio',
    description = 'DLIS v1',
    long_description = 'DLIS v1',
    url = 'https://github.com/Statoil/dlisio',
    packages = ['dlisio'],
    license = 'LGPL-3.0',
    ext_modules = [
        Extension('dlisio.core',
            sources = [
                'dlisio/ext/core.cpp',
            ],
            include_dirs = ['../lib/include',
                            '../lib/extension',
                            get_pybind_include(),
                            get_pybind_include(user=True),
            ],
            libraries = ['dlisio', 'dlisio-extension'],
        )
    ],
    platforms = 'any',
    install_requires = ['numpy'],
    setup_requires = ['setuptools >= 28',
                      'pytest-runner',
                      'pybind11 >= 2.2',
                      'setuptools_scm',
    ],
    tests_require = ['pytest', 'hypothesis'],
    cmdclass = { 'build_ext': BuildExt },
    use_scm_version = getversion(),
)
