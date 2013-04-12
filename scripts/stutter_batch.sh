DEF_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt
EXP_PFILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/exp_params.txt
INDIR=fine_streams
FOLDER_PREFIX=alpha_

for ALPHA in 001 002 003 004 005 006 007 008 009 010 011 012 013 014 015; do
    ./deltastream -x $EXP_PFILE -p $DEF_PFILE -i $INDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC -c 10 -n 2 -s
done