#!/bin/bash

PLOT_DIR=/home/michal/Dropbox/study/university/final_year/final-year-project/plots

usage (){
cat <<EOF
usage $0 options

Plot a graph from the files output from the generator or estimators.
You will need to pass in some files to plot them. Run this script with the switch you want to use but no parameters to see what it needs.

OPTIONS:
  -h   show this message
  -c   plot randomly generated function with gaussian contributions
  -E   plot error data gathered from experiments
  -f   plot generating function, bin data and estimate
  -A   plot two functions with the area in between shaded
  -m   plot generating function, bin data and two different estimates
  -b   plot generating function, combined function and two function estimates
  -o   plot generating function
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

while getopts "hEofmAcbt" opt; do
    case $opt in
	b)
	    b=1
	    ;;
	c)
	    c=1
	    ;;
	E)
	    E=1
	    ;;
        o)
	    o=1
	    ;;
	f)
	    f=1
	    ;;
	h)
	    usage
	    ;;
	m)
	    m=1
	    ;;
	t)
	    t=1
	    ;;
	A)
	    A=1
	    ;;
	\?)
	    usage
	    ;;
    esac
done

if [ $E ]; then
    if [ $# -ne 3 ]; then
	echo -e "Missing or excess arguments when plotting error.\nusage: `basename $0` -E outfile ERROR_DATA"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/error.plt" "$2" "$3"
EOF
texify $2
exit
fi

if [ $o ]; then
    if [ $# -ne 3 ]; then
	echo -e "Missing or excess arguments when plotting error.\nusage: `basename $0` -o outfile FUNC_DATA"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/orig.plt" "$2" "$3"
EOF
texify $2
exit
fi

if [ $f ]; then
    if [ $# -ne 5 ]; then
	echo -e "Missing or excess arguments when plotting function with bin data.\nusage: `basename $0` -f outfile FUNC EST BINS"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/allplot.plt" "$2" "$3" "$4" "$5"
EOF
texify $2
exit
fi

if [ $m ]; then
    if [ $# -ne 6 ]; then
	echo -e "Missing or excess arguments when plotting two estimates.\nusage: `basename $0` -m outfile FUNC EST1 EST2 BINS"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/multiplot.plt" "$2" "$3" "$4" "$5" "$6"
EOF
texify $2
exit
fi

# Note that the input files for this need some special tweaking to plot correctly. 
if [ $A ]; then
    if [ $# -ne 3 ]; then
	echo -e "Missing or excess arguments when plotting area data.\nusage: `basename $0` -A outfile EST_DATA"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/area.plt" "$2" "$3"
EOF
texify $2
exit
fi

if [ $c ]; then
    if [ $# -ne 4 ]; then
	echo -e "Missing or excess arguments when plotting random function.\nusage: `basename $0` -c outfile FUNC_SUM FUNC_CONTRIB"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/contrib.plt" "$2" "$3" "$4"
EOF
texify $2
exit
fi

if [ $b ]; then
    if [ $# -ne 7 ]; then
	echo -e "Missing or excess arguments when plotting shifted estimates.\nusage: `basename $0` -b outfile FUNC_SUM FUNC_COMBINED EST_1 EST_2 DELAY"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/combplot.plt" "$2" "$3" "$4" "$5" "$6" "$7"
EOF
texify $2
exit
fi

if [ $t ]; then
    if [ $# -ne 6 ]; then
	echo -e "Missing or excess arguments when plotting two estimates with bins.\nusage: `basename $0` -t outfile EST_1 BINS_1 EST_2 BINS_2"
	exit
    fi
gnuplot << EOF
call "$PLOT_DIR/estbins.plt" "$2" "$3" "$4" "$5" "$6"
EOF
texify $2
exit
fi