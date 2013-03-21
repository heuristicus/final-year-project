INDIR=/media/michal/Edison/fyp/new/random/rand_time_delay
OUTDIR=/media/michal/Edison/fyp/new/random/rand_results
TOP_NUMS=(04 08 1 2 3)
BOT_NUMS=(1 2 3 4 5)
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
NUM_FUNCS=5
TOP_NAME=alpha
BOT_NAME=func
TOP_PREFIX=alpha_
BOT_PREFIX=f
TT=1

# Create the output directory if it does not exist
if [ ! -d "$OUTDIR" ]; then
    mkdir -p $OUTDIR/results/data
fi

for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	for TOP_NUM in ${TOP_NUMS[@]}; do # Top level directories - this is usually the alpha parameter
	    echo "" > $OUTDIR/$TYPE\_$EST_TYPE\_$TOP_NAME\_$TOP_NUM.txt # Create a file for this parameter
	    echo -e "# $BOT_NAME\tActual delay\tMean estimate\tStandard dev\tMean error" >> $OUTDIR/$TYPE\_$EST_TYPE\_$TOP_NAME\_$TOP_NUM.txt
	    for BOT_NUM in ${BOT_NUMS[@]}; do # Subdirectories - different functions experimented with
		#echo "est_type=$EST_TYPE, $TOP_NAME=$TOP_NUM, $BOT_NAME=$BOT_NUM, type=$TYPE" 
		# Extract the first stream estimate information from the input directory, stripping the text
		RES="`grep -A 4 "Stream 1" $INDIR/$EST_TYPE/$TOP_PREFIX$TOP_NUM/$BOT_PREFIX$BOT_NUM/$TYPE/experiment_0/results.txt | sed '1d;s/Actual: //g;s/Mean est: //g;s/Est stdev: //g;s/Mean error: //g' | tr '\n' '\t'`"
#		REAL_ALPHA="`echo $TOP_NUM | sed 's/^[[:digit:]]/0./g'`"
		# Write the results of this experiment into the file

		echo -e "$BOT_NUM\t$RES" >> $OUTDIR/$TYPE\_$EST_TYPE\_$TOP_NAME\_$TOP_NUM.txt
		cp $INDIR/$EST_TYPE/$TOP_PREFIX$TOP_NUM/$BOT_PREFIX$BOT_NUM/$TYPE/experiment_0/results.txt $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt
		#echo -e "\n\n"
	    done
	done
    done
done

# This groups data with the same top-level parameter into the same file for all estimators. Should make things easier to put into t-tests
if [[ -n "$TT" ]]; then
    for BOT_NUM in ${BOT_NUMS[@]}; do # Subdirectories - different functions experimented with
	for TOP_NUM in ${TOP_NUMS[@]}; do
	    echo "" > $OUTDIR/results/$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt
	    for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
		for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators

		    RES="`grep -A $NUM_FUNCS "Estimate errors" $OUTDIR/results/data/$TYPE\_$EST_TYPE\_$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt | sed '1d'`"
		    echo -e "td_est=$EST_TYPE, func_est=$TYPE, $BOT_NAME=$BOT_NUM\n$RES\n\n" >> $OUTDIR/results/$TOP_PREFIX$TOP_NUM\_$BOT_PREFIX$BOT_NUM.txt
		done
	    done
	done
    done
fi