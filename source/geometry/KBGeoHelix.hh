#ifndef KBGEOHELIX_HH
#define KBGEOHELIX_HH

#include "TVector3.h"
#include "TGraph2D.h"
#include "TGraph.h"
#include "KBGeometry.hh"

typedef KBVector3::Axis kbaxis;

/// 
class KBGeoHelix : public KBGeometry
{
  protected:
    Double_t fI = 0; ///< axis i-position
    Double_t fJ = 0; ///< axis j-position
    Double_t fR = 0; ///< Helix radius
    Double_t fS = 0; ///< k = fS * alpha + fK
    Double_t fK = 0; ///< k = fS * alpha + fK
    Double_t fT = 0; ///< Alpha angle at tail
    Double_t fH = 0; ///< Alpha angle at head

    kbaxis fAxis = KBVector3::kZ; ///< helix axis

  public:
    KBGeoHelix();
    KBGeoHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
               Double_t t, Double_t h, kbaxis axis = KBVector3::kZ);

    virtual ~KBGeoHelix() {}

    void SetHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
                  Double_t t, Double_t h, kbaxis axis = KBVector3::kZ);

    void SetI(Double_t val);
    void SetJ(Double_t val);
    void SetR(Double_t val);
    void SetS(Double_t val);
    void SetK(Double_t val);
    void SetAlphaTail(Double_t val);
    void SetAlphaHead(Double_t val);
    void SetAxis(kbaxis val);

    Double_t GetI()         const;
    Double_t GetJ()         const;
    Double_t GetR()         const;
    Double_t GetS()         const;
    Double_t GetK()         const;
    Double_t GetAlphaTail() const;
    Double_t GetAlphaHead() const;
    kbaxis   GetAxis()      const;

    Double_t DipAngle() const;
    Double_t AngleFromAxis() const;
       Int_t Helicity() const;                          ///< Helicity of track +/-
    Double_t LengthInPeriod() const;                    ///< Length of track in one period
    Double_t KLengthInPeriod() const;                   ///< y-length of track in one period
    Double_t LengthThroughAlpha(Double_t alpha) const;  ///< Length of track in change of alpha
    Double_t AlphaThroughLength(Double_t length) const; ///< Angle alpha in change of length
    TVector3 PositionByAlpha(Double_t alpha) const;     ///< Position at angle alpha [mm]
    TVector3 Direction(Double_t alpha) const;           ///< Direction at angle alpha

    /*
    TVector3 HelicoidMapping(TVector3 pos); ///< Map pos to helicoid map coordinate

    //TVector3 ClosestPointToHelix(TVector3);
    //Double_t DistanceToHelix(TVector3);

    KBGeoCircle CircleProjection();
    //KBGeoLine   HelicoidMapLine();

    TGraph2D *DrawHelix();
    TGraph   *Draw2D(kbaxis, kbaxis);
    TGraph   *DrawIJ();
    TGraph   *DrawKI();
    TGraph   *DrawKJ();
    */

  ClassDef(KBGeoHelix, 1)
};

#endif
