set terminal epslatex size 5.0,3.5 standalone header "\\usepackage{libertine}\n" color colortext
set output "$0.tex"

# Line styles
set border linewidth 2

set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#00FF00' linetype 1 linewidth 3 # green

set xlabel '$$t$$'
set ylabel '$$\lambda$$'

set xrange [0:100]
set yrange [0:45]
set key bmargin horizontal

set ytics 10

plot "$1" using 1:2 with lines ls 1 title "Stream 1 Estimate",\
     "$1" using 1:3 with lines ls 2 title "Stream 2 Estimate",\
     "$1" using 1:2:3 with filledcurves lc rgb "gray" title "Inter-function area"