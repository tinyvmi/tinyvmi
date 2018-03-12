#!/bin/bash

tinyDir=$(dirname $0)

cd $tinyDir

if [ -z $1 ];then

make tinyvmi-stubdom

cd ../mini-os-x86_64-tinyvmi
xl create -c ../../extras/mini-os/domain_config
cd -

else

cd ..
make clean -C ../extras/mini-os
make clean -C tinyvmi

make -C ../extras/mini-os
res=$?
if [ $res -ne 0 ]; then
	echo "error run make, return $res"
	exit $res
fi

make tinyvmi-stubdom
res=$?
if [ $res -ne 0 ]; then
	echo "error run make, return $res"
	exit $res
fi

cd mini-os-x86_64-tinyvmi
xl create -c ../../extras/mini-os/domain_config
cd -

fi
