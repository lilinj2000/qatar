#! /bin/sh

node=`uname -n`
home_app=~/app/${node}

home_qatar=${home_app}/qatar

./configure --prefix=${home_qatar}

if test -d ${home_qatar}; then
    rm -rf ${home_qatar}
fi

make install

make distclean
