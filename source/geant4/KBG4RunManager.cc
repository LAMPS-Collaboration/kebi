#include "KBG4RunManager.hh"
#include "KBMCDataManager.hh"
#include "KBPrimaryGeneratorAction.hh"
#include "TString.h"

KBG4RunManager::KBG4RunManager()
:G4RunManager()
{
  fMessenger = new KBG4RunMessenger(this);
  CreateParameterContainer();
}

KBG4RunManager::KBG4RunManager(const char *name)
:KBG4RunManager()
{
  new KBMCDataManager(name);
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
  new KBMCDataManager(s.Data());
}

void KBG4RunManager::AddParFile(G4String value)
{
  TString s = value;
  fPar -> ReplaceEnvironmentVariable(s);
  fPar -> AddFile(s.Data());
}
