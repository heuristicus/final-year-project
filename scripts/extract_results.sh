EXP_PFILE=../data/exp_params.txt
INDIR=sin_td
FOLDER_PREFIX=alpha_
FUNC_PREFIX=td_

for EST_TYPE in area pmf; do
    for ALPHA in 005 01 015 03 06; do
	for TD in 10; do
	    for TYPE in gaussian; do
		echo "est_type=$EST_TYPE, alpha=$ALPHA, delay=$TD, type=$TYPE"
		RES="`grep -A 4 "Stream 1" $INDIR/$EST_TYPE/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$TD/$TYPE/experiment_0/results.txt | sed '1d;s/Actual: //g;s/Mean est: //g;s/Est stdev: //g;s/Mean error: //g'`"
		echo $RES
		echo -e "\n\n"
	    done
	done
    done
done