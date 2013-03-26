# Calculates the mean and standard deviation for different methods using the same alpha
# value across multiple functions, and puts the resulting data into an org table
INDIR=/media/michal/Doppler/fyp/new/morerandom/morerandom_results/results/alpha_errors
OUTFILE=$INDIR/errmeans.dat
AVALS=(01 02 03 04 05 06 07 08 09 10 11 12 13 14 15)
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
# Number of columns to use when calculating averages for alpha values. This is equal to the number of alpha values tested
NCOLS=15
DIVISOR=10

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
done

rm $INDIR/stdata
echo -e "\n" >> $OUTFILE

# Put the computations from the same alpha values into a single set of data so
# it can be plotted
for TD in ${TD_EST_TYPES[@]}; do
    for FUNC in ${FUNC_EST_TYPES[@]}; do
	SOUT=$INDIR/$FUNC\_$TD\_err.dat
	> $SOUT
	echo -e "# alpha\tMean\t stdev" >> $SOUT
	for VAL in ${AVALS[@]}; do
	    RVAL="`echo "scale=1; $((10#$VAL))/$DIVISOR" | bc`"
	    RES="`grep -A 1 "alpha_$VAL\_$FUNC\_$TD" $OUTFILE | sed '1d'`"
	    echo -e "$RVAL $RES" >> $SOUT
	done
    done
done