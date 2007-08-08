#!/bin/bash

. /usr/local/devprac/deploy/internal/lib/crush ||
  { echo "failed to init crush." >2; exit 1; }

startdate=`date -d "8 days ago" "+%Y-%m-%d"`
enddate=`date -d "1 day ago" "+%Y-%m-%d"`

files=`findfiles -d /export/home/dfarep/mount/dcftp2/ftp_home/eng_dfadbank/danalyzer.270/3048/ \
	-g 'NetworkClick_3048_*.log.gz' -s $startdate -e $enddate`

$wdir/../foreach_parallel -l 4 -v file $files << 'END_PARALLEL'
	outfile=$wdir/out/`basename $file | sed 's/.gz$//'`

	gzdog $file | aggregate -p -k 4 -c 1 \
		> $outfile
END_PARALLEL


