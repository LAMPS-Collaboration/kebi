#include "KBG4RunManager.hh"
#include "KBMCDataManager.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "globals.hh"
#include "G4ProcessTable.hh"
#include "G4GDMLParser.hh"
#include "TSystem.h"

KBG4RunManager::KBG4RunManager()
:G4RunManager()
{
  fVolumes = new KBParameterContainer();
  fVolumes -> SetName("Volumes");

  fSensitiveDetectors = new KBParameterContainer();
  fSensitiveDetectors -> SetName("SensitiveDetectors");

  fProcessTable = new KBParameterContainer();
  fProcessTable -> SetName("ProcessTable");
}

KBG4RunManager::~KBG4RunManager()
{
}

void KBG4RunManager::Initialize()
{
  G4RunManager::Initialize();

  SetOutputFile(fPar->GetParString("G4OutputFile").Data());
  SetGeneratorFile(fPar->GetParString("G4InputFile").Data());

  auto procNames = G4ProcessTable::GetProcessTable() -> GetNameList();
  Int_t idx = 0;
  fProcessTable -> SetPar("Primary", idx++);
  for (auto name : *procNames)
    fProcessTable -> SetPar(name, idx++);

  if (fPar->CheckPar("G4ExportGDML"))
  {
    TString fileName = fPar -> GetParString("G4ExportGDML");
    TString name = gSystem -> Which(".", fileName.Data());

    if (name.IsNull()) {
      g4_info << "Exporting geometry in GMDL format: " << fileName << endl;
      G4GDMLParser parser;
      auto world = G4RunManagerKernel::GetRunManagerKernel() -> GetCurrentWorld();
      parser.Write(fileName.Data(),world);
    }
    else {
      g4_warning << "The file " << fileName << " exist already." << endl;
      g4_warning << "Stopped exporting geomtry" << endl;
    }
  }
}

void KBG4RunManager::Run(G4int argc, char **argv, const G4String &type)
{
  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  TString command("/control/execute ");

  if (fPar->CheckPar("G4VisFile")) {
    auto fileName = fPar -> GetParString("G4VisFile");

    G4VisManager* visManager = new G4VisExecutive;
    visManager -> Initialize();

    G4UIExecutive* uiExecutive = new G4UIExecutive(argc,argv,type);
    g4_info << "Initializing Geant4 run with viewer macro " << fileName << endl;
    uiManager -> ApplyCommand(command+fileName);
    uiExecutive -> SessionStart();

    delete uiExecutive;
    delete visManager;
  }
  else if (fPar->CheckPar("G4MacroFile")) {
    auto fileName = fPar -> GetParString("G4MacroFile");
    g4_info << "Initializing Geant4 run with macro " << fileName << endl;
    uiManager -> ApplyCommand(command+fileName);
  }

  fData -> WriteToFile(fProcessTable);
  fData -> WriteToFile(fSensitiveDetectors);
  fData -> WriteToFile(fVolumes);
  fData -> EndOfRun();
}

void KBG4RunManager::SetGeneratorFile(TString value)
{
  auto pga = (KBPrimaryGeneratorAction *) userPrimaryGeneratorAction;
  fPar -> ReplaceEnvironmentVariable(value);
  pga -> SetEventGenerator(value.Data());
}

void KBG4RunManager::SetOutputFile(TString value)
{
  fPar -> ReplaceEnvironmentVariable(value);
  fData = new KBMCDataManager(value.Data());
  fData -> SetPar(fPar);
  fData -> SetStepPersistency(fPar->GetParBool("MCStepPersistency"));
  fData -> SetSecondaryPersistency(fPar->GetParBool("MCSecondaryPersistency"));
  fData -> SetTrackVertexPersistency(fPar->GetParBool("MCTrackVertexPersistency"));

  TIter itDetectors(fSensitiveDetectors);
  TParameter<Int_t> *det;
  while ((det = dynamic_cast<TParameter<Int_t>*>(itDetectors())))
  {
    TString name = det -> GetName();
    Int_t copyNo = det -> GetVal();

    g4_info << "Set " << name << " " << copyNo << endl;
    fData -> SetDetector(copyNo);
  }
}

void KBG4RunManager::SetVolume(G4VPhysicalVolume *physicalVolume)
{
  TString name = physicalVolume -> GetName().data();
  Int_t copyNo = physicalVolume -> GetCopyNo();

  fVolumes -> SetPar(name, copyNo);
}

void KBG4RunManager::SetSensitiveDetector(G4VPhysicalVolume *physicalVolume)
{
  TString name = physicalVolume -> GetName().data();
  Int_t copyNo = physicalVolume -> GetCopyNo();

  fSensitiveDetectors -> SetPar(name, copyNo);
}

KBParameterContainer *KBG4RunManager::GetVolumes() { return fVolumes; }
KBParameterContainer *KBG4RunManager::GetSensitiveDetectors() { return fSensitiveDetectors; }
KBParameterContainer *KBG4RunManager::GetProcessTable()       { return fProcessTable; }
