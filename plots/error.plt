reset

set terminal epslatex size 5.0,3.5 standalone header "\\usepackage{libertine}\n"
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red

# Tics in latex format
#set format '%g'

# Key
set key right

# Axis labels
set xlabel '$$\alpha$$'
set ylabel 'Error'

#set xrange [0:0.7]
#set yrange [-60:30]


plot "$1" using 1:5:4 with errorbars title "Mean error" ls 1