#include "KBSteppingAction.hh"
#include "KBMCDataManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

KBSteppingAction::KBSteppingAction()
: G4UserSteppingAction()
{
}

void KBSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();

  G4ThreeVector pos = step -> GetTrack() -> GetPosition();
  G4ThreeVector mom = step -> GetTrack() -> GetMomentum();

  if (stat == fWorldBoundary) {
    KBMCDataManager::Instance() -> AddTrackVertex(mom.x(),mom.y(),mom.z(),pos.x(),pos.y(),pos.z());
    return;
  }

  G4int preNo = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetCopyNo();
  G4int postNo = step -> GetPostStepPoint() -> GetPhysicalVolume() -> GetCopyNo();

  G4ThreeVector prePos = step -> GetPreStepPoint() -> GetPosition();
  G4ThreeVector postPos = step -> GetPostStepPoint() -> GetPosition();

  G4ThreeVector trackPos = step -> GetTrack() -> GetPosition();

  if (preNo != postNo)
    KBMCDataManager::Instance() -> AddTrackVertex(mom.x(),mom.y(),mom.z(),pos.x(),pos.y(),pos.z());

  G4double edep = step -> GetTotalEnergyDeposit(); 
  if (edep <= 0)
    return;

  G4double time = step -> GetPreStepPoint() -> GetGlobalTime();

  KBMCDataManager::Instance() -> AddMCStep(preNo, prePos.x(), prePos.y(), prePos.z(), time, edep);
}
