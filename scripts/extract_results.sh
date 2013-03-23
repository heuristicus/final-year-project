# Directory containing time delay experiment data
INDIR=/media/michal/Edison/fyp/new/sine/fine/fine_time_delay
# Directory to which results should be output
OUTDIR=/media/michal/Edison/fyp/new/sine/fine/fine_results
TOP_NAME=alpha
TOP_PREFIX=alpha_
NUM_FUNCS=25
 # Top level directories - this is usually the alpha parameter
TOP_NUMS=(001 002 003 004 005 006 007 008 009 010 011 012 013 014 015)
#TOP_NUMS=(005 01 015 03 06)
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
TOP_NUM_DIVISOR=100
TT=1 # Set this variable to create files which aggregate data to make t-tests easier.

# Create the output directory if it does not exist
if [ ! -d "$OUTDIR" ]; then
    mkdir -p $OUTDIR/results/data
fi

for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	echo "" > $OUTDIR/$TYPE\_$EST_TYPE.txt # Create a file for this parameter
	echo -e "# $TOP_NAME\tActual delay\tMean estimate\tStandard dev\tMean error" >> $OUTDIR/$TYPE\_$EST_TYPE.txt
	for TOP_NUM in ${TOP_NUMS[@]}; do
	    #echo "est_type=$EST_TYPE, $TOP_NAME=$TOP_NUM, type=$TYPE" 
	    # Extract the first stream estimate information from the input directory, stripping the text
	    RES="`grep -A 4 "Stream 1" $INDIR/$EST_TYPE/$TOP_PREFIX$TOP_NUM/$TYPE/experiment_0/results.txt | sed '1d;s/Actual: //g;s/Mean est: //g;s/Est stdev: //g;s/Mean error: //g' | tr '\n' '\t'`"
	    # Write the results of this experiment into the file
	    # Folders are named 005, 01, etc, which are the values after the decimal point, so we put a point in front.
	    RR=0.$TOP_NUM
	    echo -e "$RR\t$RES" >> $OUTDIR/$TYPE\_$EST_TYPE.txt
#	    echo -e "\n\n"
	    # Copy the data out of the experiment directories and into a results directory for easier access.
	    cp $INDIR/$EST_TYPE/$TOP_PREFIX$TOP_NUM/$TYPE/experiment_0/results.txt $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM.txt
	done
    done
done

# This groups data with the same top-level parameter into the same file for all estimators. Should make things easier to put into t-tests
if [[ -n "$TT" ]]; then
    for TOP_NUM in ${TOP_NUMS[@]}; do
	echo "" > $OUTDIR/results/$TOP_PREFIX$TOP_NUM.txt
	for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
	    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
		RES="`grep -A $NUM_FUNCS "Estimate errors" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM.txt | sed '1d'`"
		echo -e "td_est=$EST_TYPE, func_est=$TYPE\n$RES\n\n" >> $OUTDIR/results/$TOP_PREFIX$TOP_NUM.txt
	    done
	done
    done
fi