WORKING_DIR=`dirname $0`
export WORKING_DIR

# print a message to stderr, preceded by the script name
function warn {
  echo -e "$0: $*" >&2
}

# check whether all of a list of processes are actually alive
function verify_pid {
  if [ -z "$*" ]; then
    return 1
  fi
  nprocs=`ps -p $* | (read header; cat) | wc -l`
  if [ $nprocs -eq $# ]; then
    return 0
  fi
  return 1
}

function assert_single_instance {
  local pidfile="${1:-$0.pid}"
  if [ ! -e "$pidfile" ]; then
    echo $$ > "$pidfile" 
    return 0
  fi
  # check for stale pid file, and print warning if running from the console
  verify_pid `cat "$pidfile"` && \
    { if test -t 1; then warn "this process is already running"; fi
      exit 1
    }
  echo $$ > "$pidfile"
  return 0
}

function cleanup_pidfile {
  local exit_code=$?
  local pidfile="${1:-$0.pid}"
  if test -e "$pidfile" -a `cat "$pidfile"` -eq $$; then
    rm $pidfile
  fi
  return $exit_code
}

function get_file_header {
  if test -z "$1" -o ! -r "$1"; then return 1; fi
  local _cat=cat
  if test "${1: -3:3}" = ".gz" -o "${1: -2:2}" = ".Z"; then
    _cat="gunzip -c"
  fi
  $_cat "$1" | head -1
}

function get_file_delimiter {
  local header=`get_file_header $1`
  if test -z "$header"; then return 1; fi
  echo "$header" | egrep -o '[^-_a-zA-Z0-9]' | head -1
}

function locate_field {
  local label="$1"
  local file="$2"
  local header=`get_file_header "$file"` || return 1
  local delim=`get_file_delimiter "$file"` || return 1
  local index=`indexof -s "$header" -d "$delim" -l "$label"`
  echo "$index"
  if test $index -eq 0; then return 1; fi
  return 0
}

function DIE_ON_PIPE_ERROR {
  local exit_codes="${PIPESTATUS[*]}"
  local exit_code
  for exit_code in $exit_codes; do
    if test $exit_code -ne 0; then
      warn "$*"
      exit $exit_code
    fi
  done
  return 0
}
