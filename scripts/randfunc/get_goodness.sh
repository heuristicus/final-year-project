FUNC_EXP_DATA_DIR=/media/michal/Doppler/fyp/new/morerandom/morerandom_params
OUTDIR=/media/michal/Doppler/fyp/new/morerandom
AVALS=(04 05 06 07 08 09 10 11 12 13 14 15)
APREF=alpha_
FPREF=f
NFUNCS=5
NPAIRS=5
EXP_NAMES=(baseline gaussian)

for TYPE in ${EXP_NAMES[@]}; do
    echo "" > $OUTDIR/agg_goodness_$TYPE.txt
    for ALPHA in ${AVALS[@]}; do
	for FUNC in $(seq 1 $NFUNCS); do
	    echo "#alpha_$ALPHA func$FUNC" >> $OUTDIR/agg_goodness_$TYPE.txt
	    cat $FUNC_EXP_DATA_DIR/$APREF$ALPHA/$FPREF$FUNC/$TYPE/avg_goodness.txt | grep best | sed 's/best: //g;s/from experiment //g' >> $OUTDIR/agg_goodness_$TYPE.txt
	done
    done
done