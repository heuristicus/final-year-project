OUTPUT_DIR=/media/michal/Edison/fyp/new/morerandom/morerandom_streams
LAUNCHER_LOC=/home/michal/Dropbox/study/university/final_year/final-year-project/src/deltastream
PARAM_FILE=/home/michal/Dropbox/study/university/final_year/final-year-project/src/params.txt

AVALS=(01 02 03 04 05 06 07 08 09 10 11 12 13 14 15)
DIVISOR=10
NFUNCS=5
NPAIRS=5
FPREF=f
APREF=alpha_

for ALPHA in ${AVALS[@]}; do
    # Set the value of alpha in the parameter file
    NUM=$(echo "scale=2; $((10#$ALPHA))/$DIVISOR" | bc)
    sed -i "s/stdev_alpha [0-9\.]*/stdev_alpha $NUM/" $PARAM_FILE
    for FNUM in $(seq 1 $NFUNCS); do
	DNAME=$OUTPUT_DIR/$APREF$ALPHA/$FPREF$FNUM
	if [ ! -d $DNAME ]; then
	    echo "Directory $DNAME does not exist. Creating..."
	    mkdir -p $DNAME
	fi
	cd $DNAME
	
	$LAUNCHER_LOC -g $PARAM_FILE -f rand -c $NPAIRS -n 2 -t 3 -u
	# Sleep to prevent same seeds being used
	sleep 1
    done
done