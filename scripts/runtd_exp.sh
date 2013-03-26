EXP_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/exp_params.txt
DEF_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
INDIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_params
STREAM_DIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_streams
GOODNESS_LOC=/media/michal/Edison/fyp/new/sine/preliminary
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
OUTDIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_time_delay
FOLDER_PREFIX=alpha_
BESTFILE=$OUTDIR/best_params.txt
NSTREAMS=2
NFUNCS=25

ALPHA_VALUES=(005 01 015 03 06)

while true; do
    echo -e "Running this script will edit some parameters in the files $EXP_PFILE and $DEF_PFILE.\nResults will be output to $OUTDIR, stream data will be read from $STREAM_DIR,\nbest parameter information will be read from $INDIR.\nAre you sure you want to run it?"
    read yn
    case $yn in
	[Yy]* ) break;;
	[Nn]* ) exit;;
	* ) echo "Please enter Y or N.";;
    esac
done

# Create the output directory if it does not exist
if [ ! -d "$OUTDIR" ]; then
    mkdir $OUTDIR
fi

echo "" > $BESTFILE

# Get the names of each experiment from the experiment paramfile so we can check that they
# are set up to estimate the time delay
NPARAM="`grep experiment\_names $EXP_PFILE`"
PSTR=(${NPARAM// / })
ENAMES=(${PSTR[1]//,/ })

for V in ${ENAMES[@]}; do
    echo $V
    # Change the experiment type to do experiments on the delay and not just functions.
    sed -i "s/$V\_type [a-zA-Z]*/$V\_type delay/" $EXP_PFILE
done

# Time delay experiments are not to be run stuttered, so change that value too.
sed -i 's/run_stuttered [a-zA-Z]*/run_stuttered no/' $EXP_PFILE

LOOP=1
for TD_EST_TYPE in pmf area; do
    if [ ! -d "$OUTDIR/$TD_EST_TYPE" ]; then
	mkdir $OUTDIR/$TD_EST_TYPE
    fi
    # Set the delta estimation method to the estimator type that we are using.
    sed -i "s/delta_est_method [a-zA-Z]*/delta_est_method $TD_EST_TYPE/" $DEF_PFILE
    for ALPHA in ${ALPHA_VALUES[@]}; do
	if [ ! -d "$OUTDIR/$TD_EST_TYPE/$FOLDER_PREFIX$ALPHA" ]; then
	    mkdir $OUTDIR/$TD_EST_TYPE/$FOLDER_PREFIX$ALPHA
	fi
	for TYPE in ${ENAMES[@]}; do
	    if [ ! -d "$OUTDIR/$TD_EST_TYPE/$FOLDER_PREFIX$ALPHA/$TYPE" ]; then
		mkdir $OUTDIR/$TD_EST_TYPE/$FOLDER_PREFIX$ALPHA/$TYPE
	    fi
	    if [ $LOOP = 1 ]; then
		echo "alpha=$ALPHA, type=$TYPE, td_type=$TD_EST_TYPE" >> $BESTFILE
	    fi
	        # Extract the goodness and experiment number from the aggregate file
	    RES="`cat $GOODNESS_LOC/agg_goodness_$TYPE.txt | grep -A 1 "alpha_$ALPHA" | sed '1d;'`"
	    GOODNESS_ARR=(${RES// / }) # split the above string on a space to get an array - exp num is at index 1
	    EXP_NUM=${GOODNESS_ARR[1]}
	        # Grep the parameters for the experiment type out of the experiment file
	    T_PARAM=`grep $TYPE\_params $EXP_PFILE`
	        # Split the whole parameter string on a space - there is one separating the parameter from the values
	    PARAM_STRING=(${T_PARAM// / })
	        # Split the first array location on commas, which are used to separate parameters
	    VALUES=(${PARAM_STRING[1]//,/ })
	        # Loop over each parameter name that is in the experiment
	    for VALUE in ${VALUES[@]}; do
		    # Get the value of the parameter in the best experiment for this alpha, func and type
		BEST_P=`grep $VALUE $INDIR/$FOLDER_PREFIX$ALPHA/$TYPE/experiment_$EXP_NUM/parameters.txt`
		if [ $LOOP = 1 ]; then
		    echo $BEST_P >> $BESTFILE
		fi
		    # Get the point in the experiment file at which the parameter declaration for the current parameter is found.
		EXP_LINE=`grep -n "$VALUE " $EXP_PFILE`
		    # Split to get line number in index 0
		EXP_L_SPLIT=(${EXP_LINE//:/ })
		EXP_LINE=${EXP_L_SPLIT[0]}
		    # Replace the relevant line in the experiment parameter file to match the best parameter for this set
		sed -i "$EXP_LINE s/.*/$BEST_P/" $EXP_PFILE
	    done
	    if [ $LOOP = 1 ]; then
		echo -e "\n" >> $BESTFILE
	    fi
	    
	    # At this point, we have an experiment parameter file modified to match the best parameters in the baseline and gaussian
	    # experiments on stuttered streams. We can now run the experiments with the best parameters.
	    $LAUNCHER_LOC -x $EXP_PFILE -p $DEF_PFILE -i $STREAM_DIR/$FOLDER_PREFIX$ALPHA -o $OUTDIR/$TD_EST_TYPE/$FOLDER_PREFIX$ALPHA -c $NFUNCS -n $NSTREAMS -t 3
	done
    done
    LOOP=$[LOOP+1]
done