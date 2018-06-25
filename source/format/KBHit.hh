#ifndef KBHIT_HH
#define KBHIT_HH

#include "KBWPointCluster.hh"
#include "KBContainer.hh"

#ifdef ACTIVATE_EVE
#include "TEveElement.h"
#endif
#include "TVector3.h"
#include "TMath.h"
#include "TF1.h"

#include <vector>
using namespace std;

class KBHit : public KBWPointCluster
{
  protected:
    Int_t fHitID = -1;
    Int_t fTrackID = -1;

    vector<Int_t> fTrackCandArray;  //!

  public :
    KBHit() { Clear(); }
    KBHit(Double_t x, Double_t y, Double_t z, Double_t q) { Clear(); Set(x,y,z,q); }
    virtual ~KBHit() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;
    virtual void Copy (TObject &object) const;

    void SetHitID(Int_t id);
    void SetTrackID(Int_t id);
    void SetX(Double_t x);
    void SetY(Double_t y);
    void SetZ(Double_t z);
    void SetDX(Double_t dx);
    void SetDY(Double_t dy);
    void SetDZ(Double_t dz);
    void SetCharge(Double_t charge);

    virtual void AddHit(KBHit *hit);

    Int_t GetHitID() const;
    Int_t GetTrackID() const;
    Double_t GetX() const;
    Double_t GetY() const;
    Double_t GetZ() const;
    Double_t GetDX() const;
    Double_t GetDY() const;
    Double_t GetDZ() const;
    Double_t GetCharge() const;

    vector<Int_t> *GetTrackCandArray();
    Int_t GetNumTrackCands();
    void AddTrackCand(Int_t id);
    void RemoveTrackCand(Int_t trackID);

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *);
    virtual void AddToEveSet(TEveElement *eveSet);
#endif

  ClassDef(KBHit, 1)
};

class KBHitSortDirection {
  public:
    KBHitSortDirection(TVector3 p):fP(p) {}
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Dot(fP) > h2 -> GetPosition().Dot(fP); }
  private:
    TVector3 fP;
};

class KBHitSortThetaFromP {
  public:
    KBHitSortThetaFromP(TVector3 p):fP(p) {}
    bool operator() (KBHit* h1, KBHit* h2) {
      fP1 = h1 -> GetPosition() - fP;
      fP2 = h2 -> GetPosition() - fP;
      return TMath::ATan2(fP1.Z(),fP1.X()) > TMath::ATan2(fP2.Z(),fP2.X());
    }
  private:
    TVector3 fP, fP1, fP2;
};

class KBHitSortTheta {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { 
      Double_t t1 = TMath::ATan2(h1 -> GetPosition().Z(), h1 -> GetPosition().X());
      Double_t t2 = TMath::ATan2(h2 -> GetPosition().Z(), h2 -> GetPosition().X());
      return t1 > t2;
    }
};

class KBHitSortThetaInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      Double_t t1 = TMath::ATan2(h1 -> GetPosition().Z(), h1 -> GetPosition().X());
      Double_t t2 = TMath::ATan2(h2 -> GetPosition().Z(), h2 -> GetPosition().X());
      return t1 < t2;
    }
};

class KBHitSortR {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Mag() > h2 -> GetPosition().Mag(); }
};

class KBHitSortRInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Mag() < h2 -> GetPosition().Mag(); }
};

class KBHitSortRInvFromP {
  public:
    KBHitSortRInvFromP(TVector3 p):fP(p) {}
    bool operator() (KBHit* h1, KBHit* h2){
      fP1 = h1 -> GetPosition() - fP;
      fP2 = h2 -> GetPosition() - fP;
      if (fP1.Mag() == fP2.Mag()) {
        if (fP1.X() == fP2.X()) {
          if (fP1.Y() == fP2.Y()) {
            return fP1.Z() > fP2.Z();
          } return fP1.Y() > fP2.Z();
        } return fP1.X() > fP2.X();
      } return fP1.Mag() < fP2.Mag(); 
    }

  private:
    TVector3 fP;
    TVector3 fP1;
    TVector3 fP2;
};

class KBHitSortZ {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Z() > h2 -> GetPosition().Z(); }
};

class KBHitSortZInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Z() < h2 -> GetPosition().Z(); }
};

class KBHitSortX {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().X() > h2 -> GetPosition().X(); }
};

class KBHitSortXInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().X() < h2 -> GetPosition().X(); }
};

class KBHitSortY {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Y() > h2 -> GetPosition().Y(); }
};

class KBHitSortYInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetPosition().Y() < h2 -> GetPosition().Y(); }
};

class KBHitSortCharge {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      if (h1 -> GetCharge() == h2 -> GetCharge())
        return h1 -> GetY() > h2 -> GetY();
      return h1 -> GetCharge() > h2 -> GetCharge();
    }
};

class KBHitSortChargeInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) { return h1 -> GetCharge() < h2 -> GetCharge(); }
};

class KBHitSortXYZInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      if (h1 -> GetPosition().Z() == h2 -> GetPosition().Z()) {
        if (h1 -> GetPosition().X() == h2 -> GetPosition().X())
          return h1 -> GetPosition().Y() < h2 -> GetPosition().Y(); 
        else 
          return h1 -> GetPosition().X() < h2 -> GetPosition().X(); 
      }
      return h1 -> GetPosition().Z() < h2 -> GetPosition().Z(); 
    }
};

class KBHitSortRho {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      Double_t rho1 = (h1 -> GetPosition().X() * h1 -> GetPosition().X() + h1 -> GetPosition().Z() * h1 -> GetPosition().Z());
      Double_t rho2 = (h2 -> GetPosition().X() * h2 -> GetPosition().X() + h2 -> GetPosition().Z() * h2 -> GetPosition().Z());
      return rho1 > rho2;
    }
};

class KBHitSortRhoInv {
  public:
    bool operator() (KBHit* h1, KBHit* h2) {
      Double_t rho1 = (h1 -> GetPosition().X() * h1 -> GetPosition().X() + h1 -> GetPosition().Z() * h1 -> GetPosition().Z());
      Double_t rho2 = (h2 -> GetPosition().X() * h2 -> GetPosition().X() + h2 -> GetPosition().Z() * h2 -> GetPosition().Z());
      return rho1 < rho2;
    }
};

class KBHitSortByDistanceTo {
  public:
    KBHitSortByDistanceTo(TVector3 p):fP(p) {}
    bool operator() (KBHit* a, KBHit* b) { return (a->GetPosition()-fP).Mag() < (b->GetPosition()-fP).Mag(); }
  private:
    TVector3 fP;
};

#endif
