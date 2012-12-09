reset

set terminal epslatex size 5.0,3.5 standalone color colortext
set output "tist.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#dd181f' linetype 1 linewidth 2  # red
set style line 2 linecolor rgb '#34D800' linetype 1 linewidth 2 # green
set style line 3 linecolor rgb '#0060ad' linetype 1 linewidth 2  # blue

# Axis labels
set xlabel 'Time (sec) $z$'
set ylabel 'Event count $t$'

# Axis ranges
set yrange [0:25]

# Tics in latex format
set format '$%g$'

# Tics
#set xtics 0,10
set ytics 0,5
#set tics scale 0.75

plot sin(x) linestyle 1 title '$\sin(x)$'

# Plot. $1 should contain event data as well as bin data (usually {filename}_ad).
# $2 should be data for line estimates
#plot "ad.txt" index 0 using 1:2 with lines linestyle 1 title 'Generating function',\
#     "ad.txt" index 1 using 1:2 with linespoints linestyle 2 title 'Bin counts',\
#     "est.txt" using 1:2 with lines linestyle 3 title 'Estimated function'