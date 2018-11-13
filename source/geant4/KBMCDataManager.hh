#ifndef KBMCDATAMANAGER_HH
#define KBMCDATAMANAGER_HH

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"

#include "KBParameterContainerHolder.hh"
#include "KBMCTrack.hh"

#include <vector>
using namespace std;

class KBMCDataManager : public KBParameterContainerHolder
{
  public:
    static KBMCDataManager* Instance();
    KBMCDataManager(const char *name);

    virtual ~KBMCDataManager();

    void SetStepPersistency(bool persistence);

    void SetDetector(Int_t detectorID);

    void AddMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz);
    void AddTrackVertex(Double_t px, Double_t py, Double_t pz, Double_t vx, Double_t vy, Double_t vz);
    void AddMCStep(Int_t detectorID, Double_t x, Double_t y, Double_t z, Double_t t, Double_t e);

    void NextEvent();
    void EndOfRun();

  private:
    void Init();

    TString fName;
    TFile* fFile;
    TTree* fTree;

    Int_t fTrackID = 0;
    KBMCTrack *fCurrentTrack = nullptr;

    bool fStepPersistency = true;

    TClonesArray *fTrackArray;
    TObjArray *fStepArrayList;

    static KBMCDataManager* fInstance;
};

#endif
