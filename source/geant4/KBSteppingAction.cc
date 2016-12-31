#include "KBSteppingAction.hh"
#include "KBMCDataManager.hh"

#include "G4Event.hh"
#include "G4RunManager.hh"

KBSteppingAction::KBSteppingAction()
: G4UserSteppingAction()
{
  fDetectorName = "TPC";
}

void KBSteppingAction::SetDetectorName(G4String detectorName)
{
  fDetectorName = detectorName;
}

void KBSteppingAction::UserSteppingAction(const G4Step* step)
{
  G4StepStatus stat = step -> GetPostStepPoint() -> GetStepStatus();
  if (stat == fWorldBoundary) 
    return;

  G4double edep = step -> GetTotalEnergyDeposit(); 
  if (edep <= 0)
    return;

  G4String volume = step -> GetPreStepPoint() -> GetPhysicalVolume() -> GetName();
  if (volume != fDetectorName)
    return;

  G4double time = step -> GetPreStepPoint() -> GetGlobalTime();
  G4ThreeVector pos = step -> GetPreStepPoint() -> GetPosition();

  KBMCDataManager::Instance() -> AddMCStep(pos.x(), pos.y(), pos.z(), time, edep);
}
