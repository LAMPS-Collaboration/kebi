#include "KBG4RunManager.hh"
#include "KBMCDataManager.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "globals.hh"

KBG4RunManager::KBG4RunManager()
:G4RunManager()
{
  fMessenger = new KBG4RunMessenger(this);
}

KBG4RunManager::KBG4RunManager(const char *name)
:KBG4RunManager()
{
  auto data = new KBMCDataManager(name);
  data -> SetParameterContainer(fPar);
}

KBG4RunManager::~KBG4RunManager()
{
  KBMCDataManager::Instance() -> EndOfRun();
  delete fMessenger;
}

void KBG4RunManager::Initialize()
{
  G4RunManager::Initialize();

  SetOutputFile(fPar->GetParString("G4OutputFile").Data());
  SetGeneratorFile(fPar->GetParString("G4InputFile").Data());
}

void KBG4RunManager::Run(G4int argc, char **argv, const G4String &type)
{
  G4UImanager* uiManager = G4UImanager::GetUIpointer();
  TString command("/control/execute ");

  if (fPar->CheckPar("G4VisFile")) {
    G4VisManager* visManager = new G4VisExecutive;
    visManager -> Initialize();

    G4UIExecutive* uiExecutive = new G4UIExecutive(argc,argv,type);
    auto fileName = fPar -> GetParString("G4VisFile");
    uiManager -> ApplyCommand(command+fileName);
    uiExecutive -> SessionStart();

    delete uiExecutive;
    delete visManager;
  }
  else if (fPar->CheckPar("G4MacroFile")) {
    auto fileName = fPar -> GetParString("G4MacroFile");
    uiManager -> ApplyCommand(command+fileName);
  }
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
  auto data = new KBMCDataManager(value.Data());
  data -> SetParameterContainer(fPar);
  data -> SetStepPersistency(fPar->GetParBool("MCStepPersistency"));

  for (auto copyNo : fCopyNoArray) {
    G4cout << "Set detector " << copyNo << G4endl;
    data -> SetDetector(copyNo);
  }
}

void KBG4RunManager::SetSensitiveDetector(G4VPhysicalVolume *physicalVolume)
{
  fCopyNoArray.push_back(physicalVolume->GetCopyNo());
}
