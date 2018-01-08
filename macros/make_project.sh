#!/bin/bash


if [ "$1" = "" ]
then
  echo
  echo "== Variables must be set!"
  echo "== ./create_project [1] [2]"
  echo "== [1]: name of the project"
  echo "== [2]: name of the proejct in the file"
fi


if [ "$2" = "" ]
then
  echo
  echo "== Variables must be set!"
  echo "== ./create_project [1] [2]"
  echo "== [1]: name of the project"
  echo "== [2]: name of the proejct in the file"
fi


mkdir Project_${1}
cp ${KEBIPATH}/macros/dummies/CMakeLists.txt Project_${1}/CMakeLists.txt


mkdir Project_${1}/geant4
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorConstruction.cc Project_${1}/geant4/${2}DetectorConstruction.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorConstruction.hh Project_${1}/geant4/${2}DetectorConstruction.hh


mkdir Project_${1}/detector
cp ${KEBIPATH}/macros/dummies/LinkDefDetector.h     Project_${1}/detector/LinkDef.h
cp ${KEBIPATH}/macros/dummies/DUMMYDetector.cc      Project_${1}/detector/${2}Detector.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDetector.hh      Project_${1}/detector/${2}Detector.hh
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorPlane.cc Project_${1}/detector/${2}DetectorPlane.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorPlane.hh Project_${1}/detector/${2}DetectorPlane.hh


mkdir Project_${1}/task
cp ${KEBIPATH}/macros/dummies/LinkDefTask.h           Project_${1}/task/LinkDef.h
cp ${KEBIPATH}/macros/dummies/DUMMYDoSomethingTask.cc Project_${1}/task/${2}DoSomethingTask.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDoSomethingTask.hh Project_${1}/task/${2}DoSomethingTask.hh


mkdir Project_${1}/macros
cp ${KEBIPATH}/macros/dummies/eve.C          Project_${1}/macros/
cp ${KEBIPATH}/macros/dummies/mc.cc          Project_${1}/macros/${1}.mc.cc
cp ${KEBIPATH}/macros/dummies/dummy.par      Project_${1}/macros/
cp ${KEBIPATH}/macros/dummies/run_geant4.mac Project_${1}/macros/
cp ${KEBIPATH}/macros/dummies/vis.mac        Project_${1}/macros/
cp ${KEBIPATH}/macros/dummies/doSomething.C  Project_${1}/macros/


sed -i '' s/DUMMY/${2}/g Project_${1}/CMakeLists.txt
sed -i '' s/DUMMY/${2}/g Project_${1}/geant4/*
sed -i '' s/DUMMY/${2}/g Project_${1}/detector/*
sed -i '' s/DUMMY/${2}/g Project_${1}/task/*
sed -i '' s/DUMMY/${2}/g Project_${1}/macros/*

sed -i '' s/PROJECTNAME/${1}/g Project_${1}/CMakeLists.txt
sed -i '' s/PROJECTNAME/${1}/g Project_${1}/geant4/*
sed -i '' s/PROJECTNAME/${1}/g Project_${1}/detector/*
sed -i '' s/PROJECTNAME/${1}/g Project_${1}/task/*
sed -i '' s/PROEJCTNAME/${1}/g Project_${1}/macros/*
