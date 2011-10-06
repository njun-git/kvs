#! /usr/bin/env sh

renderers='Renderer01 Renderer02'
runpath=$PWD
runname=${runpath##*/}

for renderer in $renderers;
do
echo $renderer
 (cd $runpath; DISPLAY=localhost:0.0 KVS_TDW_RENDERER=$renderer ./$runname $* ) &

#! if use ssh
#! ssh $renderer "(cd $runpath; DISPLAY=localhost:0.0 KVS_TDW_RENDERER=$renderer ./$runname $*)" &

done

 (cd $runpath; sleep 3; ./$runname $*)

read line
