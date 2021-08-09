set terminal png size 1024,1024
set output 'statistics.png'
set xlabel 'Concurrency Level'
set ylabel 'Requests/s'
plot 'statistics.log' using 1:3 title 'Average' with lines,  'statistics.log' using 1:4 title 'std.dev' with lines
