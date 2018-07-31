#!/bin/bash

#guestXLconfig="/home/smeller/extdisk/lab/vmi/vmidocker/xenGuests/aubu16.cfg"

tinyDir=$(dirname $0)

tinyVM="TinyVMI"
tinyID="0"

waitTime="1"
WAITBOOT="60"

function Usage(){
  echo "--------------------------------------"
  echo "Usage: $0 [option [GNU make option]]"
  echo "Available options:"
  echo "'': if no option, it will just run TinyVMI without re-build."
  echo "'$0 make, m': just build TinyVMI, same as make -C stubdom/ tinyvmi-stubdom."
  echo "'$0 makerun, mr': build TinyVMI and run it."
  echo "'$0 build, b': make clean and rebuild TinyVMI."
  echo "'$0 buildrun, br': make clean and rebuild TinyVMI."
  echo "With last three options, you can also pass additional options to GNU make as following:"
  echo "'$0 make -j4'"
  echo "'$0 makerun -j4'"
  echo "'$0 build -j4'"
  echo "--------------------------------------"
  echo ""
}

git log | head


# create TinyVMI VM and run it.
# this will also set xenstore permissions after knowing its domain ID.
# It achieve this by the following tricks:
#   1. create TinyVMI as a guest VM;
#   2. pause TinyVMI and get its ID by 'xl domid TinyVMI';
#   3. change xenstore permission by 'xenstore-chmod';
#   4. resume TinyVMI.
function createTinyVMI(){

  cd mini-os-x86_64-tinyvmi
  xl create -c ../tinyvmi/domain_config 
  cd -

}


# go to dir of xen-src/stubdom/
cd $tinyDir
cd ..

options=""
if [ ! -z $2 ]; then
options="$2"
fi

opRun="run"
opMake="make"
opMakeRun="makerun"
opBuild="build"
opBuildRun="buildrun"
mode=$1

if [ -z "$mode" -o "$mode" == "$opRun" -o "$mode" == "r" ];then

  createTinyVMI

elif [ "$mode" == "$opMake" -o "$mode" == "m" -o "$mode" == "$opMakeRun" -o "$mode" == "mr" ];then
 
  make tinyvmi-stubdom $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  if [ "$mode" == "$opMakeRun" -o "$mode" == "mr" ];then
    createTinyVMI
  fi

elif [ "$mode" == "$opBuild" -o "$mode" == "b" -o "$mode" == "$opBuildRun" -o "$mode" == "br" ];then

  make clean -C ../extras/mini-os $options
  make clean -C tinyvmi $options

  make -j4 -C ../extras/mini-os $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  make -j4 tinyvmi-stubdom $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  if [ "$mode" == "$opBuildRun" -o "$mode" == "br" ];then
    createTinyVMI
  fi

else

  Usage
  
fi
