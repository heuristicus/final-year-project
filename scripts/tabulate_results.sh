# Directory containing time delay experiment data
INDIR=/media/michal/Edison/fyp/new/sine/preliminary/prelim_results
OUTFILE=/media/michal/Edison/fyp/new/sine/preliminary/prelim_results/orgtables.txt
 # Top level directories - this is usually the alpha parameter
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
LOOP=1

echo "" > $OUTFILE
for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
    echo -e "" > tmp
    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	INFILE=$INDIR/$TYPE\_$EST_TYPE.txt
	# Take alpha, mean and stdev out of the file
	ALPHA=(`awk '{print $1}' $INFILE`)
	MEAN=(`awk '{print $3}' $INFILE`)
	STDEV=(`awk '{print $4}' $INFILE`)
	total=${#ALPHA[@]}
	if [ $LOOP = 1 ]; then
	    echo "|$\\\alpha$|" >> tmp
	    for (( i=1; i<=$(( $total -1 )); i++ )); do
		echo -e "|${ALPHA[$i]}|" >> tmp
	    done
	    echo -e "\n\n" >> tmp
	fi
	echo -e "$TYPE $EST_TYPE|" >> tmp
	for (( i=1; i<=$(( $total -1 )); i++ )); do
	    echo -e "${MEAN[$i]} $\\\pm$ ${STDEV[$i]}|" >> tmp
	done
	echo -e "\n\n" >> tmp
	LOOP=$[LOOP+1]
    done
    A=`grep -v '^\s*$' tmp | pr -ts" " --columns 3`
    echo -e "$A"
    echo -e "$A\n\n" >> $OUTFILE
    LOOP=1
done

