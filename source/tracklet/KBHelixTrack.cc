#include "KBHelixTrack.hh"

#include <iostream>
using namespace std;

ClassImp(KBHelixTrack)

KBHelixTrack::KBHelixTrack()
{
  Clear();
}

KBHelixTrack::KBHelixTrack(Int_t id)
{
  Clear();
  fTrackID = id;
}

void KBHelixTrack::Clear(Option_t *option)
{
  fTrackID  = -999;
  fParentID = -999;

  fFitStatus = kBad;

  fXHelixCenter = -999;
  fZHelixCenter = -999;
  fHelixRadius  = -999;
  fYInitial     = -999;
  fAlphaSlope   = -999;

  fChargeSum = 0;

  fExpectationX = 0;
  fExpectationY = 0;
  fExpectationZ = 0;
  fExpectationXX = 0;
  fExpectationYY = 0;
  fExpectationZZ = 0;
  fExpectationXY = 0;
  fExpectationYZ = 0;
  fExpectationZX = 0;

  fRMSW = -999;
  fRMSH = -999;

  fAlphaHead = -999;
  fAlphaTail = -999;

  fIsPositiveChargeParticle = true;

  if (TString(option) == "C")
    DeleteHits();
  else {
    fMainHits.clear();
    fCandHits.clear();
  }

  fMainHitIDs.clear();

  fGenfitID  = -999;
  fGenfitMomentum = -999;
}

void KBHelixTrack::Print(Option_t *) const
{
  TString center = "("+TString::Itoa(fXHelixCenter,10)+", x, "+TString::Itoa(fZHelixCenter,10)+")";

  cout << left << " KBHelixTrack, units in [mm] [radian] [ADC]" << endl;
  cout << " - " << setw(13) << "Track ID"     << " : " << fTrackID << endl;
  cout << " - " << setw(13) << "Parent ID"    << " : " << fParentID << endl;
  cout << " - " << setw(13) << "Fit Status"   << " : " << GetFitStatusString() << endl;

  if (fFitStatus != KBHelixTrack::kHelix && fFitStatus != KBHelixTrack::kGenfitTrack)
    return;

  cout << " - " << setw(13) << "Helix Center" << " : " << center << " [mm]" << endl;
  cout << " - " << setw(13) << "Helix Radius" << " : " << fHelixRadius << " [mm]" << endl;
  cout << " - " << setw(13) << "Dip Angle"    << " : " << DipAngle() << endl;
  cout << " - " << setw(13) << "Fit RMS-w/h"  << " : " << fRMSW << " / " << fRMSH << " [mm]" << endl;
  cout << " - " << setw(13) << "Charge"       << " : " << fChargeSum << " [ADC]" << endl;;
  cout << " - " << setw(13) << "Track Length" << " : " << TrackLength() << " [mm]" << endl;;
  cout << " - " << setw(13) << "Momentum"     << " : " << Momentum().Mag() << " [MeV]" << endl;;

  if (fFitStatus == KBHelixTrack::kGenfitTrack) {
    cout << " - " << setw(13) << "GF-Momentum"  << " : " << fGenfitMomentum << " [MeV]" << endl;;
    cout << " - " << setw(13) << "dEdx (70 %)"  << " : " << GetdEdxWithCut(0, 0.7) << " [ADC/mm]" << endl;;
  }
}

KBTrackFitter *KBHelixTrack::CreateTrackFitter() const { return new KBHelixTrackFitter(); }

void KBHelixTrack::AddHit(KBHit *hit)
{
  Double_t x = hit -> GetX();
  Double_t y = hit -> GetY();
  Double_t z = hit -> GetZ();
  Double_t w = hit -> GetCharge();

  Double_t W = fChargeSum + w;

  fExpectationX = (fChargeSum * fExpectationX + w * x) / W;
  fExpectationY = (fChargeSum * fExpectationY + w * y) / W;
  fExpectationZ = (fChargeSum * fExpectationZ + w * z) / W;

  fExpectationXX = (fChargeSum * fExpectationXX + w * x * x) / W;
  fExpectationYY = (fChargeSum * fExpectationYY + w * y * y) / W;
  fExpectationZZ = (fChargeSum * fExpectationZZ + w * z * z) / W;

  fExpectationXY = (fChargeSum * fExpectationXY + w * x * y) / W;
  fExpectationYZ = (fChargeSum * fExpectationYZ + w * y * z) / W;
  fExpectationZX = (fChargeSum * fExpectationZX + w * z * x) / W;

  fChargeSum = W;

  fMainHits.push_back(hit);
}

void KBHelixTrack::RemoveHit(KBHit *hit)
{
  Double_t x = hit -> GetX();
  Double_t y = hit -> GetY();
  Double_t z = hit -> GetZ();
  Double_t w = hit -> GetCharge();

  Double_t W = fChargeSum - w;

  Int_t numHits = fMainHits.size();
  for (auto iHit = 0; iHit < numHits; iHit++) {
    if (fMainHits[iHit] == hit) {
      fMainHits.erase(fMainHits.begin()+iHit);
      break;
    }
  }

  fExpectationX = (fChargeSum * fExpectationX - w * x) / W;
  fExpectationY = (fChargeSum * fExpectationY - w * y) / W;
  fExpectationZ = (fChargeSum * fExpectationZ - w * z) / W;

  fExpectationXX = (fChargeSum * fExpectationXX - w * x * x) / W;
  fExpectationYY = (fChargeSum * fExpectationYY - w * y * y) / W;
  fExpectationZZ = (fChargeSum * fExpectationZZ - w * z * z) / W;

  fExpectationXY = (fChargeSum * fExpectationXY - w * x * y) / W;
  fExpectationYZ = (fChargeSum * fExpectationYZ - w * y * z) / W;
  fExpectationZX = (fChargeSum * fExpectationZX - w * z * x) / W;

  fChargeSum = W;
}

void KBHelixTrack::DeleteHits()
{
  for (auto hit : fMainHits)
    delete hit;

  fMainHits.clear();

  for (auto hit : fCandHits)
    delete hit;

  fCandHits.clear();
}

void KBHelixTrack::SortHits(bool increasing)
{
  if (increasing) {
    auto sorting = KBHitSortByIncreasingLength(this);
    sort(fMainHits.begin(), fMainHits.end(), sorting);
  } else {
    auto sorting = KBHitSortByDecreasingLength(this);
    sort(fMainHits.begin(), fMainHits.end(), sorting);
  }
}

void KBHelixTrack::SortHitsByTimeOrder() { SortHits(fIsPositiveChargeParticle); }

void KBHelixTrack::FinalizeHits()
{
  for (auto hit : fMainHits) {
    fMainHitIDs.push_back(hit->GetHitID());
    hit -> SetTrackID(fTrackID);
  }
}

void KBHelixTrack::SetGenfitID(Int_t idx)   { fGenfitID = idx; }

void KBHelixTrack::SetFitStatus(KBFitStatus value)  { fFitStatus = value; }
void KBHelixTrack::SetIsBad()          { fFitStatus = KBHelixTrack::kBad; }
void KBHelixTrack::SetIsLine()         { fFitStatus = KBHelixTrack::kLine; }
void KBHelixTrack::SetIsPlane()        { fFitStatus = KBHelixTrack::kPlane; }
void KBHelixTrack::SetIsHelix()        { fFitStatus = KBHelixTrack::kHelix; }
void KBHelixTrack::SetIsGenfitTrack()  { fFitStatus = KBHelixTrack::kGenfitTrack; }

void KBHelixTrack::SetLineDirection(TVector3 dir)
{
  fXHelixCenter = dir.X();
  fZHelixCenter = dir.Y();
  fHelixRadius = dir.Z();
}

void KBHelixTrack::SetPlaneNormal(TVector3 norm)
{
  fXHelixCenter = norm.X();
  fZHelixCenter = norm.Y();
  fHelixRadius = norm.Z();
}

void KBHelixTrack::SetHelixCenter(Double_t x, Double_t z) { fXHelixCenter = x; fZHelixCenter = z; }
void KBHelixTrack::SetHelixRadius(Double_t r)    { fHelixRadius = r; }
void KBHelixTrack::SetYInitial(Double_t y)       { fYInitial = y; }
void KBHelixTrack::SetAlphaSlope(Double_t s)     { fAlphaSlope = s; }
void KBHelixTrack::SetRMSW(Double_t rms)         { fRMSW = rms; }
void KBHelixTrack::SetRMSH(Double_t rms)         { fRMSH = rms; }
void KBHelixTrack::SetAlphaHead(Double_t alpha)  { fAlphaHead = alpha; }
void KBHelixTrack::SetAlphaTail(Double_t alpha)  { fAlphaTail = alpha; }

void KBHelixTrack::DetermineParticleCharge(TVector3 vertex)
{
  Double_t lHead = ExtrapolateToAlpha(fAlphaHead);
  Double_t lTail = ExtrapolateToAlpha(fAlphaTail);

  TVector3 q;
  Double_t alpha;
  Double_t lVertex = ExtrapolateToPointAlpha(vertex, q, alpha);

  if (std::abs(lVertex-lTail) > std::abs(lVertex - lHead))
    fIsPositiveChargeParticle = true;
  else
    fIsPositiveChargeParticle = false;
}

void KBHelixTrack::SetIsPositiveChargeParticle(Bool_t val)  { fIsPositiveChargeParticle = val; }

void KBHelixTrack::SetGenfitMomentum(Double_t p) { fGenfitMomentum = p; }

Int_t KBHelixTrack::GetGenfitID() const  { return fGenfitID; }

KBHelixTrack::KBFitStatus KBHelixTrack::GetFitStatus() const { return fFitStatus; }

TString KBHelixTrack::GetFitStatusString() const
{
  TString fitStat;

  if      (fFitStatus == KBHelixTrack::kBad) fitStat = "Bad";
  else if (fFitStatus == KBHelixTrack::kLine) fitStat = "Line";
  else if (fFitStatus == KBHelixTrack::kPlane) fitStat = "Plane";
  else if (fFitStatus == KBHelixTrack::kHelix) fitStat = "Helix";
  else if (fFitStatus == KBHelixTrack::kGenfitTrack) fitStat = "Genfit";

  return fitStat;
}

bool KBHelixTrack::IsBad() const          { return fFitStatus == kBad   ? true : false; }
bool KBHelixTrack::IsLine()  const        { return fFitStatus == kLine  ? true : false; }
bool KBHelixTrack::IsPlane() const        { return fFitStatus == kPlane ? true : false; }
bool KBHelixTrack::IsHelix() const        { return fFitStatus == kHelix ? true : false; }
bool KBHelixTrack::IsGenfitTrack() const  { return fFitStatus == kGenfitTrack ? true : false; }

Double_t KBHelixTrack::GetHelixCenterX() const { return fXHelixCenter; }
Double_t KBHelixTrack::GetHelixCenterZ() const { return fZHelixCenter; }
Double_t KBHelixTrack::GetHelixRadius()  const { return fHelixRadius; }
Double_t KBHelixTrack::GetYInitial()     const { return fYInitial; }
Double_t KBHelixTrack::GetAlphaSlope()   const { return fAlphaSlope; }

TVector3 KBHelixTrack::GetLineDirection() const { return TVector3(fXHelixCenter, fZHelixCenter, fHelixRadius); }
TVector3 KBHelixTrack::GetPlaneNormal()   const { return TVector3(fXHelixCenter, fZHelixCenter, fHelixRadius); }

TVector3 KBHelixTrack::PerpLine(TVector3 p) const
{
  TVector3 mean = GetMean();
  TVector3 dir = GetLineDirection();

  TVector3 pMinusMean = p - mean;
  TVector3 pMinusMeanUnit = pMinusMean.Unit();
  Double_t cosine = pMinusMeanUnit.Dot(dir);
  dir.SetMag(pMinusMean.Mag()*cosine);

  return dir - pMinusMean;
}

TVector3 KBHelixTrack::PerpPlane(TVector3 p) const
{
  TVector3 normal = GetPlaneNormal();
  TVector3 mean = GetMean();

  Double_t perp = abs(normal * p - normal * mean) / sqrt(normal * normal);
  return perp * normal;
}

Double_t KBHelixTrack::GetGenfitMomentum() const
{
  return fGenfitMomentum;
}

Double_t KBHelixTrack::DipAngle() const
{
  if (fHelixRadius <= 0)
    return -999;

  return TMath::ATan(fAlphaSlope/fHelixRadius);
}


void KBHelixTrack::GetHelixParameters(Double_t &xCenter, 
    Double_t &zCenter, 
    Double_t &radius, 
    Double_t &dipAngle,
    Double_t &yInitial,
    Double_t &alphaSlope) const
{
  if (fFitStatus == KBHelixTrack::kHelix || fFitStatus == KBHelixTrack::kGenfitTrack)
  {
    xCenter    = fXHelixCenter;
    zCenter    = fZHelixCenter;
    radius     = fHelixRadius;
    dipAngle   = DipAngle();
    yInitial   = fYInitial;
    alphaSlope = fAlphaSlope;
  }
  else
  {
    xCenter    = -999;
    zCenter    = -999;
    radius     = -999;
    dipAngle   = -999;
    yInitial   = -999;
    alphaSlope = -999;
  }
}

Double_t KBHelixTrack::GetChargeSum()  const { return fChargeSum; }

TVector3 KBHelixTrack::GetMean()  const { return TVector3(fExpectationX, fExpectationY, fExpectationZ); }
Double_t KBHelixTrack::GetXMean() const { return fExpectationX; }
Double_t KBHelixTrack::GetYMean() const { return fExpectationY; }
Double_t KBHelixTrack::GetZMean() const { return fExpectationZ; }
Double_t KBHelixTrack::GetXCov()  const { return CovWXX()/fChargeSum; }
Double_t KBHelixTrack::GetZCov()  const { return CovWZZ()/fChargeSum; }

Double_t KBHelixTrack::CovWXX() const { return fChargeSum * (fExpectationXX - fExpectationX * fExpectationX); }
Double_t KBHelixTrack::CovWYY() const { return fChargeSum * (fExpectationYY - fExpectationY * fExpectationY); }
Double_t KBHelixTrack::CovWZZ() const { return fChargeSum * (fExpectationZZ - fExpectationZ * fExpectationZ); }

Double_t KBHelixTrack::CovWXY() const { return fChargeSum * (fExpectationXY - fExpectationX * fExpectationY); }
Double_t KBHelixTrack::CovWYZ() const { return fChargeSum * (fExpectationYZ - fExpectationY * fExpectationZ); }
Double_t KBHelixTrack::CovWZX() const { return fChargeSum * (fExpectationZX - fExpectationZ * fExpectationX); }

Double_t KBHelixTrack::GetExpectationX()  const { return fExpectationX; }
Double_t KBHelixTrack::GetExpectationY()  const { return fExpectationY; }
Double_t KBHelixTrack::GetExpectationZ()  const { return fExpectationZ; }

Double_t KBHelixTrack::GetExpectationXX() const { return fExpectationXX; }
Double_t KBHelixTrack::GetExpectationYY() const { return fExpectationYY; }
Double_t KBHelixTrack::GetExpectationZZ() const { return fExpectationZZ; }

Double_t KBHelixTrack::GetExpectationXY() const { return fExpectationXY; }
Double_t KBHelixTrack::GetExpectationYZ() const { return fExpectationYZ; }
Double_t KBHelixTrack::GetExpectationZX() const { return fExpectationZX; }

Double_t KBHelixTrack::GetRMSW()       const { return fRMSW; }
Double_t KBHelixTrack::GetRMSH()       const { return fRMSH; }
Double_t KBHelixTrack::GetAlphaHead()  const { return fAlphaHead; }
Double_t KBHelixTrack::GetAlphaTail()  const { return fAlphaTail; }

Bool_t KBHelixTrack::IsPositiveChargeParticle()  const { return fIsPositiveChargeParticle; }



Int_t KBHelixTrack::GetNumHits() const { return fMainHits.size(); }
KBHit *KBHelixTrack::GetHit(Int_t idx) const { return fMainHits.at(idx); }
std::vector<KBHit *> *KBHelixTrack::GetHitArray() { return &fMainHits; }

Int_t KBHelixTrack::GetNumCandHits() const { return fCandHits.size(); }
std::vector<KBHit *> *KBHelixTrack::GetCandHitArray() { return &fCandHits; }


Int_t KBHelixTrack::GetNumHitIDs() const { return fMainHitIDs.size(); }
Int_t KBHelixTrack::GetHitID(Int_t idx) const { return fMainHitIDs.at(idx); }
std::vector<Int_t> *KBHelixTrack::GetHitIDArray() { return &fMainHitIDs; }


std::vector<Double_t> *KBHelixTrack::GetdEdxArray() { return &fdEdxArray; }



Double_t 
KBHelixTrack::DistCircle(TVector3 pointGiven) const
{
  Double_t dx = pointGiven.X() - fXHelixCenter;
  Double_t dz = pointGiven.Z() - fZHelixCenter;
  return sqrt(dx*dx + dz*dz) - fHelixRadius;
}

Int_t KBHelixTrack::Charge()   const { return fIsPositiveChargeParticle ? 1 : -1; }
Int_t KBHelixTrack::Helicity() const { return fAlphaSlope > 0 ? 1 : -1; }

TVector3 KBHelixTrack::PositionAtHead() const { return PositionByAlpha(fAlphaHead); }
TVector3 KBHelixTrack::PositionAtTail() const { return PositionByAlpha(fAlphaTail); }

TVector3 KBHelixTrack::Momentum(Double_t B) const
{
  auto mom = Direction(fAlphaTail);

  if (fFitStatus != KBHelixTrack::kHelix && fFitStatus != KBHelixTrack::kGenfitTrack)
    return TVector3(-999,-999,-999);

  Double_t cosDip = TMath::Cos(DipAngle());
  if (cosDip < 1E-2)
    return TMath::Abs(fHelixRadius/1.E-2 * 0.3 * B) * mom;

  return TMath::Abs(fHelixRadius/cosDip * 0.3 * B) * mom;
}

Double_t KBHelixTrack::TrackLength() const { 
  return std::abs(GetAlphaHead() - GetAlphaTail()) * fHelixRadius / TMath::Cos(DipAngle());
}
Double_t KBHelixTrack::LengthInPeriod()  const { 
  return 2*TMath::Pi()*fHelixRadius/TMath::Cos(DipAngle());
}
Double_t KBHelixTrack::YLengthInPeriod() const { 
  return 2*TMath::Pi()*fAlphaSlope;
}
Double_t KBHelixTrack::LengthByAlpha(Double_t alpha) const { 
  return alpha*fHelixRadius/TMath::Cos(DipAngle());
}
Double_t KBHelixTrack::AlphaByLength(Double_t length) const { 
  return length*TMath::Cos(DipAngle())/fHelixRadius;
}
TVector3 KBHelixTrack::PositionByAlpha(Double_t alpha) const {

  return TVector3(fHelixRadius*TMath::Cos(alpha)+fXHelixCenter, alpha*fAlphaSlope+fYInitial, fHelixRadius*TMath::Sin(alpha)+fZHelixCenter); 
}

TVector3 KBHelixTrack::Direction(Double_t alpha) const
{
  Double_t alphaTemp = alpha;
  Double_t ylength = YLengthInPeriod()/4.;

  Double_t alphaMid = (fAlphaHead + fAlphaTail) * 0.5;
  if (alpha > alphaMid) 
    alphaTemp += TMath::Pi()/2.;
  else {
    alphaTemp -= TMath::Pi()/2.;
    ylength *= -1;
  }

  TVector3 center(fXHelixCenter, 0, fZHelixCenter);
  TVector3 direction = PositionByAlpha(alphaTemp) - center;

  direction.SetY(0);
  direction.SetMag(0.5*TMath::Pi()*fHelixRadius);
  direction.SetY(ylength);
  direction = direction.Unit();

  return direction;
}

Double_t 
KBHelixTrack::ExtrapolateToAlpha(Double_t alpha) const 
{
  return alpha * fHelixRadius / TMath::Cos(DipAngle());
}

Double_t
KBHelixTrack::ExtrapolateToAlpha(Double_t alpha, TVector3 &pointOnHelix) const
{
  pointOnHelix.SetXYZ(fHelixRadius*TMath::Cos(alpha)+fXHelixCenter, alpha*fAlphaSlope + fYInitial, fHelixRadius*TMath::Sin(alpha)+fZHelixCenter);
  Double_t length = alpha * fHelixRadius / TMath::Cos(DipAngle());

  return length;
}

Double_t
KBHelixTrack::ExtrapolateToPointAlpha(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const
// TODO
{
  Double_t alpha0 = TMath::ATan2(pointGiven.Z()-fZHelixCenter, pointGiven.X()-fXHelixCenter);

  TVector3 point0(fHelixRadius*TMath::Cos(alpha0)+fXHelixCenter, alpha0*fAlphaSlope+fYInitial, fHelixRadius*TMath::Sin(alpha0)+fZHelixCenter);
  Double_t y0 = std::abs(point0.Y() - pointGiven.Y());

  Double_t y1; 
  Double_t alpha1 = alpha0;
  TVector3 point1 = point0;

  Double_t yLengthInPeriod = std::abs(YLengthInPeriod());
  if (yLengthInPeriod > 3*fRMSH && yLengthInPeriod > 5 && std::abs(DipAngle()) < 1.5)
  {
    Int_t count = 0;
    while(1)
    {
      alpha1 = alpha1 + 2*TMath::Pi();
      point1.SetY(point1.Y() + 2*TMath::Pi()*fAlphaSlope);
      y1 = std::abs(point1.Y() - pointGiven.Y());

      if (y0 - y1 < 1.e10)
        break;
      else {
        alpha0 = alpha1;
        point0 = point1;
        y0 = y1;
      }
      if (count++ > 20)
        break;
    }

    y1 = y0;
    alpha1 = alpha0;
    point1 = point0;

    count = 0;
    while(1)
    {
      alpha1 = alpha1 - 2*TMath::Pi();
      point1.SetY(point1.Y() - 2*TMath::Pi()*fAlphaSlope);
      y1 = std::abs(point1.Y() - pointGiven.Y());

      if (y0 - y1 < 1.e10)
        break;
      else {
        alpha0 = alpha1;
        point0 = point1;
        y0 = y1;
      }
      if (count++ > 20)
        break;
    }
  }

  pointOnHelix = point0;
  alpha = alpha0;
  Double_t length = alpha0 * fHelixRadius / TMath::Cos(DipAngle());

  return length;
}

Double_t
KBHelixTrack::ExtrapolateToPointY(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const
{
  alpha = (pointGiven.Y() - fYInitial)/fAlphaSlope;
  pointOnHelix.SetXYZ(fHelixRadius*TMath::Cos(alpha)+fXHelixCenter, pointGiven.Y(), fHelixRadius*TMath::Sin(alpha)+fZHelixCenter);

  Double_t length = alpha * fHelixRadius / TMath::Cos(DipAngle());
  return length;
}

bool
KBHelixTrack::CheckExtrapolateToX(Double_t x) const
{
  Double_t xRef = fXHelixCenter - x;
  Double_t mult = (xRef + fHelixRadius) * (xRef - fHelixRadius);
  if (mult > 0)
    return false;
  return true;
}

bool
KBHelixTrack::CheckExtrapolateToZ(Double_t z) const
{
  Double_t zRef = fZHelixCenter - z;
  Double_t mult = (zRef + fHelixRadius) * (zRef - fHelixRadius);
  if (mult > 0)
    return false;
  return true;
}

bool
KBHelixTrack::ExtrapolateToX(Double_t x,
    TVector3 &pointOnHelix1, Double_t &alpha1,
    TVector3 &pointOnHelix2, Double_t &alpha2) const
{
  if (CheckExtrapolateToX(x) == false)
    return false;

  Double_t zOff = sqrt(fHelixRadius * fHelixRadius - (x - fXHelixCenter) * (x - fXHelixCenter));
  Double_t z1 = fZHelixCenter + zOff;
  Double_t z2 = fZHelixCenter - zOff;

  alpha1 = TMath::ATan2(z1-fZHelixCenter, x-fXHelixCenter);
  Double_t alpha1Temp = alpha1;
  Double_t d1Cand = std::abs(alpha1Temp-fAlphaHead);
  Double_t d1Temp = d1Cand;

  while (1) {
    alpha1Temp = alpha1 + 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fAlphaHead);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  while (1) {
    alpha1Temp = alpha1 - 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fAlphaHead);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  pointOnHelix1 = PositionByAlpha(alpha1);

  alpha2 = TMath::ATan2(z2-fZHelixCenter, x-fXHelixCenter);
  Double_t alpha2Temp = alpha2;
  Double_t d2Cand = std::abs(alpha2Temp-fAlphaTail);
  Double_t d2Temp = d2Cand;

  while (1) {
    alpha2Temp = alpha2 + 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fAlphaTail);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  while (1) {
    alpha2Temp = alpha2 - 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fAlphaTail);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  pointOnHelix2 = PositionByAlpha(alpha2);

  return true;
}

bool
KBHelixTrack::ExtrapolateToZ(Double_t z,
    TVector3 &pointOnHelix1, Double_t &alpha1,
    TVector3 &pointOnHelix2, Double_t &alpha2) const
{
  if (CheckExtrapolateToZ(z) == false)
    return false;

  Double_t xOff = sqrt(fHelixRadius * fHelixRadius - (z - fZHelixCenter) * (z - fZHelixCenter));
  Double_t x1 = fXHelixCenter + xOff;
  Double_t x2 = fXHelixCenter - xOff;

  alpha1 = TMath::ATan2(z-fZHelixCenter, x1-fXHelixCenter);
  Double_t alpha1Temp = alpha1;
  Double_t d1Cand = std::abs(alpha1Temp-fAlphaHead);
  Double_t d1Temp = d1Cand;

  while (1) {
    alpha1Temp = alpha1 + 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fAlphaHead);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  while (1) {
    alpha1Temp = alpha1 - 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fAlphaHead);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  pointOnHelix1 = PositionByAlpha(alpha1);

  alpha2 = TMath::ATan2(z-fZHelixCenter, x2-fXHelixCenter);
  Double_t alpha2Temp = alpha2;
  Double_t d2Cand = std::abs(alpha2Temp-fAlphaTail);
  Double_t d2Temp = d2Cand;

  while (1) {
    alpha2Temp = alpha2 + 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fAlphaTail);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  while (1) {
    alpha2Temp = alpha2 - 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fAlphaTail);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  pointOnHelix2 = PositionByAlpha(alpha2);

  return true;
}

bool
KBHelixTrack::ExtrapolateToX(Double_t x, Double_t alphaRef, TVector3 &pointOnHelix) const
{
  if (CheckExtrapolateToX(x) == false)
    return false;

  Double_t zOff = sqrt(fHelixRadius * fHelixRadius - (x - fXHelixCenter) * (x - fXHelixCenter));
  Double_t z1 = fZHelixCenter + zOff;

  Double_t alpha = TMath::ATan2(z1-fZHelixCenter, x-fXHelixCenter);
  Double_t alphaTemp = alpha;
  Double_t d1Cand = std::abs(alphaTemp-alphaRef);
  Double_t d1Temp = d1Cand;

  while (1) {
    alphaTemp = alpha + 2*TMath::Pi();
    d1Temp = std::abs(alphaTemp-alphaRef);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha = alphaTemp;
      d1Cand = d1Temp;
    }
  }
  while (1) {
    alphaTemp = alpha - 2*TMath::Pi();
    d1Temp = std::abs(alphaTemp-alphaRef);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha = alphaTemp;
      d1Cand = d1Temp;
    }
  }
  pointOnHelix = PositionByAlpha(alpha);

  return true;
}

bool
KBHelixTrack::ExtrapolateToZ(Double_t z, Double_t alphaRef, TVector3 &pointOnHelix) const
{
  if (CheckExtrapolateToZ(z) == false)
    return false;

  Double_t xOff = sqrt(fHelixRadius * fHelixRadius - (z - fZHelixCenter) * (z - fZHelixCenter));
  Double_t x1 = fXHelixCenter + xOff;

  Double_t alpha = TMath::ATan2(z-fZHelixCenter, x1-fXHelixCenter);
  Double_t alphaTemp = alpha;
  Double_t d1Cand = std::abs(alphaTemp-alphaRef);
  Double_t d1Temp = d1Cand;

  while (1) {
    alphaTemp = alpha + 2*TMath::Pi();
    d1Temp = std::abs(alphaTemp-alphaRef);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha = alphaTemp;
      d1Cand = d1Temp;
    }
  }
  while (1) {
    alphaTemp = alpha - 2*TMath::Pi();
    d1Temp = std::abs(alphaTemp-alphaRef);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha = alphaTemp;
      d1Cand = d1Temp;
    }
  }
  pointOnHelix = PositionByAlpha(alpha);

  return true;
}

bool
KBHelixTrack::ExtrapolateToZ(Double_t z, TVector3 &pointOnHelix) const
{
  TVector3 position1, position2;
  Double_t alpha1, alpha2;

  if (ExtrapolateToZ(z, position1, alpha1, position2, alpha2) == false)
    return false;

  Double_t alphaMid = (fAlphaHead + fAlphaTail)/2;

  if (std::abs(alpha1 - alphaMid) < std::abs(alpha2 - alphaMid))
    pointOnHelix = position1;
  else
    pointOnHelix = position2;

  return true;
}

TVector3 
KBHelixTrack::ExtrapolateHead(Double_t length) const
{
  Double_t alpha = fAlphaHead;
  Double_t dAlpha = std::abs(AlphaByLength(length));

  if (fAlphaHead > fAlphaTail)
    alpha += dAlpha;
  else
    alpha -= dAlpha;

  return PositionByAlpha(alpha);
}

TVector3 
KBHelixTrack::ExtrapolateTail(Double_t length) const
{
  Double_t alpha = fAlphaTail;
  Double_t dAlpha = std::abs(AlphaByLength(length));

  if (fAlphaTail > fAlphaHead)
    alpha += dAlpha;
  else
    alpha -= dAlpha;

  return PositionByAlpha(alpha);
}

TVector3 
KBHelixTrack::InterpolateByRatio(Double_t r) const
{
  TVector3 q;
  ExtrapolateToAlpha(r*fAlphaHead+(1-r)*fAlphaTail, q);
  return q;
}

TVector3 
KBHelixTrack::InterpolateByLength(Double_t length) const
{
  return InterpolateByRatio(length/TrackLength());
}

TVector3 
KBHelixTrack::Map(TVector3 p) const
{
  TVector3 q, m;
  ExtrapolateByMap(p, q, m);

  return m;
}

Double_t
KBHelixTrack::AlphaAtPosition(TVector3 p) const
{
  Double_t alpha;
  TVector3 q(0,0,0);
  ExtrapolateToPointAlpha(p, q, alpha);
  return alpha;
}

Double_t 
KBHelixTrack::ExtrapolateByMap(TVector3 p, TVector3 &q, TVector3 &m) const
{
  Double_t lHead = ExtrapolateToAlpha(fAlphaHead);
  Double_t lTail = ExtrapolateToAlpha(fAlphaTail);
  Double_t lOff = lHead;
  if (lHead > lTail)
    lOff = lTail;

  Double_t alpha;
  Double_t length = ExtrapolateToPointAlpha(p, q, alpha);
  Double_t r = DistCircle(p);
  Double_t y = p.Y() - q.Y();

  m = TVector3(r, y/TMath::Cos(DipAngle()), length+y*TMath::Sin(DipAngle()) - lOff);

  return alpha * fHelixRadius / TMath::Cos(DipAngle()); 
}

Double_t 
KBHelixTrack::Continuity(Double_t &totalLength, Double_t &continuousLength)
{
  Int_t numHits = fMainHits.size();
  if (numHits < 2) 
    return -1;

  SortHits();

  Double_t total = 0;
  Double_t continuous = 0;
  TVector3 before = Map(fMainHits[0]->GetPosition());

  for (auto iHit = 1; iHit < numHits; iHit++) 
  {
    TVector3 current = Map(fMainHits[iHit]->GetPosition());
    auto length = std::abs(current.Z()-before.Z());

    total += length;
    if (length < 50)
      continuous += length;

    before = current;
  }

  totalLength = total;
  continuousLength = continuous;

  return continuous/total;
}

Double_t 
KBHelixTrack::Continuity()
{
  Int_t numHits = fMainHits.size();
  if (numHits < 2) 
    return -1;

  SortHits();

  Double_t total = 0;
  Double_t continuous = 0;
  TVector3 before = Map(fMainHits[0]->GetPosition());

  for (auto iHit = 1; iHit < numHits; iHit++) 
  {
    TVector3 current = Map(fMainHits[iHit]->GetPosition());
    auto length = std::abs(current.Z()-before.Z());

    total += length;
    if (length < 20)
      continuous += length;

    before = current;
  }

  return continuous/total;
}

Double_t 
KBHelixTrack::GetdEdxWithCut(Double_t lowR, Double_t highR) const
{
  auto numPoints = fdEdxArray.size();

  Int_t idxLow = Int_t(numPoints * lowR);
  Int_t idxHigh = Int_t(numPoints * highR);

  numPoints = idxHigh - idxLow;
  if (numPoints < 3)
    return -1;

  Double_t dEdx = 0.;
  for (Int_t idEdx = idxLow; idEdx < idxHigh; idEdx++)
    dEdx += fdEdxArray[idEdx];
  dEdx = dEdx/numPoints;

  return dEdx;
}

TVector3 KBHelixTrack::ExtrapolateTo(TVector3 point) const
{
  auto alpha = AlphaAtPosition(point);
  TVector3 position;
  ExtrapolateToAlpha(alpha, position);

  return position;
}

TVector3 KBHelixTrack::ExtrapolateByRatio(Double_t r) const
{
  return InterpolateByRatio(r);
}

TVector3 KBHelixTrack::ExtrapolateByLength(Double_t l) const
{
  return InterpolateByLength(l);
}

Double_t KBHelixTrack::LengthAt(TVector3 point) const
{
  auto alpha = AlphaAtPosition(point);
  return ExtrapolateToAlpha(alpha);
}
