#include "KBEventAction.hh"
#include "KBMCDataManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

void KBEventAction::EndOfEventAction(const G4Event*)
{
  KBMCDataManager::Instance() -> NextEvent();
}
