for TYPE in pmf; do
    for ALPHA in 005 01 015 03 06; do
	for TD in 10 25; do
	    echo -e "\n\nUpdate parameters for alpha=$ALPHA, time delay=$TD, type = $TYPE"
	    read RESP
	    ./deltastream -x ../data/exp_params.txt -p ../data/params.txt -n 2 -c 25 -i sin_streams/alpha_$ALPHA/td_$TD -o sin_td/$TYPE/alpha_$ALPHA/td_$TD/
	done
    done
done