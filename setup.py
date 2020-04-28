from setuptools import setup,Extension
import setuptools
from setuptools import find_packages
import pathlib

# The actual C extension
jc_module = Extension('jcalg1', sources = ['src\main.cpp'], libraries =["src\jcalg1_static"])
    
# The directory containing this file
HERE = pathlib.Path(__file__).parent

# The text of the README file
README = (HERE / "README.md").read_text()

# This call to setup() does all the work
setup(
    name="jcalg1",
    version="1.0.4",
    description="Interface to the JCALG1 compression library",
    long_description=README,
    long_description_content_type="text/markdown",
    url="https://github.com/CallMeAlexO/jcalg1",
    author="Alex Osheter",
    author_email="alex.osheter@gmail.com",
    license="MIT",
    classifiers=[
        "License :: OSI Approved :: MIT License",
        "Programming Language :: Python :: 3",
        "Programming Language :: Python :: 3.7",
    ],
    ext_modules = [ jc_module ],
    packages=find_packages(),
    headers=['src\jcalg1.h']
)