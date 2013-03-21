INDIR=/media/michal/Edison/fyp/new/random/rand_results/results
STREAM_NUMS=(2) # Which streams to create t-test data for
# Top level directories - this is usually the alpha parameter

if [ ! -d $INDIR/ttest ]; then
    mkdir $INDIR/ttest
fi

for FILE in $INDIR/*; do
    if [ ! -d $FILE ]; then
	echo $FILE
	for STREAM in ${STREAM_NUMS[@]}; do
	    COL=$((1+$STREAM))
	    RES=`awk '{print $'$COL'}' $FILE | grep -v '^\s*$' | pr -tw50 --columns 4 | sed 's/ \+ /\t/g'`
	    echo "$RES" > $INDIR/ttest/$(basename $FILE)\_ttest
	    echo -e "\n\n`grep td_ $FILE`" >> $INDIR/ttest/$(basename $FILE)\_ttest
	done
    fi
done