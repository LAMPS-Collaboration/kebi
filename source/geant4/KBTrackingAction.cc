#include "KBTrackingAction.hh"
#include "KBMCDataManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

void KBTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4ThreeVector momentum = track -> GetMomentum();

  if (track -> GetParentID() == 0)
    KBMCDataManager::Instance() -> AddMCTrack(track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), momentum.x(), momentum.y(), momentum.z());
}
