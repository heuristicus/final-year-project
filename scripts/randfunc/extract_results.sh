INDIR=/media/michal/Doppler/fyp/new/morerandom/morerandom_time_delay
OUTDIR=/media/michal/Doppler/fyp/new/morerandom/morerandom_results
TOP_NUMS=(01 02 03 04 05 06 07 08 09 10 11 12 13 14 15)
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
NUM_FUNCS=5
TOP_NAME=alpha
BOT_NAME=func
TOP_PREFIX=alpha_
BOT_PREFIX=f
TT=1 # Output data for use in t-tests
DV=1 # Output data for use in mean and standard deviation calculation
EV=1 # Output data for use in graphing error for each estimator depending on alpha values

# Create the output directory if it does not exist
if [ ! -d "$OUTDIR" ]; then
    mkdir -p $OUTDIR/results/data
    mkdir -p $OUTDIR/results/errors
    mkdir -p $OUTDIR/results/estimates
    mkdir -p $OUTDIR/results/alpha_errors
fi

for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	OUTFILE=$OUTDIR/$TYPE\_$EST_TYPE.txt # Create a file for this parameter
	echo "" > $OUTFILE
	for TOP_NUM in ${TOP_NUMS[@]}; do # Top level directories - this is usually the alpha parameter
	    echo -e "# $TOP_NAME$TOP_NUM" >> $OUTFILE
	    echo -e "# $BOT_NAME\tActual delay\tMean estimate\tStandard dev\tMean error" >> $OUTFILE
	    for BOT_NUM in $(seq 1 $NUM_FUNCS); do # Subdirectories - different functions experimented with
		#echo "est_type=$EST_TYPE, $TOP_NAME=$TOP_NUM, $BOT_NAME=$BOT_NUM, type=$TYPE" 
		# Extract the first stream estimate information from the input directory, stripping the text
		RES="`grep -A 4 "Stream 1" $INDIR/$EST_TYPE/$TOP_PREFIX$TOP_NUM/$BOT_PREFIX$BOT_NUM/$TYPE/experiment_0/results.txt | sed '1d;s/Actual: //g;s/Mean est: //g;s/Est stdev: //g;s/Mean error: //g' | tr '\n' '\t'`"
#		REAL_ALPHA="`echo $TOP_NUM | sed 's/^[[:digit:]]/0./g'`"
		# Write the results of this experiment into the file
		echo -e "$BOT_NUM\t$RES" >> $OUTFILE
		# Copy the file with the original data into the results folder to make it easier to find
		cp $INDIR/$EST_TYPE/$TOP_PREFIX$TOP_NUM/$BOT_PREFIX$BOT_NUM/$TYPE/experiment_0/results.txt $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt
	    done
	    echo -e "\n\n" >> $OUTFILE
	done

    done
done

# This groups data with the same top-level parameter into the same file for all estimators. Should make things easier to put into t-tests
if [[ -n "$TT" ]]; then
    for BOT_NUM in $(seq 1 $NUM_FUNCS); do # Subdirectories - different functions experimented with
	for TOP_NUM in ${TOP_NUMS[@]}; do
	    echo "" > $OUTDIR/results/errors/$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt
	    for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
		for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
		    RES="`grep -A $NUM_FUNCS "Estimate errors" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt | sed '1d'`"
		    echo -e "td_est=$EST_TYPE, func_est=$TYPE, $BOT_NAME=$BOT_NUM\n$RES\n\n" >> $OUTDIR/results/errors/$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt
		done
	    done
	done
    done
fi

# Output estimate delta values into the estimates directory. This data can be used to calculate the mean aggregated over all functions for which the experiments were performed.
if [[ -n "$DV" ]]; then
    for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
	for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	    echo "" > $OUTDIR/results/estimates/$TOP_PREFIX$TOP_NUM\_$TYPE\_$EST_TYPE.txt
	    for BOT_NUM in $(seq 1 $NUM_FUNCS); do # Subdirectories - different functions experimented with
		for TOP_NUM in ${TOP_NUMS[@]}; do
		    RES="`grep -A $NUM_FUNCS "Estimate delta" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt | sed '1d'`"
		    echo -e "td_est=$EST_TYPE, func_est=$TYPE, $BOT_NAME=$BOT_NUM\n$RES\n\n" >> $OUTDIR/results/estimates/$TOP_PREFIX$TOP_NUM\_$TYPE\_$EST_TYPE.txt
		done
	    done
	done
    done
fi

# Output the error data, sorted by alpha values, into an extra directory. This data can be used to find the aggregate error
# for each alpha value for a specific estimator combination.
if [[ -n "$EV" ]]; then
    for TOP_NUM in ${TOP_NUMS[@]}; do
	for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
	    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
		echo "" > $OUTDIR/results/alpha_errors/$TOP_PREFIX$TOP_NUM\_$TYPE\_$EST_TYPE.txt
		for BOT_NUM in $(seq 1 $NUM_FUNCS); do # Subdirectories - different functions experimented with
		    RES="`grep -A $NUM_FUNCS "Estimate error" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt | sed '1d'`"
		    echo -e "td_est=$EST_TYPE, func_est=$TYPE, $BOT_NAME=$BOT_NUM\n$RES\n\n" >> $OUTDIR/results/alpha_errors/$TOP_PREFIX$TOP_NUM\_$TYPE\_$EST_TYPE.txt
		done
	    done
	done
    done
fi