#!/bin/bash


if [ "$1" = "" ]
then
  echo
  echo "== Variables must be set!"
  echo "== ./create_project [1] [2]"
  echo "== [1]: name of the project"
  echo "== [2]: name of the proejct in the file"
else
  if [ "$2" = "" ]
  then
    echo
    echo "== Variables must be set!"
    echo "== ./create_project [1] [2]"
    echo "== [1]: name of the project"
    echo "== [2]: name of the proejct in the file"
  else
    mkdir ${1}
    cp ${KEBIPATH}/macros/dummies/CMakeLists.txt ${1}/CMakeLists.txt
    cp ${KEBIPATH}/macros/dummies/gitignore ${1}/.gitignore


    mkdir ${1}/geant4
    cp ${KEBIPATH}/macros/dummies/DUMMYDetectorConstruction.cc ${1}/geant4/${2}DetectorConstruction.cc
    cp ${KEBIPATH}/macros/dummies/DUMMYDetectorConstruction.hh ${1}/geant4/${2}DetectorConstruction.hh


    mkdir ${1}/detector
    cp ${KEBIPATH}/macros/dummies/LinkDefDetector.h     ${1}/detector/LinkDef.h
    cp ${KEBIPATH}/macros/dummies/DUMMYDetector.cc      ${1}/detector/${2}Detector.cc
    cp ${KEBIPATH}/macros/dummies/DUMMYDetector.hh      ${1}/detector/${2}Detector.hh
    cp ${KEBIPATH}/macros/dummies/DUMMYDetectorPlane.cc ${1}/detector/${2}DetectorPlane.cc
    cp ${KEBIPATH}/macros/dummies/DUMMYDetectorPlane.hh ${1}/detector/${2}DetectorPlane.hh


    mkdir ${1}/task
    cp ${KEBIPATH}/macros/dummies/LinkDefTask.h           ${1}/task/LinkDef.h
    cp ${KEBIPATH}/macros/dummies/DUMMYDoSomethingTask.cc ${1}/task/${2}DoSomethingTask.cc
    cp ${KEBIPATH}/macros/dummies/DUMMYDoSomethingTask.hh ${1}/task/${2}DoSomethingTask.hh


    mkdir ${1}/macros
    cp ${KEBIPATH}/macros/dummies/eve.C          ${1}/macros/
    cp ${KEBIPATH}/macros/dummies/mc.cc          ${1}/macros/${1}.mc.cc
    cp ${KEBIPATH}/macros/dummies/dummy.par      ${1}/macros/${1}.par
    cp ${KEBIPATH}/macros/dummies/run_geant4.mac ${1}/macros/
    cp ${KEBIPATH}/macros/dummies/vis.mac        ${1}/macros/
    cp ${KEBIPATH}/macros/dummies/doSomething.C  ${1}/macros/


    sed -i '' s/DUMMY/${2}/g ${1}/CMakeLists.txt
    sed -i '' s/DUMMY/${2}/g ${1}/geant4/*
    sed -i '' s/DUMMY/${2}/g ${1}/detector/*
    sed -i '' s/DUMMY/${2}/g ${1}/task/*
    sed -i '' s/DUMMY/${2}/g ${1}/macros/*

    sed -i '' s/PROJECTNAME/${1}/g ${1}/CMakeLists.txt
    sed -i '' s/PROJECTNAME/${1}/g ${1}/geant4/*
    sed -i '' s/PROJECTNAME/${1}/g ${1}/detector/*
    sed -i '' s/PROJECTNAME/${1}/g ${1}/task/*
    sed -i '' s/PROEJCTNAME/${1}/g ${1}/macros/*

    sed -i '' s/dummy/${1}/g ${1}/macros/*
  fi
fi
