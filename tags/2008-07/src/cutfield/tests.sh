#!/bin/bash

export wdir=`dirname $0`

has_error=0
for f in $wdir/test/*.sh; do
  $f
  if [ $? -ne 0 ]; then
    has_error=1
  fi
done

exit $has_error
