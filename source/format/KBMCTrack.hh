#ifndef KBMCTRACK_HH
#define KBMCTRACK_HH

#include "TObject.h"
#include "TVector3.h"
#include "KBMCStep.hh"
#include <vector>
using namespace std;

class KBMCTrack : public TObject
{
  public:
    KBMCTrack();
    virtual ~KBMCTrack();

    virtual void Clear(Option_t *option = "");

    void SetTrackID(Int_t val);
    void SetParentID(Int_t val);
    void SetPDG(Int_t val);
    void SetPX(Double_t val);
    void SetPY(Double_t val);
    void SetPZ(Double_t val);

    void SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz);

    Int_t GetTrackID()  const;
    Int_t GetParentID() const;
    Int_t GetPDG()      const;
    Double_t GetPX()    const;
    Double_t GetPY()    const;
    Double_t GetPZ()    const;
    TVector3 GetMomentum() const;

    void AddStep(KBMCStep *hit);
    vector<KBMCStep *> *GetStepArray();

  private:
    Int_t fTrackID;
    Int_t fParentID;
    Int_t fPDG;
    Double_t fPX;
    Double_t fPY;
    Double_t fPZ;

    vector<KBMCStep *> fStepArray; //!
  
  ClassDef(KBMCTrack, 1)
};

#endif
