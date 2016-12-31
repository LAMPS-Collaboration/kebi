#include "KBMCDataManager.hh"
#include "KBMCTrack.hh"
#include "KBMCStep.hh"
#include "globals.hh"

KBMCDataManager* KBMCDataManager::fInstance = 0;
KBMCDataManager* KBMCDataManager::Instance() { return fInstance; }

KBMCDataManager::KBMCDataManager(TString name)
{
  fName = name;
  fInstance = this;

  Init();
}

KBMCDataManager::~KBMCDataManager()
{
}

void KBMCDataManager::Init()
{
  fFile = new TFile(fName,"recreate");

  fTrackArray = new TClonesArray("KBMCTrack", 100);
  fStepArray = new TClonesArray("KBMCStep", 10000);

  fTree = new TTree("data", fName);
  fTree -> Branch("MCTrack", &fTrackArray);
  fTree -> Branch("MCStep",  &fStepArray);
}

void KBMCDataManager::AddMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz)
{
  fTrackID = trackID;
  G4cout << fTrackArray -> GetEntries() << G4endl;
  KBMCTrack *track = (KBMCTrack *) fTrackArray -> ConstructedAt(fTrackArray -> GetEntries());
  track -> SetMCTrack(trackID, parentID, pdg, px, py, pz);
}

void KBMCDataManager::AddMCStep(Double_t x, Double_t y, Double_t z, Double_t t, Double_t e)
{
  KBMCStep *step = (KBMCStep *) fStepArray -> ConstructedAt(fStepArray -> GetEntries());
  step -> SetMCStep(fTrackID, x, y, z, t, e);
}

void KBMCDataManager::NextEvent()
{ 
  fTree -> Fill();

  fTrackArray -> Clear();
  fStepArray -> Clear();
}

void KBMCDataManager::EndOfRun() 
{ 
  fFile -> cd();
  fTree -> Write(); 
  fFile -> Close();
}
