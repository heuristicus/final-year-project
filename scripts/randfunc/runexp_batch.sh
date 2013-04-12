DEF_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
EXP_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/exp_params.txt
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
INDIR=/media/michal/Edison/fyp/new/morerandom/morerandom_streams
OUTDIR=/media/michal/Doppler/fyp/new/morerandom/morerandom_params

AVALS=(04 05 06 07 08 09 10 11 12 13 14 15)
APREF=alpha_
FPREF=f
NFUNCS=5
NPAIRS=5

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

for ALPHA in ${AVALS[@]}; do
    for FUNC in $(seq 1 $NFUNCS); do
	if [ ! -d $OUTDIR/$APREF$ALPHA/$FPREF$FUNC ]; then
	    mkdir -p $OUTDIR/$APREF$ALPHA/$FPREF$FUNC
	fi

	$LAUNCHER_LOC -x $EXP_PFILE -p $DEF_PFILE -i $INDIR/$APREF$ALPHA/$FPREF$FUNC -o $OUTDIR/$APREF$ALPHA/$FPREF$FUNC -c $NFUNCS -r
    done
done