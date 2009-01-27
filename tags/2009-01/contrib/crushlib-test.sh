#!/bin/bash

. crushlib.sh || { echo "failed to source crushlib.sh" >&2; exit 1; }

warn "this is only\na test"
assert_single_instance
trap 'cleanup_pidfile; exit $?' EXIT TERM INT HUP QUIT

echo "field-0|field-1|field-2" > "$WORKING_DIR/tmpfile.log"
delim=`get_file_delimiter "$WORKING_DIR/tmpfile.log"`
if [ "$delim" != '|' ]; then
  warn "get_file_delimiter failed"
fi
idx=`locate_field "field-1" "$WORKING_DIR/tmpfile.log"`
if [ $idx -ne 2 ]; then
  warn "locate_field failed"
fi

cat "$WORKING_DIR/tmpfile.log" | grep field > /dev/null
DIE_ON_PIPE_ERROR "this pipeline should work"
rm "$WORKING_DIR/tmpfile.log"
(cat "$WORKING_DIR/tmpfile.log" | grep field) > /dev/null 2>&1
DIE_ON_PIPE_ERROR "this pipeline was expected to fail"

