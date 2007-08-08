
outfile=$wdir/out/`basename $file | sed 's/.gz$//'`

gzdog $file | aggregate -p -k 4 -c 1 \
	> $outfile

