FUNC_EXP_DATA_DIR=/media/michal/Edison/fyp/new/random/rand_params
OUTDIR=/media/michal/Edison/fyp/new/random
AVALS=(04 08 1 2 3)
FVALS=(1 2 3 4 5)

for TYPE in baseline gaussian; do
    echo "" > $OUTDIR/agg_goodness_$TYPE.txt
    for ALPHA in ${AVALS[@]}; do
	for FUNC in ${FVALS[@]}; do
	    echo "#alpha_$ALPHA func$FUNC" >> $OUTDIR/agg_goodness_$TYPE.txt
	    cat $FUNC_EXP_DATA_DIR/alpha_$ALPHA/f$FUNC/$TYPE/avg_goodness.txt | grep best | sed 's/best: //g;s/from experiment //g' >> $OUTDIR/agg_goodness_$TYPE.txt
	done
    done
done