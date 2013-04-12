DEF_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
EXP_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/exp_params.txt
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
INDIR=/media/michal/Edison/fyp/new/morerandom/morerandom_streams

AVALS=(01 02 03 04 05 06 07 08 09 10 11 12 13 14 15)
NFUNCS=5
NPAIRS=5
APREF=alpha_
FPREF=f

for ALPHA in ${AVALS[@]}; do
    for VAL in $(seq 1 $NFUNCS); do
	$LAUNCHER_LOC -x $EXP_PFILE -p $DEF_PFILE -i $INDIR/$APREF$ALPHA/$FPREF$VAL -c $NPAIRS -n 2 -s -r
    done
done