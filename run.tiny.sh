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

git log | head

function checkAndCreateGuest(){

 hasGuestDom
 hasGuest="$?"
 #echo "hasGuest: $hasGuest"

 if [ "$hasGuest" -ne 0 ]
 then
   echo "[OK]: A potential Target VM is running."
   return 1
 fi

 try=1
 maxtry=2

 while [ "$hasGuest" -eq 0 ]
 do
  if [ "$try" -gt $maxtry ];then
     break
  fi

  echo "--------------------------------------"
  echo "[WARNING] Has no guest VM. Please create guest VM as the introspection target before run this."
  echo "[WARNING] Please create the Target VM and try again."
  echo "--------------------------------------"

  exit 1
  #echo "create guest using $guestXLconfig"
  #xl create $guestXLconfig

  hasGuestDom
  hasGuest="$?"
  ((try++))
 done

 if [ "$hasGuest" -eq 0 ]
 then
   # echo "failed to create guest"
   Usage
   return 0
 fi

 # success to create guest, wait for 60 seconds allowing the guest to boot
 waitTime=$WAITBOOT

 return 1

}


# create TinyVMI VM and run it.
# this will also set xenstore permissions after knowing its domain ID.
# It achieve this by the following tricks:
#   1. create TinyVMI as a guest VM;
#   2. pause TinyVMI and get its ID by 'xl domid TinyVMI';
#   3. change xenstore permission by 'xenstore-chmod';
#   4. resume TinyVMI.
function createTinyVMI(){

  echo "wait for $waitTime seconds before start tinyVMI"
  sleep $waitTime
  cd mini-os-x86_64-tinyvmi
  xl create -c ../tinyvmi/domain_config &
  echo "$tinyVM started"
  sleep 1
  xl pause $tinyVM

  echo "$tinyVM paused"
  xl list

  tinyID=$(xl domid $tinyVM)
  echo "get tinyID: $tinyID"

  # now change the permission of xenstore directory.
  # this will allow TinyVMI to convert a VM's string name into its 
  # domain ID.

  xenstore-chmod -r '/local/domain' r$tinyID

  echo "DONE: xenstore-chmod -r '/local/domain' r$tinyID"
  xenstore-ls -p '/local/domain'
  # unpause TinyVMI to resume normal run
  xl unpause $tinyVM
  echo "$tinyVM ($tinyID) unpaused"

  cd -

}

checkAndCreateGuest
success="$?" # return 0 if failed, return 1 if success.

if [ $success -eq 0 ];then
  exit -1
fi



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
