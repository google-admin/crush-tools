#!/bin/bash

if [ ! -d "../wiki" ] || [ ! -d "./src" ]; then
  echo "$0: must be run from trunk/ or tags/ in a full repository checkout." \
    >&2
  exit 1
fi

if [ "$*" ]; then
  argstabs="$*"
  regen_index=1
else
  argstabs="`find src -name 'args.tab'`"
fi

for argstab in $argstabs; do
  app_name=$(basename $(dirname $argstab))
  app_name=`echo ${app_name:0:1} | tr 'a-z' 'A-Z'`${app_name:1}
  dev_tools/argstab2wiki.pl "$argstab" \
    > ../wiki/${app_name}UserDocs.wiki

  if [[ "`svn stat ../wiki/${app_name}UserDocs.wiki`" =~ "^?" ]]; then
    svn add ../wiki/${app_name}UserDocs.wiki
  fi

  pages="$pages ${app_name}UserDocs"
done

if [ $regen_index ]; then
  cat > ../wiki/UserDocs.wiki << END_USERDOCS_TOC
#summary CRUSH User Documentation

= Applications =

END_USERDOCS_TOC

  for page in $pages; do
    echo "  * $page" >> ../wiki/UserDocs.wiki
  done
fi
svn commit ../wiki

exit $?
