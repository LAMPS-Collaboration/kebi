#ifndef KBMCDATAMANAGER_HH
#define KBMCDATAMANAGER_HH

#include "TFile.h"
#include "TTree.h"
#include "TClonesArray.h"

class KBMCDataManager
{
  public:
    static KBMCDataManager* Instance();
    KBMCDataManager(const char *name);

    virtual ~KBMCDataManager();

    void AddMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz);
    void AddMCStep(Double_t x, Double_t y, Double_t z, Double_t t, Double_t e);

    void NextEvent();
    void EndOfRun();

  private:
    void Init();

    TString fName;

    TFile* fFile;
    TTree* fTree;

    Int_t fTrackID = 0;

    TClonesArray *fTrackArray;
    TClonesArray *fStepArray;

    static KBMCDataManager* fInstance;
};

#endif
