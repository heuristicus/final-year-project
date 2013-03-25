# dirname="/media/michal/Edison/fyp/new/sine/fine/fine_results/results/ttest"
# read_start_x=0
# read_start_y=0
# read_end_x=9
# read_end_y=3

# dirname="/media/michal/Edison/fyp/new/sine/preliminary/prelim_results/results/alpha_errors/ttest"
# # Define the points from which the dlmread command reads data. Specifies a rectangle over the data.
# read_start_x=0
# read_start_y=0
# read_end_x=4
# read_end_y=3

# Define where the t-test data is to be read from
dirname="/media/michal/Edison/fyp/new/random/rand_results/results/alpha_errors/ttest"
# Define the points from which the dlmread command reads data. Specifies a rectangle over the data.
read_start_x=0
read_start_y=0
read_end_x=24
read_end_y=3
# separator for the dlmread command
separator="\t"

cd(dirname)
format short G

# Read all the filenames from the given directory
files=dir(fullfile(dirname, '*'))
sigmat=[] # Used to store the results of the paired t-tests
errmat=[] # Used to store the results of single t-tests on the error difference
# Loop over all file names
for ind = 1:length(files)
  # Extract the data from the current file
  
  data = dlmread(files(ind).name, separator, [read_start_x,read_start_y,read_end_x,read_end_y])
  # There are four possible methods which we can work with.
  for i = 1:4
    # There are four possible methods to which we can compare the first method
    for j = i:4
      # There is no point comparing the method to itself
      if j==i
	continue
      endif
      first(end+1)=i
      comp(end+1)=j
      # Contains the first set of error data from the file, for the ith method
      d1=data(:,i)
      # Contains the second set of error data from the file, for the jth method
      d2=data(:,j)
      # Subtract one vector from the other to get the error differences
      d3=d1-d2
      # Do a paired t-test to compare the ith method to the jth
      pvalSig(end+1)=t_test_2(d1, d2)
      # Do a single valued t-test on the error difference
      pvalErrorDiff(end+1)=t_test(d3,0)
    endfor
  endfor
  # Append the significance values to the result matrix
  sigmat = [sigmat;pvalSig]
  # Append the error diff values to the result matrix
  errmat = [errmat;pvalErrorDiff]

  # Information about which i and j values were compared in which column
  # of data
  comps=[first;comp]
  # Clear the variables so that they can be re-used in the next loop
  files.name
  clear pvalSig
  clear pvalErrorDiff
  clear first
  clear comp
endfor

