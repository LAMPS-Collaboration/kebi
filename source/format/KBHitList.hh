#ifndef KBHITLIST_HH
#define KBHITLIST_HH

#include "TObject.h"
#include <vector>
#include <iomanip>
using namespace std;

#include "KBODRFitter.hh"
#include "KBGeoLine.hh"
#include "KBGeoPlane.hh"
#include "KBGeoCircle.hh"
#include "KBGeoHelix.hh"

class KBHit;

typedef KBVector3::Axis kbaxis;

class KBHitList : public TObject
{
  private:
    vector<KBHit*> fHitArray; //!
    vector<Int_t> fHitIDArray;

       Int_t fN = 0;  ///< Number of hits
    Double_t fW = 0;  ///< Sum of charge
    Double_t fEX  = 0;  //! < <x>   Expectation value of x
    Double_t fEY  = 0;  //! < <y>   Expectation value of y
    Double_t fEZ  = 0;  //! < <z>   Expectation value of z
    Double_t fEXX = 0;  //! < <x*x> Expectation value of x*x
    Double_t fEYY = 0;  //! < <y*y> Expectation value of y*y
    Double_t fEZZ = 0;  //! < <z*z> Expectation value of z*z
    Double_t fEXY = 0;  //! < <x*y> Expectation value of x*y
    Double_t fEYZ = 0;  //! < <y*z> Expectation value of y*z
    Double_t fEZX = 0;  //! < <z*x> Expectation value of z*y

    KBODRFitter *fODRFitter = nullptr;

  public:
    KBHitList();
    virtual ~KBHitList() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "at") const;

    KBGeoLine FitLine();
    KBGeoPlane FitPlane();
    KBGeoCircle FitCircle(kbaxis ref = KBVector3::kZ);

    void AddHit(KBHit *hit);
    void AddHit(Double_t x, Double_t y, Double_t z, Double_t q);
    bool RemoveHit(KBHit *hit);

    vector<KBHit*> *GetHitArray();
    vector<Int_t> *GetHitIDArray();

    Int_t GetNumHits() const;
    KBHit *GetHit(Int_t idx) const;
    Int_t GetHitID(Int_t idx) const;

    Double_t GetW() const;
    Double_t GetChargeSum() const;

    Double_t GetXMean() const;
    Double_t GetYMean() const;
    Double_t GetZMean() const;
    Double_t GetExpectationX()  const;
    Double_t GetExpectationY()  const;
    Double_t GetExpectationZ()  const;

    Double_t GetCovWXX() const; ///< SUM_i {(z_centroid-z_i)*(z_centroid-z_i) }
    Double_t GetCovWYY() const; ///< SUM_i {(x_centroid-x_i)*(x_centroid-x_i) }
    Double_t GetCovWZZ() const; ///< SUM_i {(y_centroid-y_i)*(y_centroid-y_i) }
    Double_t GetCovWXY() const; ///< SUM_i {(z_centroid-z_i)*(x_centroid-x_i) }
    Double_t GetCovWYZ() const; ///< SUM_i {(x_centroid-x_i)*(y_centroid-y_i) }
    Double_t GetCovWZX() const; ///< SUM_i {(y_centroid-y_i)*(z_centroid-z_i) }

    Double_t GetVarX() const;
    Double_t GetVarY() const;
    Double_t GetVarZ() const;

    Double_t GetExpectationXX() const;
    Double_t GetExpectationYY() const;
    Double_t GetExpectationZZ() const;
    Double_t GetExpectationXY() const;
    Double_t GetExpectationYZ() const;
    Double_t GetExpectationZX() const;

    TVector3 GetMean()          const;
    TVector3 GetExpectation()   const;
    TVector3 GetCovWD()         const;
    TVector3 GetCovWO()         const;
    TVector3 GetVar()           const;
    TVector3 GetExpectation2D() const;
    TVector3 GetExpectation2O() const;

    KBVector3 GetMean(kbaxis ref)          const;
    KBVector3 GetExpectation(kbaxis ref)   const;
    KBVector3 GetCovWD(kbaxis ref)         const;
    KBVector3 GetCovWO(kbaxis ref)         const;
    KBVector3 GetVar(kbaxis ref)           const;
    KBVector3 GetExpectation2D(kbaxis ref) const;
    KBVector3 GetExpectation2O(kbaxis ref) const;


    ClassDef(KBHitList, 1)
};

#endif
