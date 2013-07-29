# Usage
## Installation
This installation guide is intended for users of Linux distributions,
particularly those which are Ubuntu based. The program has been tested on
Linux Mint 13 and 14, but should work on most Linux distributions. First,
download the latest version of the program from
https://github.com/heuristicus/final-year-project/tags and extract it with
your favourite program. Alternatively, clone the current version of the
repository with 
   
    git clone https://github.com/heuristicus/final-year-project.git
   
Before the program can be configured, we must install some libraries without
which the program will not run. Download the latest muParser package from
http://sourceforge.net/projects/muparser/files/latest/download (must be > v2.2.3).
Then, run the following commands

    unzip muparser_v_[your_version]
    cd muparser_v_[your_version]
    ./configure --prefix=/usr
    make && make install // may require sudo

This will install muParser so that the header files it uses can be found in
`/usr/include`. Your system must have the `g++` package
installed for the `configure` command to complete, and you may also
require the `autoconf` package. We must also install the GNU
Scientific Library and the Check test framework. All the required packages
can be installed with
   
    apt-get install libgsl0-dev check g++ autoconf

Once this is done, go to the top level directory of the project, and run
`make`. This should create all the files required for the system to run. If you
require special configuration options, you can use `./configure` with the
required switches.
   
## General Usage
The executable for the program can be found in the `src` directory,
and is named `deltastream`. It can be run from the top level directory
with
   

    src/deltastream [OPTIONS]

   
To find out what options are available, call the executable with the
`-h` or `--help` options. We will detail some of the options
below. All parameters which govern the behaviour of the system are defined in
the parameter files, which have information about what the effect of each is.
### Parameter files
Some parameter files are provided with the program, but if for some reason
they are deleted, then additional ones can be created using
    

    deltastream -d paramfile.txt // default
    deltastream -d paramfile.txt -x a // experiment

    
### Generating Functions
The `-g` switch is used to run all generation functions. Generating a
random function can be done in one of two ways. Using
    

    deltastream -g params.txt -r -c 1

    
We can generate a file containing a Gaussian representation of a random
function which we can use to generate streams. Changing the number passed
to the `-c` switch changes the number of functions generated. To
generate streams from the functions, we use
    

    deltastream -g params.txt -f rand -n 2 -i random_function_0.dat

    
This takes the data in the file `random_function_0.dat`, generated
in the previous step, and generates two streams. Modifying the number passed
to the `-n` option will generate different numbers of
streams. Another way to generate random functions is with
    

    deltastream -g params.txt -f rand -c 3 -n 2

    
The `-c` switch defines how many functions should be generated. After
the functions are generated, two streams are generated from each. If you
wish to generate multiple different pairs of streams from the same function,
use
    

    deltastream -g params.txt -f rand -c 3 -n 2 -u

    
The first function generated will be copied into multiple files, and streams
will be generated from those copied files. The `-t` switch can be
used to specify more or less verbose output. For example, passing a value of
3 will output bin counts for the streams, and a file containing the sum of
Gaussians which make up the random function.

The generation of streams from expressions is rather simpler. The following
two commands are equivalent.
    

    deltastream -g params.txt -n 2
    deltastream -g params.txt -f mup -n 2

    
The generator defaults to generating streams from the expression defined in
the parameter file. Multiple pairs can be generated using the `-c` switch.
### Estimating Functions and Time Delay
Estimates of functions are done using the `-e` switch. The most
important parameters are defined in the parameter file. Once streams have
been generated, we can estimate them using the baseline estimator


    deltastream -e params.txt -a base -n 2

    
If the streams were generated from a random function, the `-r` switch
must be added to indicate this fact. Again, if there are multiple functions
to estimate at once, use the `-c` switch to specify the number. The
`-a` option has 5 possible arguments (`ols`, `iwls`,
`pc`, `base` and `gauss`), each of which use a
different estimator to produce an estimate. Passing a value larger than 1 to
the `-n` option will result in an estimate of the time delay. To
estimate only the function, simply omit the switch.
### Plotting Output
The `scripts/plot.sh` script can be used to plot various data which
is output from the system. Calling it with the `-h` option will
output information about what plots can be made. The script generates a
`.tex` file using `gnuplot`, which it then processes into a
`.pdf` and displays using `evince`. After doing a function
estimate with the baseline estimator, the generating function can be plotted
along with the bin data and estimate using 

    scripts/plot.sh -f output random_function_0_sum.dat est_out.dat
    random_function_0_output_stream_0_bins.dat

## Running Experiments
### Creating Functions for Experimentation
Using the `genfunc_rand.sh` script found in the `scripts` directory, random
functions can be generated, conforming to certain parameters. In this file,
we specify the directory to which to output by modifying the
`OUTPUT_DIR` parameter. The `LAUNCHER_LOC` parameter specifies the
location of the `deltastream` executable used to run the program. The
`PARAM_FILE` parameter defines the location of the parameter file to use
to generate the functions.

Once these have been set, we specify the values to use to generate the
function. The values in the the `AVALS` parameter define what values of
$alpha$ will be used to generate the functions. The `DIVISOR` parameter
specifies what to divide the values in `AVALS` by when modifying the
$alpha$ parameter in the parameter file. This can be set to 1 to just use the
values inside the array. The values in the `AVALS` array are also used to
create directories, so the divisor is also used to prevent creation of
directories such as `alpha_0.3`. The `NFUNCS` parameter defines
how many different functions to generate. `NPAIRS` defines the number of
pairs of streams that will be generated from each function. Streams generated
will be copies of the function. For example, when `NPAIRS` is set to 5, a
function $f(a)$ is generated, along with two streams. Then, four more streams
are generated from the same function $f(a)$. This allows for multiple trials on
similar data. The `FPREF` and `APREF` define the text that is
prepended to the directories. Setting `FPREF` to `function_` and
`APREF` to `alpha_` will put each set of functions in a directory
structure like `alpha_1/function_1`.
### Generating Model Selection Data
Next, we use the `stutter_batch.sh` script to generate streams with data
removed in certain intervals to use for model selection. Here, we set the
`INDIR` parameter to the directory which we set as the output directory
in the previous script, and make sure to set the `AVALS`, `NFUNCS`
and `NPAIRS` parameters to the same values. We must also define the
`EXP_PFILE` parameter, which tells the script where to look for the
experimental parameters. In this file, we must set up which data should be
removed. Modifying values in the `setup` section of the experiment
parameter file will allow the choosing of various intervals. To generate a
default experiment parameter file, use `deltastream -d [filename] -x
a`. Once this is set up, we run the script, and it generates a new set of files
in the same location as the original data which has data in some intervals
removed, with names something like `random_function_0_output_stream_0_stuttered.dat`.
### Experiment Parameter Setup
Now, we set up the experiments that we wish to perform on the data. In the
experiment parameter file, there are various options which control how the
experiments are run. The most important is the `experiment_names`
parameter, which defines the names of the experiments that you wish to run. Once
the names are set, we must define four parameters that are used to run the experiment.

    experiment_names exp_1,exp_2 // Name the experiments
    // These parameters will be varied during the experiments
    exp_1_params base_max_breakpoints,base_max_extension
    exp_2_params gauss_est_stdev
    test_exp_1 yes // We want to experiment on this
    test_exp_2 no // This will not be experimented on
    // Set the estimator to use for the experiment
    exp_1_estimator base 
    ext_2_estimator gauss
    // Estimate the function or the time delay
    exp_1_type function
    exp_2_type delay

    // Set the parameter values for experiments
    base_max_extension 3,6,...,11
    base_max_breakpoints 4,5,...,10
    gauss_est_stdev 1,2,3,4

    // This is important! Set the time delay between streams
    // Used later to analyse the results
    timedelta 0,15

When setting the parameter values, `...` can be used to specify a
range. In the example, the `base_max_extension` parameters would be 3,
6, 9 and 11. The `timedelta` parameter is important as well---it provides
the program with the actual value of the time delay between streams, which is
used to determine the score of certain parameter settings. Information about the
parameters used to generate streams can be found in the output directories in
the `gen_params.txt` file.
### Running Model Selection
Once the parameters are set up, we run model selection on the generated streams
using the `runexp_batch.sh` script. Here we again set the various
parameters needed, and specify a new output directory into which the experiment
data is output. Depending on the number of experiments being run, the data can
take up a lot of space (on the order of gigabytes), so choose a disk with plenty
of free space. It is also a good idea to run a small subset of the experiments
before running them all, just to make sure that you are outputting to the
correct directory--- **data in the output directories from previous
experiments is overwritten**. Once you are sure that everything is good to go,
run the script. Time taken depends on the number of parameter combinations and
number of functions you are running the experiments on. A reasonably large set
of data (approximately 151,000 experiments) took approximately two hours on an
Intel i5 processor.
### Time Delay Calculation
Once the experiments have completed, we use the best parameter settings from the
model selection stage to run time delay estimators on the data again, this time
with all data available to the estimators. First, we use the
`get_goodness.sh` script to extract the experiment numbers of the
highest scoring parameter settings. Inside the `runtd_exp.sh` file, we
modify the relevant parameters, setting the parameter files to read from, the
directory from which to read the parameter data---the directory set as the
output directory for the model selection, the location in which the files output
from the `get_goodness.sh` script, and the place where we wish to put
the files produced by this stage of the process. When the script is run, it
performs a time delay estimation on the streams with the best parameters for
each function and $alpha$ value. Inside each directory, a file
`results.txt` is produced, which contains the some data about the
performance of the estimators with that combination of methods on the given
$alpha$ value for that function. In the next step, we extract this data into a
more usable form.
### Extracting Result Data
In the `extract_results.sh` script, we set up the parameters so that
`INDIR` is set to read from the top level of the time delay results
directory, and `OUTDIR` is set to the location to which we wish to output
the aggregated results. There are three different flags that can be set to
produce data in different forms for processing. The `TT` flag makes the
script output error data in a form in which it can be processed by other scripts
to run t-tests. The `DV` flag outputs data which can be used to calculate
the mean value of the time delay estimate across all functions. Usually, the
means are calculated on a per-function basis, but setting this flag outputs data
in a form which groups data from all the functions for one value of $alpha$
into one set which can then be easily processed as a single set of
estimates. The `EV` flag does a similar thing to the `DV` flag,
but for error data. The error values are grouped by $alpha$ value, and the
resulting files can be used to find the aggregate error for each value of
$alpha$ for a specific method combination. Running the
`extract_results.sh` script will output the data. Next, we will explain
how to process the resulting files.
### Processing Result Data
Inside the results directory, the top level contains files which detail the mean
estimate, standard deviation and mean error for each function for each value of
$alpha$. The `results` directory contains directories with files which
are used to produce different data. The `data` directory contains copies
of all results files, with the filenames showing what experiment the file was
taken from.

To create data for t-tests, we use the files in the `alpha_errors`
directory. With this data we will be able to compare the errors of one
combination of method to another. The `ttest_columnate_agg.sh` and
`ttest_columnate_individual.sh` scripts are used to process the data
further into files readable by the `ttest.m` script. The first script
groups data so that when the t-tests are run, results from all functions for one
value of $alpha$ for one method are compared to the same set of functions for
the same value of $alpha$, but with a different method combination. The second
script processes data so that results for individual functions are compared,
rather than an aggregate set of data. T-test data will be output to a directory
`ttest` in the directory specified in the script. In each file, there
will be columns of data used for the t-test, as well as some information about
where the data was taken from.

Using the `ttest.m` script, we can run t-tests on the data. The script
was written using GNU Octave cite{octave}, but should also be compatible with
Matlab. The `read_start_x`, `read_start_y`,
`read_end_x` and `read_end_y` must be modified to match the
data before the script is run. These values specify the range used by the
`dlmread` command to parse in data from the files. In the case of 4
columns with 25 lines each, the values are set to

    read_start_x=0
    read_start_y=0
    read_end_x=24
    read_end_y=3

When run, the script produces a set of t-tests from the data. The
`paired_tests` matrix contains the results of two-tailed paired t-tests
on the data, and the `single_sample` matrix contains the results of
single sample t-tests on the error values calculated by subtracting one set of
data from the other. The comparisons array indicates which columns were compared
to produce each column of the matrix. In general, 1 refers to the baseline area
method, 2 to the baseline PDF method, 3 to the Gaussian area method, and 4 to
the Gaussian PDF method.

#### Mean and Standard Deviation of Estimates
Using the `multifunc_mean.sh` script, the mean
and standard deviation of estimates from different combinations of methods can
be generated. Setting the `INDIR` variable to point to the
`results/estimates` directory will perform the computations using a short
Octave script, and output the results to a file, which will additionally contain
tables for use in Emacs' `org-mode`. Tables ref{tbl:sine1} and
ref{tbl:sine2} are examples of these tables converted into LaTeX using the
export functionality built into `org-mode`.

#### Error of Estimates
Being able to display the error of combinations of methods, such as the graphs
in Figures ref{fig:prelimerror} and ref{fig:fineerror} is also useful, and
data to do this can be produced by the `multifunc_errmean.sh`
script. The script will produce files for each combination of methods, which can
then be plotted with a program such as gnuplot. One way to plot the data using
gnuplot is

    plot "baseline_area_err.txt" using 1:2:3 with errorbars