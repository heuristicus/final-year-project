FUNC_EXP_DATA_DIR=/media/michal/Edison/fyp/exp_rand_param

for ALPHA in 04 08 1 2 3; do
    for FUNC in 1 2 3 4 5; do
	for TYPE in baseline gaussian; do
	    echo "#alpha_$ALPHA func$FUNC" >> agg_goodness_$TYPE.txt
	    cat $FUNC_EXP_DATA_DIR/alpha_$ALPHA/f$FUNC/$TYPE/avg_goodness.txt | grep best | sed 's/best: //g;s/from experiment //g' >> agg_goodness_$TYPE.txt
	done
    done
done