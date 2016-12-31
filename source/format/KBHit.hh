#ifndef KBHIT_HH
#define KBHIT_HH

#include "TObject.h"
#include "TVector3.h"
#include "TMath.h"

#include <vector>
using namespace std;

class KBHit : public TObject
{
  public :
    enum KBCenterAxis { kX, kY, kZ };

    KBHit() {};
    virtual ~KBHit() {};

    virtual void Print(Option_t *option = "") const;

    void SetHitID(Int_t id);
    void SetPadID(Int_t id);
    void SetTrackID(Int_t id);
    void SetX(Double_t x);
    void SetY(Double_t y);
    void SetZ(Double_t z);
    void SetDX(Double_t dx);
    void SetDY(Double_t dy);
    void SetDZ(Double_t dz);
    void SetCharge(Double_t charge);

    Int_t GetHitID() const;
    Int_t GetPadID() const;
    Int_t GetTrackID() const;
    Double_t GetX() const;
    Double_t GetY() const;
    Double_t GetZ() const;
    TVector3 GetPosition() const;
    Double_t GetDX() const;
    Double_t GetDY() const;
    Double_t GetDZ() const;
    TVector3 GetPosSigma() const;
    Double_t GetCharge() const;

    vector<Int_t> *GetTrackCandArray();
    Int_t GetNTrackCands();
    void AddTrackCand(Int_t id);
    void RemoveTrackCand(Int_t trackID);

    void Change();
    void ChangeBack();

  protected:
    Int_t fHitID = -1;
    Int_t fPadID = -1;
    Int_t fTrackID = -1;
    Double_t fX = -999;
    Double_t fY = -999;
    Double_t fZ = -999;
    Double_t fDX = -999;
    Double_t fDY = -999;
    Double_t fDZ = -999;
    Double_t fCharge = -1;

    vector<Int_t> fTrackCandArray;  //!

    KBCenterAxis fCenterAxis = KBHit::kZ;


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
