DEF_PFILE=../data/params.txt
EXP_PFILE=../data/exp_params.txt
INDIR=rand_streams
FOLDER_PREFIX=alpha_
FUNC_PREFIX=f

for ALPHA in 04 08 1 2 3; do
    for FUNC in 1 2 3 4 5; do
	./deltastream -x $EXP_PFILE -p $DEF_PFILE -i $INDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC -c 5 -n 2 -r -s
    done
done