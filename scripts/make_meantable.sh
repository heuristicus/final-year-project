INDIR=/media/michal/Edison/fyp/new/random/rand_results
OUTFILE=$INDIR/meanvals.dat
# Number of columns to use when calculating averages for alpha values. 
NCOLS=5

echo "" > $OUTFILE
for COL in $(seq 1 $NCOLS); do
    echo "Processing column $COL..."
    echo -e "||Baseline|Gaussian|\n|--+--+--+--|\n|/|<||" >> $OUTFILE
    HOLD=""
    for FILE in $INDIR/*.txt; do
        # Get the column of the mean values of the estimates and put it into one column for each alpha value
	COLS=`awk '{print $4}' $FILE | grep -v '^\s*$' | pr -ts --columns $NCOLS | sed '1d'`

        # Extract the column in question
	C=`echo "$COLS" | awk '{print $'$COL'}'`
        # Create an octave script to calculate mean and stdev
        # and save the values to the file stdata
	OSCRIPT="a = [$C];
             b = [mean(a) std(a)];
             c = mat2str(b,5);
             d = eval(c);
             save('stdata', 'd')"
	echo $OSCRIPT | octave -q
        # octave outputs a header, so grep only the relevant data and strip leading spaces
	DATA="`cat stdata | grep -v '#' | sed 's/^ *//g;s/ / $\\\pm$ /'`|"
	HOLD="`echo "$HOLD $DATA\n"`"
    done
    # should only be 2 columns, but need 3 for some reason?
    RES="`echo -e "$HOLD" | pr -ts --columns 3`"
    RES="`echo -e "$RES" | sed -e 's/^/|Area|/; 2s/Area/PDF/'`"
    echo -e "$RES\n" >> $OUTFILE
done

echo "Order of alpha values" >> $OUTFILE
echo "`grep "alpha" $FILE`" >> $OUTFILE

# remove the octave output file
rm stdata
