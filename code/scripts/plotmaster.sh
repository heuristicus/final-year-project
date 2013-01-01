#!/bin/bash

usage (){
cat <<EOF
usage $0 options

Plot a graph from the files output from the generator or estimators.
You will need to pass in some files to plot them. Run this script with the switch you want to use but no parameters to see what it needs.

OPTIONS:
  -h   show this message
  -s   plot graph for multiple streams
  -l   plot graph with linear estimates
  -L   plot linear estimate graph with extra data
  -e   plot graph with data from the given estimate. Makes no assumptions about the type of the estimate.
  -p   plot piecewise estimate graph
  -P   plot piecewise estimate with extra data
  -b   plot baseline estimate graph
EOF
}

texify (){
    latex -interaction=nonstopmode $1.tex
    dvips -o $1.ps $1.dvi
    ps2pdf $1.ps
    rm $1.tex $1.ps $1.dvi $1.log $1.aux $1-inc.eps
    evince $1.pdf&
}

if [ $# -eq 0 ]; then
    usage
    exit
fi

while getopts "slLpPbeh" opt; do
  case $opt in
    s)
      s=1
      ;;
    l)
      l=1
      ;;
    L)
      L=1
      ;;
    e)
      e=1
      ;;
    b)
      b=1
      ;;
    h)
      usage
      ;;
    \?)
      usage
      ;;
  esac
done

if [ $s ]; then
    if [ $# -ne 4 ]; then
	echo -e "Missing or excess arguments when plotting two streams.\nusage: `basename $0` -s outfile stream_1_data stream_2_data"
	exit
    fi
gnuplot<<EOF
call "../plots/twostreams.plt" "$2" "$3" "$4"
EOF
texify $2
    exit
fi

if [ $l ]; then
    exit
fi

if [ $L ]; then
    exit
fi

if [ $e ]; then
    if [ $# -ne 4 ]; then
	echo -e "Missing or excess arguments when plotting unspecified estimate.\nusage: `basename $0` -e outfile stream_data_all estimator_output"
	exit
    fi
gnuplot <<EOF
call "../plots/undef.plt" "$2" "$3" "$4"
EOF
texify $2
fi

if [ $b ]; then
    if [ $# -ne 4 ]; then
	echo -e "Missing or excess arguments when plotting baseline estimates.\nusage: `basename $0` -b outfile stream_data_all IWLS_output baseline_output"
	exit
    fi
gnuplot << EOF
# $1 is the switch, so skip it $2 is the file to output to, $3 is all data from the generator, $4 is the data produced by the IWLS, $5 is the output from the baseline estimate
call "../plots/blplot.plt" "$2" "$3" "$4" "$5"

EOF
texify $2
    exit
fi