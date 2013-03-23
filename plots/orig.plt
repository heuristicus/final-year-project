reset

set terminal epslatex size 5.0,3.5 standalone header "\\usepackage{libertine}\n"
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#FFA0A0' linetype 1 linewidth 2 # ltred
set style line 3 linecolor rgb '#0000FF' linetype 1 linewidth 3 # blue
set style line 4 linecolor rgb '#00FF00' linetype 1 linewidth 3 # ltgreen

# Axis labels
set xlabel '$$t$$'
set ylabel '$$\lambda$$'

# Axis ranges
set yrange [0:20]
#set xrange[0:100]

# Tics in latex format
#set format '$$%g$$'

# Tics
#set xtics 0,10
#set ytics 0,5
#set tics scale 0.75

# Key
set key right

plot "$1" using 1:2 with lines ls 1 title "$$\\lambda(t)$$"