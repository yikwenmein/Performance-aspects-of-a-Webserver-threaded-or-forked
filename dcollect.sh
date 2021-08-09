#!/bin/bash

rm -rf statistics.log
for i in {1..30}; do
    for k in {1..32}; do
	bonkers=$(ab -n 10000 -c $i http://127.0.0.1:7799/file.txt 2>/dev/null | grep 'Requests per second');
#	sleep 1
	value=$(echo "$bonkers" | awk '{print $4}');
	echo "C=$i, => $value";
	echo "$value" >> "perf_$i.txt" ;
	
    done;
    echo "Done all repetitions for $i, doing statistics (with awk). "
    statistics=$(awk '{for(i=1;i<=NF;i++) {sum[i] += $i; sumsq[i] += ($i)^2}} 
          END {for (i=	     1;i<=NF;i++) { 
          printf "%f %f \n", sum[i]/NR, sqrt((sumsq[i]-sum[i]^2/NR)/NR)}
         }' perf_$i.txt)
    echo "$i => $statistics " | tee -a statistics.log
done

gnuplot dcollect.p
