set terminal epslatex size 5.0,3.5 standalone color colortext header "\\usepackage{libertine}\n"
set output "kernel.tex"

set border linewidth 2


delta(x) = x ? 1 : 0
sqr(x) = x*x
cub(x) = x*x*x;
set samples 5000 
plot [u=-3:3] [0:1.2] (1.0-abs(u))*delta(abs(u)<1.0) title "Triangle" lt 1 lw 3 lc rgb "red", \
 35.0/32.0*cub(1.0-sqr(u))*delta(abs(u)<1.0) title "Triweight" lt 1 lw 3 lc rgb "blue", \
 1.0/sqrt(2.0*pi)*exp(-0.5*sqr(u))           title "Gaussian" lt 1 lw 3 lc rgb "magenta",\
 0.5*delta(abs(u)<1)                         title "Uniform" lt 1 lw 3 lc rgb "green"