mkdir project_$1


mkdir project_$1/geant4
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorConstruction.cc project_$1/geant4/${2}DetectorConstruction.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorConstruction.hh project_$1/geant4/${2}DetectorConstruction.hh


mkdir project_$1/detector
cp ${KEBIPATH}/macros/dummies/LinkDefDetector.h     project_$1/detector/LinkDef.h
cp ${KEBIPATH}/macros/dummies/DUMMYDetector.cc      project_$1/detector/${2}Detector.cc     
cp ${KEBIPATH}/macros/dummies/DUMMYDetector.hh      project_$1/detector/${2}Detector.hh     
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorPlane.cc project_$1/detector/${2}DetectorPlane.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDetectorPlane.hh project_$1/detector/${2}DetectorPlane.hh


mkdir project_$1/task
cp ${KEBIPATH}/macros/dummies/LinkDefTask.h           project_$1/task/LinkDef.h
cp ${KEBIPATH}/macros/dummies/DUMMYDoSomethingTask.cc project_$1/task/${2}DoSomethingTask.cc
cp ${KEBIPATH}/macros/dummies/DUMMYDoSomethingTask.hh project_$1/task/${2}DoSomethingTask.hh


mkdir project_$1/macros
cp ${KEBIPATH}/macros/dummies/eve.C          project_$1/macros/
cp ${KEBIPATH}/macros/dummies/mc.cc          project_$1/macros/
cp ${KEBIPATH}/macros/dummies/dummy.par      project_$1/macros/
cp ${KEBIPATH}/macros/dummies/run_geant4.mac project_$1/macros/
cp ${KEBIPATH}/macros/dummies/vis.mac        project_$1/macros/
cp ${KEBIPATH}/macros/dummies/doSomething.C  project_$1/macros/


sed -i -- s/DUMMY/$2/g project_$1/geant4/*
sed -i -- s/DUMMY/$2/g project_$1/detector/*
sed -i -- s/DUMMY/$2/g project_$1/task/*
sed -i -- s/DUMMY/$2/g project_$1/macros/*
