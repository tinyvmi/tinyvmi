#!/bin/bash

guestXLconfig="/home/tupipa/extdisk/lab/vmi/vmidocker/xenGuests/u1432bhvm.cfg"

tinyDir=$(dirname $0)

waitTime="1"
WAITBOOT="60"

function hasGuestDom(){

 lists="$(xl list)"

 lines=$(echo "$(xl list)" | wc -l)

 #echo "get $lines lines"

 if [ "$lines" -eq 2 ]
 then
	echo "No domU guest on Xen"
	xl list
	return 0
 elif [ "$lines" -gt 2 ]
 then
	echo "found domU guests"
	xl list
	return 1
 else
	echo "something wrong, number of domains is less than 1"
	xl list
	return 0
 fi

}

function checkAndCreateGuest(){

 hasGuestDom
 hasGuest="$?"
 #echo "hasGuest: $hasGuest"

 if [ "$hasGuest" -ne 0 ]
 then
   echo "no need to create guest"
   return 1
 fi

 try=1
 maxtry=2

 while [ "$hasGuest" -eq 0 ]
 do
  if [ "$try" -gt $maxtry ];then
     break
  fi
  echo "has no guest, now try to create one"
  echo "create guest using $guestXLconfig"
  xl create $guestXLconfig
  hasGuestDom
  hasGuest="$?"
  ((try++))
 done

 if [ "$hasGuest" -eq 0 ]
 then
   echo "failed to create guest"
   return 0
 fi

 # success to create guest, wait for 60 seconds allowing the guest to boot
 waitTime=$WAITBOOT

 return 1

}


checkAndCreateGuest
success="$?" # return 0 if failed, return 1 if success.

if [ $success -eq 0 ];then
  exit -1
fi




cd $tinyDir
cd ..

options=""
if [ ! -z $2 ]; then
options="$2"
fi

justMake="make"
makeRun="makerun"
mode=$1

if [ -z $mode ];then

  #make tinyvmi-stubdom
  #res=$?
  #if [ $res -ne 0 ]; then
  #	echo "error run make, return $res"
  #	exit $res
  #fi

  echo "wait for $waitTime seconds before start tinyVMI"
  sleep $waitTime
  cd mini-os-x86_64-tinyvmi
  xl create -c ../../extras/mini-os/domain_config
  cd -

elif [ "$mode" == "$justMake" ];then
 
  make tinyvmi-stubdom $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  # echo "wait for $waitTime seconds before start tinyVMI"
  # sleep $waitTime
  # cd mini-os-x86_64-tinyvmi
  # xl create -c ../../extras/mini-os/domain_config
  # cd -

elif [ "$mode" == "$makeRun" ];then

  make tinyvmi-stubdom $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  echo "wait for $waitTime seconds before start tinyVMI"
  sleep $waitTime
  cd mini-os-x86_64-tinyvmi
  xl create -c ../../extras/mini-os/domain_config
  cd -

else

  # exit 1
  make clean -C ../extras/mini-os $options
  make clean -C tinyvmi $options

  make -C ../extras/mini-os $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  make tinyvmi-stubdom $options
  res=$?
  if [ $res -ne 0 ]; then
    echo "error run make, return $res"
    exit $res
  fi

  echo "wait for $waitTime seconds before start tinyVMI"
  sleep $waitTime
  cd mini-os-x86_64-tinyvmi
  xl create -c ../../extras/mini-os/domain_config
  cd -

fi
