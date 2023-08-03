#! /bin/sh

#################################################
# Perpare ambacv package
#################################################

ABSPATH=$(readlink -f "$0")
SRCDIR=$(dirname "$ABSPATH")
if [ $# != 0 ]; then
    if [ "$1" = "--help" ]; then
        echo "Usage: prepare_package.sh [SRCDIR]"
        echo "       SRCDIR,       ambacv source path"
        echo "                     default. ${SRCDIR}"
        return
    fi
    SRCDIR=$1
fi

#AMBACV PREPARE CVTASK
if [ -e $SRCDIR/orc_framework/cvlib_common/orcvp ]; then
    mkdir -p $SRCDIR/orc_framework/cvlib/orcvp;
    for i in $SRCDIR/orc_framework/cvlib_common/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/orc_framework/cvlib/orcvp/$name;
        ln -sf ../../cvlib_common/orcvp/$name $SRCDIR/orc_framework/cvlib/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/orc_framework/cvlib_$PROJECT/orcvp ]; then
    mkdir -p $SRCDIR/orc_framework/cvlib/orcvp;
    for i in $SRCDIR/orc_framework/cvlib_$PROJECT/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/orc_framework/cvlib/orcvp/$name;
        ln -sf ../../cvlib_$PROJECT/orcvp/$name $SRCDIR/orc_framework/cvlib/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/orc_framework/cvtask_common/orcvp ]; then
    mkdir -p $SRCDIR/orc_framework/cvtask/orcvp;
    for i in $SRCDIR/orc_framework/cvtask_common/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/orc_framework/cvtask/orcvp/$name;
        ln -sf ../../cvtask_common/orcvp/$name $SRCDIR/orc_framework/cvtask/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/orc_framework/cvtask_common/orcany ]; then
    mkdir -p $SRCDIR/orc_framework/cvtask/orcany;
    for i in $SRCDIR/orc_framework/cvtask_common/orcany/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/orc_framework/cvtask/orcany/$name;
        ln -sf ../../cvtask_common/orcany/$name $SRCDIR/orc_framework/cvtask/orcany/$name;
    done;
fi
if [ -e $SRCDIR/orc_framework/cvtask_$PROJECT/orcvp ]; then
    mkdir -p $SRCDIR/orc_framework/cvtask/orcvp;
    for i in $SRCDIR/orc_framework/cvtask_$PROJECT/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/orc_framework/cvtask/orcvp/$name;
        ln -sf ../../cvtask_$PROJECT/orcvp/$name $SRCDIR/orc_framework/cvtask/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/orc_framework/cvtask_$PROJECT/orcany ]; then
    mkdir -p $SRCDIR/orc_framework/cvtask/orcany;
    for i in $SRCDIR/orc_framework/cvtask_$PROJECT/orcany/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/orc_framework/cvtask/orcany/$name;
        ln -sf ../../cvtask_$PROJECT/orcany/$name $SRCDIR/orc_framework/cvtask/orcany/$name;
    done;
fi

#AMBACV PREPARE LIBS
if [ -e $SRCDIR/cv_common/libs/orc/cvlib_common/orcvp ]; then
    mkdir -p $SRCDIR/cv_common/libs/orc/cvlib/orcvp;
    for i in $SRCDIR/cv_common/libs/orc/cvlib_common/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/cv_common/libs/orc/cvlib/orcvp/$name;
        ln -sf ../../cvlib_common/orcvp/$name $SRCDIR/cv_common/libs/orc/cvlib/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/cv_common/libs/orc/cvlib_$PROJECT/orcvp ]; then
    mkdir -p $SRCDIR/cv_common/libs/orc/cvlib/orcvp;
    for i in $SRCDIR/cv_common/libs/orc/cvlib_$PROJECT/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/cv_common/libs/orc/cvlib/orcvp/$name;
        ln -sf ../../cvlib_$PROJECT/orcvp/$name $SRCDIR/cv_common/libs/orc/cvlib/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/cv_common/libs/orc/cvtask_common/orcvp ]; then
    mkdir -p $SRCDIR/cv_common/libs/orc/cvtask/orcvp;
    for i in $SRCDIR/cv_common/libs/orc/cvtask_common/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/cv_common/libs/orc/cvtask/orcvp/$name;
        ln -sf ../../cvtask_common/orcvp/$name $SRCDIR/cv_common/libs/orc/cvtask/orcvp/$name;
    done;
fi
if [ -e $SRCDIR/cv_common/libs/orc/cvtask_$PROJECT/orcvp ]; then
    mkdir -p $SRCDIR/cv_common/libs/orc/cvtask/orcvp;
    for i in $SRCDIR/cv_common/libs/orc/cvtask_$PROJECT/orcvp/*; do
        name=$(basename "$i")
        rm -rf $SRCDIR/cv_common/libs/orc/cvtask/orcvp/$name;
        ln -sf ../../cvtask_$PROJECT/orcvp/$name $SRCDIR/cv_common/libs/orc/cvtask/orcvp/$name;
    done;
fi

