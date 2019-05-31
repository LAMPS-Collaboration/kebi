#include "KBG4RunManager.hh"
#include "KBMCDataManager.hh"
#include "KBMCStep.hh"
#include "globals.hh"

KBMCDataManager* KBMCDataManager::fInstance = 0;
KBMCDataManager* KBMCDataManager::Instance() { return fInstance; }

KBMCDataManager::KBMCDataManager(const char *name)
{
  fName = name;
  fInstance = this;

  Init();
}

KBMCDataManager::~KBMCDataManager()
{
}

void KBMCDataManager::SetStepPersistency(bool v) { fStepPersistency = v; }
void KBMCDataManager::SetSecondaryPersistency(bool v) { fSecondaryPersistency = v; }
void KBMCDataManager::SetTrackVertexPersistency(bool v) { fTrackVertexPersistency = v; }

void KBMCDataManager::Init()
{
  fFile = new TFile(fName,"recreate");

  fTrackArray = new TClonesArray("KBMCTrack", 100);

  fTree = new TTree("event", fName);
  fTree -> Branch("MCTrack", &fTrackArray);

  fStepArrayList = new TObjArray();
}

void KBMCDataManager::SetDetector(Int_t detectorID)
{
  if (!fStepPersistency) return;
  auto stepArray = new TClonesArray("KBMCStep", 10000);
  stepArray -> SetName(Form("MCStep%d", detectorID));

  fTree -> Branch(stepArray -> GetName(), &stepArray);
  fStepArrayList -> Add(stepArray);
}

void KBMCDataManager::AddMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz, Int_t processID)
{
  if (parentID != 0 && !fSecondaryPersistency) {
    fCurrentTrack = nullptr;
    return;
  }

  fTrackID = trackID;
  fCurrentTrack = (KBMCTrack *) fTrackArray -> ConstructedAt(fTrackArray -> GetEntriesFast());
  fCurrentTrack -> SetMCTrack(trackID, parentID, pdg, px, py, pz, detectorID, vx, vy, vz, processID);
}

void KBMCDataManager::AddTrackVertex(Double_t px, Double_t py, Double_t pz, Int_t detectorID, Double_t vx, Double_t vy, Double_t vz)
{
  if (fCurrentTrack == nullptr || !fTrackVertexPersistency)
    return;

  fCurrentTrack -> AddVertex(px, py, pz, detectorID, vx, vy, vz);
}

void KBMCDataManager::AddMCStep(Int_t detectorID, Double_t x, Double_t y, Double_t z, Double_t t, Double_t e)
{
  if (!fStepPersistency) return;
  auto stepArray = (TClonesArray *) fStepArrayList -> FindObject(Form("MCStep%d", detectorID));
  if (stepArray == nullptr)
    return;

  KBMCStep *step = (KBMCStep *) stepArray -> ConstructedAt(stepArray -> GetEntriesFast());
  step -> SetMCStep(fTrackID, x, y, z, t, e);
}

void KBMCDataManager::NextEvent()
{ 
  g4_info << "End of Event " << fTree -> GetEntries() << endl;
  fTree -> Fill();

  fTrackArray -> Clear("C");
  TIter it(fStepArrayList);

  if (fStepPersistency) {
    while (auto stepArray = (TClonesArray *) it.Next())
      stepArray -> Clear("C");
  }
}

void KBMCDataManager::WriteToFile(TObject *obj)
{
  fFile -> cd();
  g4_info << "Writing " << obj -> GetName() << " to output file" << endl;
  obj -> Write(obj->GetName(),TObject::kSingleKey);
}

void KBMCDataManager::EndOfRun() 
{ 
  fFile -> cd();
  g4_info << "Writing " << fTree -> GetName() << " to output file" << endl;
  fTree -> Write(); 
  g4_info << "Writing " << fPar -> GetName() << " to output file" << endl;
  fPar -> Write(fPar->GetName(),TObject::kSingleKey);
  fFile -> Close();
  g4_info << "Output: " << fFile -> GetName() << endl;
}
