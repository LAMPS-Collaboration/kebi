#include "KBGeoHelix.hh"
#include "TMath.h"

ClassImp(KBGeoHelix)

KBGeoHelix::KBGeoHelix()
{
}

KBGeoHelix::KBGeoHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
                       Double_t t, Double_t h, kbaxis axis)
{
  SetHelix(i,j,r,s,k,t,h,axis);
}

void KBGeoHelix::SetHelix(Double_t i, Double_t j, Double_t r, Double_t s, Double_t k,
                          Double_t t, Double_t h, kbaxis axis)
{
  fI = i;
  fJ = j;
  fR = r;
  fS = s;
  fK = k;
  fT = t;
  fH = h;
  fAxis = axis;
}


void KBGeoHelix::SetI(Double_t val)         { fI = val; }
void KBGeoHelix::SetJ(Double_t val)         { fJ = val; }
void KBGeoHelix::SetR(Double_t val)         { fR = val; }
void KBGeoHelix::SetS(Double_t val)         { fS = val; }
void KBGeoHelix::SetK(Double_t val)         { fK = val; }
void KBGeoHelix::SetAlphaTail(Double_t val) { fT = val; }
void KBGeoHelix::SetAlphaHead(Double_t val) { fH = val; }
void KBGeoHelix::SetAxis(kbaxis val)        { fAxis = val; }

Double_t KBGeoHelix::GetI()         const { return fI; }
Double_t KBGeoHelix::GetJ()         const { return fJ; }
Double_t KBGeoHelix::GetR()         const { return fR; }
Double_t KBGeoHelix::GetS()         const { return fS; }
Double_t KBGeoHelix::GetK()         const { return fK; }
Double_t KBGeoHelix::GetAlphaTail() const { return fT; }
Double_t KBGeoHelix::GetAlphaHead() const { return fH; }
kbaxis   KBGeoHelix::GetAxis()      const { return fAxis; }

Double_t KBGeoHelix::DipAngle()      const { return TMath::ATan(fS/fR); }
Double_t KBGeoHelix::AngleFromAxis() const { return TMath::Pi()/2 - DipAngle(); }

   Int_t KBGeoHelix::Helicity() const { return 1; } //@todo 
Double_t KBGeoHelix::LengthInPeriod()  const { return 2*TMath::Pi()*fR/TMath::Cos(DipAngle()); }
Double_t KBGeoHelix::KLengthInPeriod() const { return TMath::Abs(2*TMath::Pi()*fS); }

Double_t KBGeoHelix::LengthThroughAlpha(Double_t alpha) const
{
  return alpha*fR/TMath::Cos(DipAngle());
}

Double_t KBGeoHelix::AlphaThroughLength(Double_t length) const
{
  return length*TMath::Cos(DipAngle())/fR;
}

TVector3 KBGeoHelix::PositionByAlpha(Double_t alpha) const
{
  KBVector3 pos(fAxis);
  pos.SetIJK(fR*TMath::Cos(alpha)+fI, fR*TMath::Sin(alpha)+fJ, alpha*fS+fK); 
  return pos.GetXYZ();
}

TVector3 KBGeoHelix::Direction(Double_t alpha) const
{
  KBVector3 posCenter(fAxis,fI,fJ,0);
  Double_t alphaPointer = alpha;

  if (fT<fH) alphaPointer += TMath::Pi()/2.;
  else       alphaPointer -= TMath::Pi()/2.;

  KBVector3 posPointer(PositionByAlpha(alphaPointer), fAxis);
  KBVector3 direction = posPointer - posCenter;
  auto directionz = direction.Z();
  direction.SetK(0);
  direction.SetMag(2*TMath::Pi()*fR);

  if (directionz > 0) direction.SetK(+KLengthInPeriod());
  else                direction.SetK(-KLengthInPeriod());

  auto unitDirection = direction.GetXYZ().Unit();

  return unitDirection;
}
