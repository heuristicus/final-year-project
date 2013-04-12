# Directory containing time delay experiment data
INDIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_time_delay
# Directory to which results should be output
OUTDIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_results
TOP_NAME=alpha
TOP_PREFIX=alpha_
NUM_FUNCS=25
 # Top level directories - this is usually the alpha parameter
#TOP_NUMS=(001 002 003 004 005 006 007 008 009 010 011 012 013 014 015)
TOP_NUMS=(005 01 015 03 06)
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
TOP_NUM_DIVISOR=100
TT=1 # Set this variable to create files which aggregate data to make t-tests easier.
DV=1 # Output data for use in mean and standard deviation calculation
EV=1 # Output data for use in graphing error for each estimator depending on alpha values

# Create the output directory if it does not exist
if [ ! -d "$OUTDIR" ]; then
    mkdir -p $OUTDIR/results/data
    mkdir -p $OUTDIR/results/estimates
    mkdir -p $OUTDIR/results/alpha_errors
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

# Output estimate delta values into the estimates directory. This data can be used to calculate the mean aggregated over all functions for which the experiments were performed.
if [[ -n "$DV" ]]; then
    for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
	for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	    echo "" > $OUTDIR/results/estimates/$TYPE\_$EST_TYPE.txt
	    for TOP_NUM in ${TOP_NUMS[@]}; do
		
		RES="`grep -A $NUM_FUNCS "Estimate delta" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM.txt | sed '1d'`"
		echo -e "td_est=$EST_TYPE, func_est=$TYPE, alpha=$TOP_NUM\n$RES\n\n" >> $OUTDIR/results/estimates/$TYPE\_$EST_TYPE.txt
	    done
	done
    done
fi

# Output the error data, sorted by alpha values, into an extra directory. This data can be used to find the aggregate error
# for each alpha value for a specific estimator combination.
if [[ -n "$EV" ]]; then
    for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
	for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	    echo "" > $OUTDIR/results/alpha_errors/$TYPE\_$EST_TYPE.txt
	    for TOP_NUM in ${TOP_NUMS[@]}; do
		RES="`grep -A $NUM_FUNCS "Estimate error" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM.txt | sed '1d'`"
		echo -e "td_est=$EST_TYPE, func_est=$TYPE, alpha=$TOP_NUM\n$RES\n\n" >> $OUTDIR/results/alpha_errors/$TYPE\_$EST_TYPE.txt
	    done
	done
    done
fi