#!/bin/bash

policydir="tools/flask/policy"
policyfile="tools/flask/policy/xenpolicy-4.*"
bootpolicydir="/boot/flask/"

tinyDir=$(dirname $0)

cd $tinyDir

cd ../../

echo "compiling the policy"

make -C $policydir

echo ""
echo "load policy"

xl loadpolicy $policyfile
echo ""
echo "done load policy"
echo ""

if [ ! -z $1 ]; then
 echo "copy file to dir: $bootpolicydir"
 cp $policyfile $bootpolicydir
 echo "done copy"
 echo ""
fi
