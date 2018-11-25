#include "KBHelixTrack.hh"

#include <iostream>
#include <iomanip>
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

void KBHelixTrack::Clear(Option_t *)
{
  fTrackID  = -999;
  fParentID = -999;

  fReferenceAxis = KBVector3::kZ;

  fFitStatus = kBad;

  fIHelixCenter = -999;
  fJHelixCenter = -999;
  fHelixRadius  = -999;
  fKInitial     = -999;
  fAlphaSlope   = -999;

  fChargeSum = 0;

  fExpectationI = 0;
  fExpectationK = 0;
  fExpectationJ = 0;
  fExpectationII = 0;
  fExpectationKK = 0;
  fExpectationJJ = 0;
  fExpectationIJ = 0;
  fExpectationJK = 0;
  fExpectationKI = 0;

  fRMSW = -999;
  fRMSH = -999;

  fAlphaHead = -999;
  fAlphaTail = -999;

  fIsPositiveChargeParticle = true;

  //if (TString(option) == "C") DeleteHits();
  //else {
    fMainHits.clear();
    fCandHits.clear();
  //}

  fMainHitIDs.clear();
  fdEdxArray.clear();

  fGenfitID = -999;
  fGenfitMomentum = -999;
}

void KBHelixTrack::Print(Option_t *) const
{
  TString center = "("+TString::Itoa(fIHelixCenter,10)+", "+TString::Itoa(fJHelixCenter,10)+")";

       if (fReferenceAxis == KBVector3::kX) center = TString("(y,z): ")+center;
  else if (fReferenceAxis == KBVector3::kY) center = TString("(z,x): ")+center;
  else if (fReferenceAxis == KBVector3::kZ) center = TString("(x,y): ")+center;

  kc_raw << left;
  kc_info << setw(13) << "Track ID"     << " : " << fTrackID << endl;
  kc_info << setw(13) << "Parent ID"    << " : " << fParentID << endl;
  kc_info << setw(13) << "Fit Status"   << " : " << GetFitStatusString() << endl;
  kc_info << setw(13) << "# of Hits"    << " : " << fMainHits.size() << endl;

  if (fFitStatus != KBHelixTrack::kHelix && fFitStatus != KBHelixTrack::kGenfitTrack)
    return;

  kc_info << setw(13) << "Helix Center" << " : " << center << " [mm]" << endl;
  kc_info << setw(13) << "Helix Radius" << " : " << fHelixRadius << " [mm]" << endl;
  kc_info << setw(13) << "Dip Angle"    << " : " << DipAngle() << endl;
  kc_info << setw(13) << "Fit RMS-w/h"  << " : " << fRMSW << " / " << fRMSH << " [mm]" << endl;
  kc_info << setw(13) << "Charge"       << " : " << fChargeSum << " [ADC]" << endl;;
  kc_info << setw(13) << "Track Length" << " : " << TrackLength() << " [mm]" << endl;;
  kc_info << setw(13) << "Momentum"     << " : " << Momentum().Mag() << " [MeV]" << endl;;

  if (fFitStatus == KBHelixTrack::kGenfitTrack) {
    kc_info << setw(13) << "GF-Momentum"  << " : " << fGenfitMomentum << " [MeV]" << endl;;
    kc_info << setw(13) << "dEdx (70 %)"  << " : " << GetdEdxWithCut(0, 0.7) << " [ADC/mm]" << endl;;
  }
}

void KBHelixTrack::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto helix = (KBHelixTrack &) obj;

  helix.SetHelixCenter(fIHelixCenter, fJHelixCenter);
  helix.SetHelixRadius(fHelixRadius);
  helix.SetKInitial(fKInitial);
  helix.SetAlphaSlope(fAlphaSlope);

  helix.SetAlphaHead(fAlphaHead);
  helix.SetAlphaTail(fAlphaTail);
  helix.SetIsPositiveChargeParticle(fIsPositiveChargeParticle);

  helix.SetTrackID(fTrackID);
  helix.SetParentID(fParentID);
  helix.SetReferenceAxis(fReferenceAxis);
}

bool KBHelixTrack::Fit()
{
  return KBHelixTrackFitter::GetFitter() -> Fit(this);
}

KBTrackFitter *KBHelixTrack::CreateTrackFitter() const { return KBHelixTrackFitter::GetFitter(); }

void KBHelixTrack::AddHit(KBHit *hit)
{
  KBVector3 pos(hit->GetPosition(),fReferenceAxis);
  Double_t i = pos.I();
  Double_t j = pos.J();
  Double_t k = pos.K();
  Double_t w = hit -> GetCharge();

  Double_t W = fChargeSum + w;

  fExpectationI = (fChargeSum * fExpectationI + w * i) / W;
  fExpectationJ = (fChargeSum * fExpectationJ + w * j) / W;
  fExpectationK = (fChargeSum * fExpectationK + w * k) / W;

  fExpectationII = (fChargeSum * fExpectationII + w * i * i) / W;
  fExpectationJJ = (fChargeSum * fExpectationJJ + w * j * j) / W;
  fExpectationKK = (fChargeSum * fExpectationKK + w * k * k) / W;

  fExpectationIJ = (fChargeSum * fExpectationIJ + w * i * j) / W;
  fExpectationJK = (fChargeSum * fExpectationJK + w * j * k) / W;
  fExpectationKI = (fChargeSum * fExpectationKI + w * k * i) / W;

  fChargeSum = W;

  fMainHits.push_back(hit);
}

void KBHelixTrack::RemoveHit(KBHit *hit)
{
  KBVector3 pos(hit->GetPosition(),fReferenceAxis);
  Double_t i = pos.I();
  Double_t j = pos.J();
  Double_t k = pos.K();
  Double_t w = hit -> GetCharge();

  Double_t W = fChargeSum - w;

  Int_t numHits = fMainHits.size();
  for (auto iHit = 0; iHit < numHits; iHit++) {
    if (fMainHits[iHit] == hit) {
      fMainHits.erase(fMainHits.begin()+iHit);
      break;
    }
  }

  fExpectationI = (fChargeSum * fExpectationI - w * i) / W;
  fExpectationJ = (fChargeSum * fExpectationJ - w * j) / W;
  fExpectationK = (fChargeSum * fExpectationK - w * k) / W;

  fExpectationII = (fChargeSum * fExpectationII - w * i * i) / W;
  fExpectationJJ = (fChargeSum * fExpectationJJ - w * j * j) / W;
  fExpectationKK = (fChargeSum * fExpectationKK - w * k * k) / W;

  fExpectationIJ = (fChargeSum * fExpectationIJ - w * i * j) / W;
  fExpectationJK = (fChargeSum * fExpectationJK - w * j * k) / W;
  fExpectationKI = (fChargeSum * fExpectationKI - w * k * i) / W;

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

    fHitList.AddHit(hit); //@todo XXX
  }

  PropagateMC(); //@todo XXX
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
  fIHelixCenter = dir.X();
  fJHelixCenter = dir.Y();
  fHelixRadius = dir.Z();
}

void KBHelixTrack::SetPlaneNormal(TVector3 norm)
{
  fIHelixCenter = norm.X();
  fJHelixCenter = norm.Y();
  fHelixRadius = norm.Z();
}

void KBHelixTrack::SetHelixCenter(Double_t i, Double_t j) { fIHelixCenter = i; fJHelixCenter = j; }
void KBHelixTrack::SetHelixRadius(Double_t r)    { fHelixRadius = r; }
void KBHelixTrack::SetKInitial(Double_t k)       { fKInitial = k; }
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

  if (std::abs(lVertex - lTail) > std::abs(lVertex - lHead)) {
    auto a = fAlphaTail;
    fAlphaTail = fAlphaHead;
    fAlphaHead = a;
  }

  if (fAlphaTail < fAlphaHead) {
    if (fAlphaSlope > 0)
      fIsPositiveChargeParticle = false;
    else
      fIsPositiveChargeParticle = true;
  }
  else {
    if (fAlphaSlope > 0)
      fIsPositiveChargeParticle = true;
    else
      fIsPositiveChargeParticle = false;
  }
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

Double_t KBHelixTrack::GetHelixCenterJ() const { return fIHelixCenter; }
Double_t KBHelixTrack::GetHelixCenterI() const { return fJHelixCenter; }
Double_t KBHelixTrack::GetHelixRadius()  const { return fHelixRadius; }
Double_t KBHelixTrack::GetKInitial()     const { return fKInitial; }
Double_t KBHelixTrack::GetAlphaSlope()   const { return fAlphaSlope; }

KBVector3 KBHelixTrack::GetLineDirection() const { return KBVector3(fReferenceAxis, fIHelixCenter, fJHelixCenter, fHelixRadius); }
KBVector3 KBHelixTrack::GetPlaneNormal()   const { return KBVector3(fReferenceAxis, fIHelixCenter, fJHelixCenter, fHelixRadius); }

KBVector3 KBHelixTrack::PerpLine(TVector3 p) const
{
  KBVector3 mean = GetMean();
  KBVector3 dir = GetLineDirection();

  KBVector3 pMinusMean = KBVector3(p,fReferenceAxis) - mean;
  KBVector3 pMinusMeanUnit = KBVector3(pMinusMean.Unit(), fReferenceAxis);
  Double_t cosine = pMinusMeanUnit.Dot(dir);
  dir.SetMag(pMinusMean.Mag()*cosine);

  return dir - pMinusMean;
}

KBVector3 KBHelixTrack::PerpPlane(TVector3 p) const
{
  KBVector3 normal = GetPlaneNormal();
  KBVector3 mean = GetMean();

  Double_t perp = abs(normal * KBVector3(p, fReferenceAxis) - normal * mean) / sqrt(normal * normal);
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


void KBHelixTrack::GetHelixParameters(Double_t &iCenter, 
    Double_t &jCenter, 
    Double_t &radius, 
    Double_t &dipAngle,
    Double_t &kInitial,
    Double_t &alphaSlope) const
{
  if (fFitStatus == KBHelixTrack::kHelix || fFitStatus == KBHelixTrack::kGenfitTrack)
  {
    iCenter    = fIHelixCenter;
    jCenter    = fJHelixCenter;
    radius     = fHelixRadius;
    dipAngle   = DipAngle();
    kInitial   = fKInitial;
    alphaSlope = fAlphaSlope;
  }
  else
  {
    iCenter    = -999;
    jCenter    = -999;
    radius     = -999;
    dipAngle   = -999;
    kInitial   = -999;
    alphaSlope = -999;
  }
}

Double_t KBHelixTrack::GetChargeSum()  const { return fChargeSum; }

KBVector3 KBHelixTrack::GetMean()  const { KBVector3 v3(fReferenceAxis); v3.SetIJK(fExpectationI, fExpectationJ, fExpectationK); return v3; }
Double_t KBHelixTrack::GetIMean() const { return fExpectationJ; }
Double_t KBHelixTrack::GetJMean() const { return fExpectationI; }
Double_t KBHelixTrack::GetKMean() const { return fExpectationK; }
Double_t KBHelixTrack::GetJCov()  const { return CovWJJ()/fChargeSum; }
Double_t KBHelixTrack::GetICov()  const { return CovWII()/fChargeSum; }

Double_t KBHelixTrack::CovWJJ() const { return fChargeSum * (fExpectationII - fExpectationI * fExpectationI); }
Double_t KBHelixTrack::CovWII() const { return fChargeSum * (fExpectationJJ - fExpectationJ * fExpectationJ); }
Double_t KBHelixTrack::CovWKK() const { return fChargeSum * (fExpectationKK - fExpectationK * fExpectationK); }

Double_t KBHelixTrack::CovWIJ() const { return fChargeSum * (fExpectationIJ - fExpectationI * fExpectationJ); }
Double_t KBHelixTrack::CovWJK() const { return fChargeSum * (fExpectationJK - fExpectationJ * fExpectationK); }
Double_t KBHelixTrack::CovWKI() const { return fChargeSum * (fExpectationKI - fExpectationK * fExpectationI); }

Double_t KBHelixTrack::GetExpectationI()  const { return fExpectationI; }
Double_t KBHelixTrack::GetExpectationJ()  const { return fExpectationJ; }
Double_t KBHelixTrack::GetExpectationK()  const { return fExpectationK; }

Double_t KBHelixTrack::GetExpectationII() const { return fExpectationII; }
Double_t KBHelixTrack::GetExpectationJJ() const { return fExpectationJJ; }
Double_t KBHelixTrack::GetExpectationKK() const { return fExpectationKK; }

Double_t KBHelixTrack::GetExpectationIJ() const { return fExpectationIJ; }
Double_t KBHelixTrack::GetExpectationJK() const { return fExpectationJK; }
Double_t KBHelixTrack::GetExpectationKI() const { return fExpectationKI; }

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
  KBVector3 pointGiven2(pointGiven,fReferenceAxis);
  Double_t di = pointGiven2.I() - fIHelixCenter;
  Double_t dj = pointGiven2.J() - fJHelixCenter;
  return sqrt(di*di + dj*dj) - fHelixRadius;
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
Double_t KBHelixTrack::KLengthInPeriod() const { 
  return 2*TMath::Pi()*fAlphaSlope;
}
Double_t KBHelixTrack::LengthByAlpha(Double_t alpha) const { 
  return alpha*fHelixRadius/TMath::Cos(DipAngle());
}
Double_t KBHelixTrack::AlphaByLength(Double_t length) const { 
  return length*TMath::Cos(DipAngle())/fHelixRadius;
}
TVector3 KBHelixTrack::PositionByAlpha(Double_t alpha) const
{
  KBVector3 pos(fReferenceAxis);
  pos.SetIJK(fHelixRadius*TMath::Cos(alpha)+fIHelixCenter, fHelixRadius*TMath::Sin(alpha)+fJHelixCenter, alpha*fAlphaSlope+fKInitial); 
  return pos.GetXYZ();
}

TVector3 KBHelixTrack::Direction(Double_t alpha) const
{
  KBVector3 posCenter(fReferenceAxis,fIHelixCenter,fJHelixCenter,0);

  Double_t alphaPointer = alpha;

  if (fAlphaTail < fAlphaHead) alphaPointer += TMath::Pi()/2.;
  else                         alphaPointer -= TMath::Pi()/2.;

  KBVector3 posPointer(PositionByAlpha(alphaPointer), fReferenceAxis);
  KBVector3 direction = posPointer - posCenter;
  auto directionz = direction.Z();
  direction.SetK(0);
  direction.SetMag(2*TMath::Pi()*fHelixRadius);

  if (directionz > 0) direction.SetK(+abs(KLengthInPeriod()));
  else                direction.SetK(-abs(KLengthInPeriod()));

  auto unitDirection = direction.GetXYZ().Unit();

  return unitDirection;
}

Double_t 
KBHelixTrack::ExtrapolateToAlpha(Double_t alpha) const 
{
  return alpha * fHelixRadius / TMath::Cos(DipAngle());
}

Double_t
KBHelixTrack::ExtrapolateToAlpha(Double_t alpha, TVector3 &pointOnHelix) const
{
  KBVector3 pointOnHelix0(fReferenceAxis);
  pointOnHelix0.SetIJK(fHelixRadius*TMath::Cos(alpha)+fIHelixCenter, fHelixRadius*TMath::Sin(alpha)+fJHelixCenter, alpha*fAlphaSlope + fKInitial);
  pointOnHelix = pointOnHelix0.GetXYZ();
  Double_t length = alpha * fHelixRadius / TMath::Cos(DipAngle());

  return length;
}

Double_t
KBHelixTrack::ExtrapolateToPointAlpha(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const
// TODO
{
  KBVector3 pointGiven2(pointGiven,fReferenceAxis);
  Double_t alpha0 = TMath::ATan2(pointGiven2.J()-fJHelixCenter, pointGiven2.I()-fIHelixCenter);

  KBVector3 point0(fReferenceAxis);
  point0.SetIJK(fHelixRadius*TMath::Cos(alpha0)+fIHelixCenter, fHelixRadius*TMath::Sin(alpha0)+fJHelixCenter, alpha0*fAlphaSlope+fKInitial);
  Double_t k0 = std::abs(point0.K() - pointGiven2.K());

  Double_t k1; 
  Double_t alpha1 = alpha0;
  KBVector3 point1 = point0;

  Double_t kLengthInPeriod = std::abs(KLengthInPeriod());
  if (kLengthInPeriod > 3*fRMSH && kLengthInPeriod > 5 && std::abs(DipAngle()) < 1.5)
  {
    Int_t count = 0;
    while(1)
    {
      alpha1 = alpha1 + 2*TMath::Pi();
      point1.SetK(point1.K() + 2*TMath::Pi()*fAlphaSlope);
      k1 = std::abs(point1.K() - pointGiven2.K());

      if (std::abs(k0) < std::abs(k1))
        break;
      else {
        alpha0 = alpha1;
        point0 = point1;
        k0 = k1;
      }
      if (count++ > 20)
        break;
    }

    k1 = k0;
    alpha1 = alpha0;
    point1 = point0;

    count = 0;
    while(1)
    {
      alpha1 = alpha1 - 2*TMath::Pi();
      point1.SetK(point1.K() - 2*TMath::Pi()*fAlphaSlope);
      k1 = std::abs(point1.K() - pointGiven2.K());

      if (std::abs(k0) < std::abs(k1))
        break;
      else {
        alpha0 = alpha1;
        point0 = point1;
        k0 = k1;
      }
      if (count++ > 20)
        break;
    }
  }

  pointOnHelix = point0.GetXYZ();
  alpha = alpha0;
  Double_t length = alpha0 * fHelixRadius / TMath::Cos(DipAngle());

  return length;
}

Double_t
KBHelixTrack::ExtrapolateToPointK(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const
{
  KBVector3 pointGiven2(pointGiven, fReferenceAxis);
  alpha = (pointGiven2.K() - fKInitial)/fAlphaSlope;

  KBVector3 ph(fReferenceAxis, fHelixRadius*TMath::Cos(alpha)+fIHelixCenter, fHelixRadius*TMath::Sin(alpha)+fJHelixCenter, pointGiven2.K());
  pointOnHelix = ph.GetXYZ();

  Double_t length = alpha * fHelixRadius / TMath::Cos(DipAngle());
  return length;
}

bool
KBHelixTrack::CheckExtrapolateToI(Double_t i) const
{
  Double_t iRef = fIHelixCenter - i;
  Double_t mult = (iRef + fHelixRadius) * (iRef - fHelixRadius);
  if (mult > 0)
    return false;
  return true;
}

bool
KBHelixTrack::CheckExtrapolateToJ(Double_t j) const
{
  Double_t jRef = fJHelixCenter - j;
  Double_t mult = (jRef + fHelixRadius) * (jRef - fHelixRadius);
  if (mult > 0)
    return false;
  return true;
}

bool
KBHelixTrack::ExtrapolateToI(Double_t i,
    TVector3 &pointOnHelix1, Double_t &alpha1,
    TVector3 &pointOnHelix2, Double_t &alpha2) const
{
  if (CheckExtrapolateToI(i) == false)
    return false;

  Double_t jOff = sqrt(fHelixRadius * fHelixRadius - (i - fIHelixCenter) * (i - fIHelixCenter));
  Double_t j1 = fJHelixCenter + jOff;
  Double_t j2 = fJHelixCenter - jOff;

  alpha1 = TMath::ATan2(j1-fJHelixCenter, i-fIHelixCenter);
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

  alpha2 = TMath::ATan2(j2-fJHelixCenter, i-fIHelixCenter);
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
KBHelixTrack::ExtrapolateToJ(Double_t j,
    TVector3 &pointOnHelix1, Double_t &alpha1,
    TVector3 &pointOnHelix2, Double_t &alpha2) const
{
  if (CheckExtrapolateToJ(j) == false)
    return false;

  Double_t iOff = sqrt(fHelixRadius * fHelixRadius - (j - fJHelixCenter) * (j - fJHelixCenter));
  Double_t i1 = fIHelixCenter + iOff;
  Double_t i2 = fIHelixCenter - iOff;

  alpha1 = TMath::ATan2(j-fJHelixCenter, i1-fIHelixCenter);
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

  alpha2 = TMath::ATan2(j-fJHelixCenter, i2-fIHelixCenter);
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
KBHelixTrack::ExtrapolateToI(Double_t x, Double_t alphaRef, TVector3 &pointOnHelix) const
{
  if (CheckExtrapolateToI(x) == false)
    return false;

  Double_t zOff = sqrt(fHelixRadius * fHelixRadius - (x - fIHelixCenter) * (x - fIHelixCenter));
  Double_t z1 = fJHelixCenter + zOff;

  Double_t alpha = TMath::ATan2(z1-fJHelixCenter, x-fIHelixCenter);
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
KBHelixTrack::ExtrapolateToJ(Double_t j, Double_t alphaRef, TVector3 &pointOnHelix) const
{
  if (CheckExtrapolateToJ(j) == false)
    return false;

  Double_t iOff = sqrt(fHelixRadius * fHelixRadius - (j - fJHelixCenter) * (j - fJHelixCenter));
  Double_t i1 = fIHelixCenter + iOff;

  Double_t alpha = TMath::ATan2(j-fJHelixCenter, i1-fIHelixCenter);
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
KBHelixTrack::ExtrapolateToJ(Double_t j, TVector3 &pointOnHelix) const
{
  TVector3 position1, position2;
  Double_t alpha1, alpha2;

  if (ExtrapolateToJ(j, position1, alpha1, position2, alpha2) == false)
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

  KBVector3 p2(p, fReferenceAxis);
  KBVector3 q2(q, fReferenceAxis);

  Double_t k = p2.K() - q2.K();

  m = TVector3(r, k/TMath::Cos(DipAngle()), length+k*TMath::Sin(DipAngle()) - lOff);

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
    if (length < 25)
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
  Double_t l1, l2;
  return Continuity(l1, l2);
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
