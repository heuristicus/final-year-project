# Columnate data for t-tests on a per-function basis. The data is organised such
# that for each function, it is compared to the same function number estimated
# by different methods. With this, the significance for each separate
# function can be seen 
INDIR=/media/michal/Edison/fyp/new/random/rand_results/results/alpha_errors
STREAM_NUMS=(2) # Which streams to create t-test data for
# Top level directories - this is usually the alpha parameter

if [ ! -d $INDIR/ttest ]; then
    mkdir $INDIR/ttest
fi

for FILE in $INDIR/*.txt; do
    if [ ! -d $FILE ]; then
	echo $FILE
	for STREAM in ${STREAM_NUMS[@]}; do
	    COL=$((1+$STREAM))
	    # Grab the column for the stream from file, grep out the whitespace, then print 4 columns
	    # and delete the top line of the resulting data, which contains unneeded text
	    RES=`awk '{print $'$COL'}' $FILE | grep -v '^\s*$' | pr -ts --columns 4 | sed '1d'`
	    echo "$RES" > $INDIR/ttest/$(basename $FILE)\_ttest
	    echo -e "\n\n`grep td_ $FILE`" >> $INDIR/ttest/$(basename $FILE)\_ttest
	done
    fi
done