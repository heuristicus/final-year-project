# Calculates the mean and standard deviation for different methods using the same alpha
# value across multiple functions, and puts the resulting data into an org table
INDIR=/media/michal/Edison/fyp/new/random/rand_results/results/estimates
OUTFILE=$INDIR/meanvals.dat
# Number of columns to use when calculating averages for alpha values. This is equal to the number of alpha values tested
NCOLS=5

# Empty the output file
> $OUTFILE
for FILE in $INDIR/*.txt; do
    echo `basename $FILE` >> $OUTFILE
    # Get the column of the mean values of the estimates and put it into one column for each alpha value
    RES="`cat $FILE | grep -v 'td' | awk '{print $3}' | grep -v '^\s*$'`"
    # Extract the column in question
    # Create an octave script to calculate mean and stdev
    # and save the values to the file stdata
    OSCRIPT="a = [$RES];
             b = [mean(a) std(a)];
             c = mat2str(b,5);
             d = eval(c);
             save('$INDIR/stdata', 'd')"
    echo $OSCRIPT | octave -q
    SC="`cat $INDIR/stdata | grep -v '#'`"
    echo $SC >> $OUTFILE
    DATA="`echo -e "$SC" | sed 's/^ *//g;s/ /$\\\pm$/'`|"
    HOLD="$HOLD $DATA\n"
done

rm $INDIR/stdata

echo -e "\n" >> $OUTFILE

# Make the data into columns for each value of alpha
COLUMNATED="`echo -e "$HOLD" | grep -v '^\s*$' | pr -ts --columns $NCOLS`"
# Loop through each column in turn
for COL in $(seq 1 $NCOLS); do
    # Echo the top line to the output file
    echo -e "||Baseline|Gaussian|\n|--+--+--+--|\n|/|<||" >> $OUTFILE
    # Put the data into two columns to separate the baseline and gaussian methods
    TD="`echo -e "$COLUMNATED" | awk '{print $'$COL'}' | pr -ts' ' --columns 2`"
    # Prefix each line with the correct time delay estimation method
    RES="`echo -e "$TD" | sed -e 's/^/|Area|/; 2s/Area/PDF/; s/\\\$\\\pm\\\$/ $\\\pm$ /g'`"
    echo -e "$RES" >> $OUTFILE
done