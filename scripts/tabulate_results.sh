# Makes tables for each of the method combinations. Should be used with data produced from
# the default function generator.
INDIR=/media/michal/Doppler/fyp/new/morerandom/morerandom_results # Directory containing time delay experiment data
OUTFILE=/media/michal/Doppler/fyp/new/morerandom/morerandom_results/orgtables.txt
 # Top level directories - this is usually the alpha parameter
TD_EST_TYPES=(area pmf)
FUNC_EST_TYPES=(baseline gaussian)
LOOP=1

echo "" > $OUTFILE
for EST_TYPE in ${TD_EST_TYPES[@]}; do # Two types of time delay estimators
    echo -e "" > tmp
    for TYPE in ${FUNC_EST_TYPES[@]}; do # Two types of function estimators
	INFILE=$INDIR/$TYPE\_$EST_TYPE.txt
	echo $INFILE
	# Take alpha, mean and stdev out of the file
	ALPHA=(`awk '{print $1}' $INFILE`)
	MEAN=(`awk '{print $3}' $INFILE`)
	STDEV=(`awk '{print $4}' $INFILE`)
	echo $ALPHA
	total=${#ALPHA[@]}
	# On the first loop, print the alpha column
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
#    echo -e "$A"
    echo -e "$A\n\n" >> $OUTFILE
    LOOP=1
done

