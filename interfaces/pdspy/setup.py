###############################################################################
# PROJECT:  PLC data server library (Python interface)
# MODULE:   setup.py
# PURPOSE:  Packaging configuration for the PLC data server Python API module
# AUTHOR:   Paul M. Breen
# DATE:     2018-07-07
###############################################################################

from setuptools import setup

setup(name='pds',
      version='0.1',
      description='Python bindings for the Plc Data Server (PDS)',
      url='https://github.com/paul-breen/plc-data-server',
      author='Paul Breen',
      author_email='paul.breen6@btinternet.com',
      license='Apache 2.0',
      packages=['pds'],
      package_data={'': ['_pds.so']},
      zip_safe=False)

