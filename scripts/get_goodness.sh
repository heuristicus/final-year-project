FUNC_EXP_DATA_DIR=/media/michal/Edison/fyp/sine/fine/exp_fine_param

for TYPE in baseline gaussian; do
    echo "" > agg_goodness_$TYPE.txt
    for ALPHA in 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015; do
	    echo "#alpha_$ALPHA" >> agg_goodness_$TYPE.txt
	    cat $FUNC_EXP_DATA_DIR/alpha_$ALPHA/$TYPE/avg_goodness.txt | grep best | sed 's/best: //g;s/from experiment //g' >> agg_goodness_$TYPE.txt
    done
done