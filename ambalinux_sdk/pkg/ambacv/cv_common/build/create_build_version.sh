#!/bin/bash
# First parameter to script   : (absolute) Source directory
# Second parameter to script  : (absolute) Target build directory
# Third parameter to script   : (absolute) Common directory
source_dir=$1
target_dir=$2
common_dir=$3

pushd . >/dev/null

# BUILD TOOL TAGS
toolver_vas=`tv2 -version vas`
toolver_vpref=`tv2 -version VpRef`
toolver_libvdg=`tv2 -version libvdg`
toolver_cnngen=`tv2 -version AmbaCnnUtils`
toolver_dagcfg=`tv2 -version dagcfg`
toolver_tablear=`tv2 -version table_ar`
toolversion=`echo vas-$toolver_vas-vpref-$toolver_vpref-libvdg-$toolver_libvdg-cnngen-$toolver_cnngen-dagcfg-$toolver_dagcfg-tablear-$toolver_tablear`

function giterror()
{
    echo "Error from GIT, no version information provided"
    unversioned
}

function unversioned()
{
	cd $target_dir
    echo -e "#define LIBRARY_VERSION "'"'"UNVERSIONED"'"'"\n#define TOOL_VERSION    "'"'"$toolversion"'"' > build_version.h.tmp

    if [ -f build_version.h ]; then
        diff build_version.h.tmp build_version.h > /dev/null
        if [ $? != 0 ]; then
            cp build_version.h.tmp build_version.h
            echo "Updating : Library at $source_dir has no version information"
        fi
    else
        cp build_version.h.tmp build_version.h
        echo "Creating : Library at $source_dir has no version information"
    fi

    popd > /dev/null
    exit 0
}

if [ -d $source_dir ]; then

    cd $source_dir

    pretest=`git describe --always >& /dev/null`
    if [ $? != 0 ]; then unversioned; fi
    gitdescribe=`git describe --always --long --abbrev=16 --dirty="-dirty"`
    if [ $? != 0 ]; then gitdescribe="none"; fi
    gitbranchcheck=`git symbolic-ref --short HEAD >& /dev/null`
    if [ $? != 0 ]; then gitbranchcheck="none"; fi
    gitbranch=`git symbolic-ref --short HEAD`
    if [ $? != 0 ]; then gitbranch="none"; fi
    gitcurrhash=`git rev-parse --short=16 HEAD`
    if [ $? != 0 ]; then gitcurrhash="none"; fi
    gitserver=`git remote`
    if [ $? != 0 ]; then gitserver="none"; fi
    gitremotehash=`git show-ref $gitbranch --abbrev=16 | grep remotes | grep $gitserver | cut -d ' ' -f 1`
    if [ $? != 0 ]; then gitremotehash="none"; fi

#   echo ">>>>> SRCPATH  : $source_dir"
#   echo ">>>>> DSTPATH  : $target_dir"
#   echo ">>>>> DESCRIBE : $gitdescribe"
#   echo ">>>>> BRANCH   : $gitbranch"
#   echo ">>>>> CURR     : $gitcurrhash"
#   echo ">>>>> SERVER   : $gitserver"
#   echo ">>>>> REMOTE   : $gitremotehash"

    if [ "$gitcurrhash" != "$gitremotehash" ]; then
        gittrailer=`echo -rrev-$gitremotehash`
    else
        gittrailer=""
    fi

#   echo ">>>>> TRAILER  : $gittrailer"

    cd $common_dir
    cmngitdescribe=`git describe --long --abbrev=16 --dirty="-dirty"`
    if [ $? != 0 ]; then cmngitdescribe="none"; fi
    cmngitbranch=`git symbolic-ref --short HEAD`
    if [ $? != 0 ]; then cmngitbranch="none"; fi
    cmngitcurrhash=`git rev-parse --short=16 HEAD`
    if [ $? != 0 ]; then cmngitcurrhash="none"; fi
    cmngitserver=`git remote`
    if [ $? != 0 ]; then cmngitserver="none"; fi
    cmngitremotehash=`git show-ref $cmngitbranch --abbrev=16 | grep -w remotes | grep $cmngitserver | cut -d ' ' -f 1 `
    if [ $? != 0 ]; then cmngitremotehash="none"; fi

#   echo ">>>>>>> CMNPATH  : $common_dir"
#   echo ">>>>>>> DESCRIBE : $cmngitdescribe"
#   echo ">>>>>>> BRANCH   : $cmngitbranch"
#   echo ">>>>>>> CURR     : $cmngitcurrhash"
#   echo ">>>>>>> SERVER   : $cmngitserver"
#   echo ">>>>>>> REMOTE   : $cmngitremotehash"

    if [ "$cmngitcurrhash" != "$cmngitremotehash" ]; then
        cmngittrailer=`echo -rrev-$cmngitremotehash`
    else
        cmngittrailer=""
    fi

#   echo ">>>>>>> TRAILER  : $cmngittrailer"

    finalversion=`echo $gitdescribe$gittrailer-common-$cmngitdescribe$cmngittrailer`

    cd $target_dir
    echo -e "#define LIBRARY_VERSION "'"'"$finalversion"'"'"\n#define TOOL_VERSION    "'"'"$toolversion"'"' > build_version.h.tmp

    if [ -f build_version.h ]; then
        diff build_version.h.tmp build_version.h > /dev/null
        if [ $? != 0 ]; then
            cp build_version.h.tmp build_version.h
            echo "Updating : Library at $source_dir has version $finalversion"
        fi
    else
        cp build_version.h.tmp build_version.h
        echo "Creating : Library at $source_dir has version $finalversion"
    fi
    rm build_version.h.tmp
else
    unversioned
fi

popd > /dev/null
exit 0

