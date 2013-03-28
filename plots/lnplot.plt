reset

set terminal epslatex size 5.0,3.5 standalone header "\\usepackage{libertine}\n" color colortext
set output "$0.tex"

# Line styles
set border linewidth 2
set style line 1 linecolor rgb '#FF0000' linetype 1 linewidth 3 # red
set style line 2 linecolor rgb '#FF8080' linetype 1 linewidth 2 # ltred
set style line 3 linecolor rgb '#00FF00' linetype 1 linewidth 3 # green
set style line 4 linecolor rgb '#80FF80' linetype 1 linewidth 2 # ltgreen

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
set key left

plot "./output.txt_stream_0_ad" index 0 using 1:2 with lines linestyle 1 title "Actual function", \
"./output.txt_stream_0_ad" index 1 using 1:2 with lines linestyle 2 title "Events per second (original)", \
"./est_out.txt" using 1:2 with lines linestyle 3 title "Estimated function",\
"./est_out.txt" using 1:3 with lines linestyle 4 title "Events per time interval (estimator)",\
"./est_out.txt" using 1:4 with lines linestyle 5 title "Lambda estimates"
