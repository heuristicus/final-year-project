DEF_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
EXP_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/exp_params.txt
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
INDIR=/media/michal/Edison/fyp/new/random/rand_streams
OUTDIR=/media/michal/Doppler/fyp/new/rand_param
FOLDER_PREFIX=alpha_
FUNC_PREFIX=f
NUM_STREAMS=5

# We do model selection on stuttered streams in these experiments.
sed -i 's/run_stuttered [a-zA-Z]*/run_stuttered yes/' $EXP_PFILE

# Get the names of each experiment from the experiment paramfile so we can check that they
# are set up to estimate the functions
NPARAM="`grep experiment\_names $EXP_PFILE`"
PSTR=(${NPARAM// / })
ENAMES=(${PSTR[1]//,/ })

for V in ${ENAMES[@]}; do
    # Change the experiment type to do experiments on the delay and not just functions.
    sed -i "s/$V\_type [a-zA-Z]*/$V\_type function/" $EXP_PFILE
done

for ALPHA in 3; do
    for FUNC in 5; do
	if [ ! -d $OUTDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC ]; then
	    mkdir -p $OUTDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC
	fi
	    
	$LAUNCHER_LOC -x $EXP_PFILE -p $DEF_PFILE -i $INDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC -o $OUTDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC -c $NUM_STREAMS -r
    done
done