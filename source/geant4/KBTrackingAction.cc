#include "KBTrackingAction.hh"
#include "KBMCDataManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"

void KBTrackingAction::PreUserTrackingAction(const G4Track* track)
{
  G4ThreeVector momentum = track -> GetMomentum();
  G4ThreeVector position = track -> GetPosition();
  G4int volumeID = track -> GetVolume() -> GetCopyNo();

  KBMCDataManager::Instance() -> AddMCTrack(track -> GetTrackID(), track -> GetParentID(), track -> GetDefinition() -> GetPDGEncoding(), momentum.x(), momentum.y(), momentum.z(), volumeID, position.x(), position.y(), position.z());
}
