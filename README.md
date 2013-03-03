# Basics
This program is a program which you can use to generate timing data for streams of 
photons using poisson processes. Once a stream has been generated, you can use the
estimators provided to try and estimate the function that you used to generate the
photon stream.

To run, first follow the instructions in INSTALL, and then run the `launcher` file 
found in the src directory:

`src/launcher`

This will output some useful information about switches that you can use.

# Required libraries
To compile, you will need muParser (http://muparser.beltoforion.de/), the
GNU Scientific Library, gsl (http://www.gnu.org/software/gsl), and check
(http://check.sourceforge.net/). If you are running Ubuntu or some other apt-based
system, you should be able to install the required libraries using:

`apt-get install check libgsl0-dev libmuparser-dev`
