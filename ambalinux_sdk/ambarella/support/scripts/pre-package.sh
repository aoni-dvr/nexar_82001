#! /bin/sh

#################################################
# Stamping git revision of VLC
#################################################

echo "Stamping VLC git revision.."

if test -z "$1"; then
  echo "Usage: $0 <path_to_boss_sdk>"
  exit 1
fi

vlc_cust=$1/pkg/vlc

if ! test -d ${vlc_cust}/.git; then
  echo "Error: ${vlc_cust}/.git does not exist"
  exit 1
fi

git --git-dir="${vlc_cust}/.git/" describe \
    --tags --long --match '?.*.*' --always > \
    ${vlc_cust}/src/revision.txt

echo -n "${vlc_cust}/src/revision.txt: "
cat ${vlc_cust}/src/revision.txt
