DEF_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
EXP_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/exp_params.txt
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
INDIR=/media/michal/Edison/fyp/new/sine/fine/fine_streams
OUTDIR=/media/michal/Edison/fyp/new/sine/fine/fine_params
FOLDER_PREFIX=alpha_
NUM_STREAMS=10
EXP_NUMS=(001 002 003 004 005 006 007 008 009 010 011 012 013 014 015)
#EXP_NUMS=(005 01 015 03 06)

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

for ALPHA in ${EXP_NUMS[@]}; do
    $LAUNCHER_LOC -x $EXP_PFILE -p $DEF_PFILE -i $INDIR/$FOLDER_PREFIX$ALPHA -o $OUTDIR/$FOLDER_PREFIX$ALPHA -c $NUM_STREAMS
done