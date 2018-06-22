#ifndef KBMCRECOMATCHING_HH
#define KBMCRECOMATCHING_HH

#include "TObject.h"
#include "TVector3.h"

class KBMCRecoMatching : public TObject
{
  public:
    KBMCRecoMatching();
    virtual ~KBMCRecoMatching() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual void Copy (TObject &object) const;

    enum Status { kNotFound, kMatched, kFake };
    void SetIsMatched();
    void SetIsNotFound();
    void SetIsFake();
    void SetStatus(Status val);

    bool IsMatched();  ///< MC and Reco tracks are matched
    bool IsNotFound(); ///< Only MCID is set. Not found in reconstruction
    bool IsFake();     ///< Only RecoID is set. Fake track
    Status GetStatus();

    void Set(Int_t mcid, TVector3 mcp, Int_t recoid, TVector3 recop);
    void SetMCID(Int_t val);
    void SetMCMomentum(TVector3 p);
    void SetRecoID(Int_t val);
    void SetRecoMomentum(TVector3 p);

       Int_t GetMCID();
    TVector3 GetMCMomentum();
       Int_t GetRecoID();
    TVector3 GetRecoMomentum();

  private:
    Status fStatus; ///< Matching status

       Int_t fMCID;          ///< position in array of KBMCTrack 
    TVector3 fMCMomentum;    ///< Original momentum

       Int_t fRecoID;        ///< position in array of KBTrack 
    TVector3 fRecoMomentum;  ///< Reconstructed momentum

  ClassDef(KBMCRecoMatching, 1)
};

#endif
