#include "KBG4RunManager.hh"
#include "KBMCDataManager.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "TString.h"
#include "globals.hh"

KBG4RunManager::KBG4RunManager()
:G4RunManager()
{
  fMessenger = new KBG4RunMessenger(this);
  CreateParameterContainer();
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

void KBG4RunManager::SetGeneratorFile(G4String value)
{
  auto pga = (KBPrimaryGeneratorAction *) userPrimaryGeneratorAction;
  TString s = value;
  fPar -> ReplaceEnvironmentVariable(s);
  pga -> SetEventGenerator(s.Data());
}

void KBG4RunManager::SetOutputFile(G4String value)
{
  TString s = value;
  fPar -> ReplaceEnvironmentVariable(s);
  auto data = new KBMCDataManager(s.Data());
  data -> SetParameterContainer(fPar);

  for (auto copyNo : fCopyNoArray) {
    G4cout << "Set detector " << copyNo << G4endl;
    data -> SetDetector(copyNo);
  }
}

void KBG4RunManager::AddParFile(G4String value)
{
  TString s = value;
  fPar -> ReplaceEnvironmentVariable(s);
  fPar -> AddFile(s.Data());
}

void KBG4RunManager::SetSensitiveDetector(G4PVPlacement *pvp)
{
  fCopyNoArray.push_back(pvp->GetCopyNo());
}
