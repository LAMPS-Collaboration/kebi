#ifndef KBVECTOR3_HH
#define KBVECTOR3_HH

#include "TVector3.h"
#include <map>

class KBVector3 : public TVector3
{
  public:
    enum Axis { kNon=0, kX=1, kY=2, kZ=3, kMX=4, kMY=5, kMZ=6, kI=7, kJ=8, kK=9 }; 

    friend std::ostream& operator<<(std::ostream& out, const Axis value) {
      static std::map<Axis, std::string> axisNames;
      if (axisNames.size() == 0) {
        axisNames[kNon] = "not-defined";
        axisNames[kX]   = "x";
        axisNames[kY]   = "y";
        axisNames[kZ]   = "z";
        axisNames[kI]   = "i";
        axisNames[kJ]   = "j";
        axisNames[kK]   = "z";
        axisNames[kMX]  = "-x";
        axisNames[kMY]  = "-y";
        axisNames[kMZ]  = "-z";
      }
      return out << axisNames[value];
    }

    static Axis GetAxis(TString name) {
      name.ToLower();
           if (name == "x")  return kX;
      else if (name == "y")  return kY;
      else if (name == "z")  return kZ;
      else if (name == "i")  return kI;
      else if (name == "j")  return kJ;
      else if (name == "z")  return kK;
      else if (name == "-x") return kMX;
      else if (name == "-y") return kMY;
      else if (name == "-z") return kMZ;
      else                   return kNon;
    }

    static TString AxisName(Axis value) {
      static std::map<Axis, std::string> axisNames;
      if (axisNames.size() == 0) {
        axisNames[kNon] = "not-defined";
        axisNames[kX]   = "x";
        axisNames[kY]   = "y";
        axisNames[kZ]   = "z";
        axisNames[kI]   = "i";
        axisNames[kJ]   = "j";
        axisNames[kK]   = "z";
        axisNames[kMX]  = "-x";
        axisNames[kMY]  = "-y";
        axisNames[kMZ]  = "-z";
      }
      return TString(axisNames[value].c_str());
    }


    KBVector3()
    :TVector3() { Clear(); }

    KBVector3(Axis referenceAxis)
    :TVector3(), fReferenceAxis(referenceAxis) {}

    KBVector3(const KBVector3 &p)
    :TVector3(p), fReferenceAxis(p.fReferenceAxis) {}

    KBVector3(const TVector3 &p, Axis referenceAxis = kNon)
    :TVector3(p), fReferenceAxis(referenceAxis) {}

    KBVector3(Double_t x0, Double_t y0, Double_t z0, Axis referenceAxis = kNon)
    :TVector3(x0,y0,z0), fReferenceAxis(referenceAxis) {}

    KBVector3(Axis referenceAxis, Double_t i0, Double_t j0, Double_t k0)
    :fReferenceAxis(referenceAxis) { SetIJK(i0,j0,k0); }

    KBVector3(Axis referenceAxis, TVector3 p)
    :fReferenceAxis(referenceAxis) { SetIJK(p.X(),p.Y(),p.Z()); }

    virtual ~KBVector3() {}

    inline KBVector3 operator - () const {
      return KBVector3(-X(), -Y(), -Z(), fReferenceAxis);
    }

    inline KBVector3 & operator = (const KBVector3 & p) {
      SetX(p.X());
      SetY(p.Y());
      SetZ(p.Z());
      fReferenceAxis = p.fReferenceAxis;
      return *this;
    }

    virtual void Print(Option_t *option = "") const;
    virtual void Clear(Option_t *option = "");

    void SetReferenceAxis(Axis referenceAxis);
    Axis GetReferenceAxis() const;

    Double_t At(Axis ka) const;
    void AddAt(Double_t value, Axis ka, bool ignoreNegative = false);

    void SetIJKR(Double_t i, Double_t j, Double_t k, Axis referenceAxis);
    void SetIJK(Double_t i, Double_t j, Double_t k);
    void SetI(Double_t i);
    void SetJ(Double_t j);
    void SetK(Double_t k);

    Double_t I() const;
    Double_t J() const;
    Double_t K() const;

    TVector3 GetXYZ();
    TVector3 GetIJK();

  private:
    Axis fReferenceAxis = KBVector3::kNon;

  ClassDef(KBVector3, 1)
};

KBVector3 operator + (const KBVector3 &a, const KBVector3 &b);
KBVector3 operator - (const KBVector3 &a, const KBVector3 &b);

KBVector3 operator * (Double_t a, const KBVector3 &p);
KBVector3 operator * (const KBVector3 &p, Double_t a);
KBVector3 operator * (Int_t a, const KBVector3 &p);
KBVector3 operator * (const KBVector3 &p, Int_t a);

#endif
