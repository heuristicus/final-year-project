reset

set terminal epslatex size 5.0,3.5 standalone header "\\usepackage{cmbright}\n" color colortext
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#FFA0A0' linetype 1 linewidth 2 # ltred
set style line 3 linecolor rgb '#0000FF' linetype 1 linewidth 3 # blue

# Axis labels
set xlabel '$$t$$ (sec)'
set ylabel 'Events per second'

# Axis ranges
#set yrange [0:25]
#set xrange[0:100]

# Tics in latex format
set format '$%g$'

# Tics
#set xtics 0,10
#set ytics 0,5
#set tics scale 0.75

# Key
set key right

plot "$1" index 0 using 1:2 with lines linestyle 1 title "Actual function", \
"$1" index 1 using 1:2 with lines linestyle 2 title "Events per second", \
"$2" using 1:2 with lines linestyle 3 title "Piecewise estimate"
