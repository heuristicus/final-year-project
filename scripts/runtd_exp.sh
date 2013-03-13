EXP_PFILE=../data/exp_params.txt
INDIR=/media/michal/Edison/fyp/exp_rand_param
OUTDIR=/media/michal/Edison/fyp/exp_rand_td
FOLDER_PREFIX=alpha_
FUNC_PREFIX=f

echo ""
for ALPHA in 04 08 1 2 3; do
    for FUNC in 1 2 3 4 5; do
	for TYPE in baseline gaussian; do
	    echo "alpha=$ALPHA, func=$FUNC, type=$TYPE"
	    # Extract the goodness and experiment number from the aggregate file
	    RES="`cat agg_goodness_$TYPE.txt | grep -A 1 "alpha_$ALPHA func$FUNC" | sed '1d;'`"
	    GD_AR=(${RES// / }) # split the above string on a space to get an array - exp num is at index 1
	    EXP_NUM=${GD_AR[1]}
	    if [ "$TYPE" = "baseline" ]; then # Extract the relevant parameters
		cat $INDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC/$TYPE/experiment_$EXP_NUM/parameters.txt | grep -n base_
	    else
		cat $INDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC/$TYPE/experiment_$EXP_NUM/parameters.txt | grep -n gauss_est_stdev
	    fi
	    
	    grep -n base_ $EXP_PFILE

	    echo -e "\n\n"
	done
		#./deltastream -x $EXP_PFILE -p ../data/params -i $INDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC -o $OUTDIR/$FOLDER_PREFIX$ALPHA/$FUNC_PREFIX$FUNC -c 5 -r
    done
done