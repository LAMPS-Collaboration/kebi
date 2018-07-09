#ifndef KBMCTRACK_HH
#define KBMCTRACK_HH

#include "TObject.h"
#include "TVector3.h"
#include "KBMCStep.hh"
#include "KBTracklet.hh"
#include <vector>
using namespace std;

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif

class KBMCTrack : public KBTracklet
{
  public:
    KBMCTrack();
    virtual ~KBMCTrack() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option="") const;

    void SetPDG(Int_t val);
    void SetPX(Double_t val);
    void SetPY(Double_t val);
    void SetPZ(Double_t val);
    void SetVX(Double_t val);
    void SetVY(Double_t val);
    void SetVZ(Double_t val);

    void SetMCTrack(Int_t trackID, Int_t parentID, Int_t pdg, Double_t px, Double_t py, Double_t pz, Double_t vx = 0, Double_t vy = 0, Double_t vz = 0);

    Int_t GetPDG() const;
    Double_t GetPX() const;
    Double_t GetPY() const;
    Double_t GetPZ() const;
    TVector3 GetMomentum() const;

    Double_t GetVX() const;
    Double_t GetVY() const;
    Double_t GetVZ() const;
    TVector3 GetPrimaryPosition() const;

    void AddStep(KBMCStep *hit);
    vector<KBMCStep *> *GetStepArray();

    virtual KBTrackFitter *CreateTrackFitter() const { return nullptr; }

    virtual TVector3 Momentum(Double_t B = 0.5) const;
    virtual TVector3 PositionAtHead() const;
    virtual TVector3 PositionAtTail() const;
    virtual Double_t TrackLength() const;

    virtual TVector3 ExtrapolateTo(TVector3 point) const;
    virtual TVector3 ExtrapolateHead(Double_t l) const;
    virtual TVector3 ExtrapolateTail(Double_t l) const;
    virtual TVector3 ExtrapolateByRatio(Double_t r) const;
    virtual TVector3 ExtrapolateByLength(Double_t l) const;

    virtual Double_t LengthAt(TVector3 point) const;

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *);
    virtual void AddToEveSet(TEveElement *);
#endif

  protected:
    Int_t fPDG;
    Double_t fPX;
    Double_t fPY;
    Double_t fPZ;
    Double_t fVX;
    Double_t fVY;
    Double_t fVZ;

    vector<KBMCStep *> fStepArray; //!
  
  ClassDef(KBMCTrack, 2)
};

#endif
