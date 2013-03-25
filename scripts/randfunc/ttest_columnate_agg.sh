# Columnate data for t-tests by aggregating functions in such a way that all
# functions estimated by one method are compared. If there are 5 functions with
# 5 pairs of streams each, then 25 numbers will be in each t-test.
INDIR=/media/michal/Edison/fyp/new/random/rand_results/results/alpha_errors
STREAM_NUMS=(2) # Which streams to create t-test data for

# Top level directories - this is usually the alpha parameter
ALPHA=(04 08 1 2 3)
if [ ! -d $INDIR/ttest ]; then
    mkdir $INDIR/ttest
fi

for VAL in ${ALPHA[@]}; do
    OUTFILE=$INDIR/ttest/alpha_$VAL\_ttest.txt
    > $OUTFILE
    RES=""
    NAMES=""
    FILES=`ls $INDIR | grep $VAL`
    for FILE in $FILES; do
	for STREAM in ${STREAM_NUMS[@]}; do
	    COL=$((1+$STREAM))
	    RES="$RES\n`cat $INDIR/$FILE | grep -v '^\s*$' | grep -v 'td' | awk '{print $3}'`"
	    NAMES="$NAMES\n`cat $INDIR/$FILE | grep 'td'`"
	done
    done
    echo -e "$RES" | grep -v '^\s*$' | pr -ts --columns 4 >> $OUTFILE
    echo -e "$NAMES" >> $OUTFILE
done


# # Grab the column for the stream from file, grep out the whitespace, then print 4 columns
# 	    # and delete the top line of the resulting data, which contains unneeded text
# 	    RES=`awk '{print $'$COL'}' $FILE | grep -v '^\s*$' | pr -ts --columns 4 | sed '1d'`
# 	    echo "$RES" > $INDIR/ttest/$(basename $FILE)\_ttest
# 	    echo -e "\n\n`grep td_ $FILE`" >> $INDIR/ttest/$(basename $FILE)\_ttest