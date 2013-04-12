FUNC_EXP_DATA_DIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_params
OUTDIR=/media/michal/Edison/fyp/new/sine/preliminary/

#TOP_VALS=(001 002 003 004 005 006 007 008 009 010 011 012 013 014 015)
TOP_VALS=(005 01 015 03 06)

for TYPE in baseline gaussian; do
    echo "" > $OUTDIR/agg_goodness_$TYPE.txt
    for ALPHA in ${TOP_VALS[@]}; do
	    echo "#alpha_$ALPHA" >> $OUTDIR/agg_goodness_$TYPE.txt
	    cat $FUNC_EXP_DATA_DIR/alpha_$ALPHA/$TYPE/avg_goodness.txt | grep best | sed 's/best: //g;s/from experiment //g' >> $OUTDIR/agg_goodness_$TYPE.txt
    done
done