reset

set terminal epslatex size 5.0,3.5 standalone color colortext header "\\usepackage{libertine}\n"
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#0000FF' linetype 1 linewidth 3 # blue
set style line 3 linecolor rgb '#A0A0FF' linetype 1 linewidth 3 # ltblue
set style line 4 linecolor rgb '#00FF00' linetype 1 linewidth 1 # green

# Axis labels
set xlabel '$$t$$'
set ylabel '$$\lambda$$'

set yrange [-5:50]

set key bmargin
set key horizontal

plot "$1" index 0 using 1:2 with lines linestyle 1 title 'Generating function',\
     "$2" using 1:2 with lines linestyle 2 title 'Baseline estimate',\
     "$3" using 1:2 with lines linestyle 3 title 'Piecewise estimate',\
     "$4" using 1:2 with points linestyle 4 title 'Bin counts'