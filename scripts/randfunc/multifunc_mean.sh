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

#rm $INDIR/stdata

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

echo -e "\n" >> $OUTFILE

# Output each combination of methods into one large org table for all alpha values
# This is quite non-specific. Might need to change quite a few things for it to work...
# Four possible combinations
for A in {0..3}; do
    NAME="`sed -n -e $(($A*2 + 1))p $OUTFILE | sed 's/alpha_[0-9]*_\([A-Za-z]*\)_\([A-Za-z]*\)\.txt/\1 \2/'`"
    echo -e "|$\\\alpha$|$NAME|\n|-+-+|" >> $OUTFILE
    # there are $NCOLS values for each combination
    for COL in $(seq 0 $(($NCOLS-1))); do
	# column number * num combinations * 2 lines for each value + line
	# numbers start at 1 + shift by combination number * 2 lines for each
	# value + get to the actual number
	ALPHA="`sed -n -e $(($COL*4*2+1+$A*2))p $OUTFILE | sed 's/alpha_\([0-9]*\).*/\1/'`"
	RA="`echo "scale=1; $((10#$ALPHA))/10" | bc`"
	
	VALS="`sed -n -e $(($COL*4*2+1+$A*2+1))p $OUTFILE | sed 's/ / $\\\pm$ /g'`"
	echo "|$RA|$VALS|" >> $OUTFILE
    done
    echo -e "\n" >> $OUTFILE
done

echo "Data output to $OUTFILE"