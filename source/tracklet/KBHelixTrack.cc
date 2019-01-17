#include "KBHelixTrack.hh"

#include <iostream>
#include <iomanip>
using namespace std;

#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

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
  fFitStatus = kBad;

  fTrackID  = -999;
  fParentID = -999;
  fGenfitID = -999;
  fGenfitMomentum = -999.;

  fA = KBVector3::kZ;
  fI = -999.;
  fJ = -999.;
  fR = -999.;
  fS = -999.;
  fK = -999.;
  fT = -999.;
  fH = -999.;

  fIsPositiveChargeParticle = true;

  fHitList.Clear();
  fdEdxArray.clear();
}

void KBHelixTrack::Print(Option_t *option) const
{
  TString opts = TString(option);

  if (opts.Index("s")>=0) {
    TString trackID = Form("%d",fTrackID);
    TString parentID = Form("%d",fParentID);
    if (fTrackID < 0) trackID = "x";
    if (fParentID < 0) parentID = "x";

    kr_info(0) << GetFitStatusString() << "-" << trackID << "(" << parentID << ")"
            << " | p=" << Momentum().Mag() << "[MeV]" << endl;
    return;
  }

  TString center = "("+TString::Itoa(fI,10)+", "+TString::Itoa(fJ,10)+")";

       if (fA == KBVector3::kX) center = TString("(y,z): ")+center;
  else if (fA == KBVector3::kY) center = TString("(z,x): ")+center;
  else if (fA == KBVector3::kZ) center = TString("(x,y): ")+center;

  kb_raw << left;
  kr_info(0) << setw(13) << "Track ID"     << " : " << fTrackID << endl;
  kr_info(0) << setw(13) << "Parent ID"    << " : " << fParentID << endl;
  kr_info(0) << setw(13) << "Fit Status"   << " : " << GetFitStatusString() << endl;
  kr_info(0) << setw(13) << "# of Hits"    << " : " << fHitList.GetNumHits() << endl;

  if (fFitStatus != KBHelixTrack::kHelix && fFitStatus != KBHelixTrack::kGenfitTrack)
    return;

  kr_info(0) << setw(13) << "Center Axis"  << " : " << fA << endl;
  kr_info(0) << setw(13) << "Helix Center" << " : " << center << " [mm]" << endl;
  kr_info(0) << setw(13) << "Helix Radius" << " : " << fR << " [mm]" << endl;
  kr_info(0) << setw(13) << "Dip Angle"    << " : " << DipAngle() << endl;
  kr_info(0) << setw(13) << "Fit RMS-w/h"  << " : " << fRMSR << " / " << fRMST << " [mm]" << endl;
  kr_info(0) << setw(13) << "Charge"       << " : " << fHitList.GetW() << " [ADC]" << endl;;
  kr_info(0) << setw(13) << "Track Length" << " : " << TrackLength() << " [mm]" << endl;;
  kr_info(0) << setw(13) << "Momentum"     << " : " << Momentum().Mag() << " [MeV]" << endl;;

  if (fFitStatus == KBHelixTrack::kGenfitTrack) {
    kr_info(0) << setw(13) << "GF-Momentum"  << " : " << fGenfitMomentum << " [MeV]" << endl;;
    kr_info(0) << setw(13) << "dEdx (70 %)"  << " : " << GetdEdxWithCut(0, 0.7) << " [ADC/mm]" << endl;;
  }

  if (opts.Index(">")>=0)
    fHitList.PrintHits(1);
}

void KBHelixTrack::Copy(TObject &obj) const
{
  TObject::Copy(obj);
  auto track = (KBHelixTrack &) obj;

  track.SetFitStatus(fFitStatus);
  track.SetHelix(fI,fJ,fR,fS,fK,fT,fH,fA);
  track.SetIsPositiveChargeParticle(fIsPositiveChargeParticle);

  track.SetTrackID(fTrackID);
  track.SetParentID(fParentID);
  track.SetGenfitID(fGenfitID);
  track.SetGenfitMomentum(fGenfitMomentum);

  auto numHits = fHitList.GetNumHits();
  for (auto i=0; i<numHits; ++i)
    track.AddHit(fHitList.GetHit(i));
}

#ifdef ACTIVATE_EVE
bool KBHelixTrack::DrawByDefault() { return true; }
bool KBHelixTrack::IsEveSet() { return false; }

TEveElement *KBHelixTrack::CreateEveElement()
{
  auto element = new TEveLine();

  return element;
}

void KBHelixTrack::SetEveElement(TEveElement *element)
{
  auto line = (TEveLine *) element;
  line -> SetElementName("HelixTrack");
  line -> Reset();

  line -> SetElementName(Form("HelixTrack%d",fTrackID));

  if (fParentID > -1)
    line -> SetLineColor(kPink);
  else
    line -> SetLineColor(kGray);

  Double_t dalpha = TMath::Abs(fH-fT)/200.;
  Double_t t, h;
  if (fT < fH) {
    t = fT;
    h = fH;
  } else {
    h = fT;
    t = fH;
  }

  for (Double_t alpha = t; alpha < h; alpha += dalpha) {
    auto pos = PositionAtAlpha(alpha);
    line -> SetNextPoint(pos.X(), pos.Y(), pos.Z());
  }
}

void KBHelixTrack::AddToEveSet(TEveElement *)
{
}
#endif

bool KBHelixTrack::Fit()
{
  auto helix = fHitList.FitHelix(fA);
  if (helix.GetRMS() < 0)
    return false;

  SetIsHelix();
  KBGeoHelix::SetHelix(helix.GetI(), helix.GetJ(), helix.GetR(), helix.GetS(),
                       helix.GetK(), helix.GetT(), helix.GetH(), helix.GetA());
  return true;
}

bool KBHelixTrack::FitPlane()
{
  auto plane = fHitList.FitPlane();
  if (plane.GetRMS() > 0) {
    SetIsPlane();
    SetPlaneNormal(plane.GetNormal());
    SetRMS(plane.GetRMS());
    return true;
  }

  auto line = fHitList.FitLine();
  if (line.GetRMS() > 0) {
    SetIsLine();
    SetLineDirection(line.Direction());
    SetRMS(line.GetRMS());
    return true;
  }

  return false;
}

void KBHelixTrack::AddHit(KBHit *hit)
{
  fHitList.AddHit(hit);
}

void KBHelixTrack::RemoveHit(KBHit *hit)
{
  fHitList.RemoveHit(hit);
}

void KBHelixTrack::DeleteHits()
{
  auto hits = fHitList.GetHitArray();
  for (auto hit : *hits)
    delete hit;

  fHitList.Clear();
}

void KBHelixTrack::SortHits(bool increasing)
{
  auto hits = fHitList.GetHitArray();
  if (increasing) {
    auto sorting = KBHitSortByIncreasingLength(this);
    sort(hits->begin(), hits->end(), sorting);
  } else {
    auto sorting = KBHitSortByDecreasingLength(this);
    sort(hits->begin(), hits->end(), sorting);
  }
}

void KBHelixTrack::SortHitsByTimeOrder() { SortHits(fIsPositiveChargeParticle); }

void KBHelixTrack::FinalizeHits()
{
  auto hits = fHitList.GetHitArray();
  for (auto hit : *hits)
    hit -> SetTrackID(fTrackID);

  PropagateMC();
}

void KBHelixTrack::SetFitStatus(KBFitStatus value)  { fFitStatus = value; }
void KBHelixTrack::SetIsBad()          { fFitStatus = KBHelixTrack::kBad; }
void KBHelixTrack::SetIsLine()         { fFitStatus = KBHelixTrack::kLine; }
void KBHelixTrack::SetIsPlane()        { fFitStatus = KBHelixTrack::kPlane; }
void KBHelixTrack::SetIsHelix()        { fFitStatus = KBHelixTrack::kHelix; }
void KBHelixTrack::SetIsGenfitTrack()  { fFitStatus = KBHelixTrack::kGenfitTrack; }

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

/**
 * LINE
 *
 * If the track is fitted to plane by FitLine(),
 * 1. SetIsLine(), SetLineDirection is called
 * 2. The direction vector is set to (fI, fJ, fR)
 * 3. Only then methos GetLineDirection(), PerpLine(TVector3) can be used
 * 4. These methods should not be used otherwise (such as, after the container is written)
 */
void KBHelixTrack::SetLineDirection(TVector3 dir) { fI = dir.X(); fJ = dir.Y(); fR = dir.Z(); }
KBVector3 KBHelixTrack::GetLineDirection()   const { return KBVector3(fA, fI, fJ, fR); }
KBVector3 KBHelixTrack::PerpLine(TVector3 p) const
{
  KBVector3 mean = fHitList.GetMean(fA);
  KBVector3 dir = GetLineDirection();

  KBVector3 pMinusMean = KBVector3(p,fA) - mean;
  KBVector3 pMinusMeanUnit = KBVector3(pMinusMean.Unit(), fA);
  Double_t cosine = pMinusMeanUnit.Dot(dir);
  dir.SetMag(pMinusMean.Mag()*cosine);

  return dir - pMinusMean;
}

/**
 * PLANE
 *
 * If the track is fitted to plane by FitPlane(),
 * 1. SetIsPlane(), SetPlaneNormal is called
 * 2. The normal vector is set to (fI, fJ, fR)
 * 3. Only then methos GetPlaneNormal(), PerpPlane(TVector3) can be used
 * 4. These methods should not be used otherwise (such as, after the container is written)
 */
void KBHelixTrack::SetPlaneNormal(TVector3 norm)    { fI = norm.X(); fJ = norm.Y(); fR = norm.Z(); }
KBVector3 KBHelixTrack::GetPlaneNormal()      const { return KBVector3(fA, fI, fJ, fR); }
KBVector3 KBHelixTrack::PerpPlane(TVector3 p) const
{
  KBVector3 normal = GetPlaneNormal();
  KBVector3 mean = fHitList.GetMean(fA);
  Double_t perp = abs(normal * KBVector3(p, fA) - normal * mean) / sqrt(normal * normal);
  return perp * normal;
}

void KBHelixTrack::SetHelixCenter(Double_t i, Double_t j) { fI = i; fJ = j; }
void KBHelixTrack::SetHelixRadius(Double_t r)             { fR = r; }
void KBHelixTrack::SetKInitial(Double_t k)                { fK = k; }
void KBHelixTrack::SetAlphaSlope(Double_t s)              { fS = s; }
void KBHelixTrack::SetAlphaHead(Double_t alpha)           { fH = alpha; }
void KBHelixTrack::SetAlphaTail(Double_t alpha)           { fT = alpha; }
void KBHelixTrack::SetReferenceAxis(KBVector3::Axis ref)  { fA = ref; }

TVector3 KBHelixTrack::GetMean()         const { return fHitList.GetMean(); }
Double_t KBHelixTrack::GetHelixCenterI() const { return fI; }
Double_t KBHelixTrack::GetHelixCenterJ() const { return fJ; }
Double_t KBHelixTrack::GetHelixRadius()  const { return fR; }
Double_t KBHelixTrack::GetKInitial()     const { return fK; }
Double_t KBHelixTrack::GetAlphaSlope()   const { return fS; }
Double_t KBHelixTrack::GetChargeSum()    const { return fHitList.GetW(); }
KBVector3::Axis KBHelixTrack::GetReferenceAxis() const { return fA; }

void KBHelixTrack::SetIsPositiveChargeParticle(Bool_t val)  { fIsPositiveChargeParticle = val; }
Bool_t KBHelixTrack::IsPositiveChargeParticle()  const { return fIsPositiveChargeParticle; }


/**
 * GENFIT TRACK
 *
 * If the track is fitted by GENFIT SUCCESSFULLY,
 * SetIsGenfitTrack() is called and the methods
 */
 void KBHelixTrack::SetGenfitID(Int_t idx) { fGenfitID = idx; }
Int_t KBHelixTrack::GetGenfitID() const { return fGenfitID; }

void KBHelixTrack::SetGenfitMomentum(Double_t p) { fGenfitMomentum = p; }
Double_t KBHelixTrack::GetGenfitMomentum() const { return fGenfitMomentum; }

void KBHelixTrack::DetermineParticleCharge(TVector3 vertex)
{
  Double_t lHead = ExtrapolateToAlpha(fH);
  Double_t lTail = ExtrapolateToAlpha(fT);

  TVector3 q;
  Double_t alpha;
  Double_t lVertex = ExtrapolateToPointAlpha(vertex, q, alpha);

  if (std::abs(lVertex - lTail) > std::abs(lVertex - lHead)) {
    auto a = fT;
    fT = fH;
    fH = a;
  }

  if (fT < fH) {
    if (fS > 0) fIsPositiveChargeParticle = false;
    else        fIsPositiveChargeParticle = true;
  }
  else {
    if (fS > 0) fIsPositiveChargeParticle = true;
    else        fIsPositiveChargeParticle = false;
  }
}


/*
 * HITS
 */
Int_t KBHelixTrack::GetNumHits() const { return fHitList.GetNumHits(); }
KBHit *KBHelixTrack::GetHit(Int_t idx) const { return fHitList.GetHit(idx); }
std::vector<KBHit *> *KBHelixTrack::GetHitArray() { return fHitList.GetHitArray(); }
Int_t KBHelixTrack::GetNumHitIDs() const { return fHitList.GetNumHits(); }
Int_t KBHelixTrack::GetHitID(Int_t idx) const { return fHitList.GetHitID(idx); }
std::vector<Int_t> *KBHelixTrack::GetHitIDArray() { return fHitList.GetHitIDArray(); }

/*
 * dE/dx
 */
std::vector<Double_t> *KBHelixTrack::GetdEdxArray() { return &fdEdxArray; }
Double_t KBHelixTrack::GetdEdxWithCut(Double_t lowR, Double_t highR) const
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

/*
 * EXTRAPOLATES
 */
Double_t 
KBHelixTrack::ExtrapolateToAlpha(Double_t alpha) const 
{
  return alpha * fR / CosDip();
}

Double_t
KBHelixTrack::ExtrapolateToAlpha(Double_t alpha, TVector3 &pointOnHelix) const
{
  KBVector3 pointOnHelix0(fA);
  pointOnHelix0.SetIJK(fR*TMath::Cos(alpha)+fI, fR*TMath::Sin(alpha)+fJ, alpha*fS + fK);
  pointOnHelix = pointOnHelix0.GetXYZ();
  Double_t length = alpha * fR / CosDip();

  return length;
}

Double_t
KBHelixTrack::ExtrapolateToPointAlpha(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const
// TODO
{
  KBVector3 pointGiven2(pointGiven,fA);
  Double_t alpha0 = TMath::ATan2(pointGiven2.J()-fJ, pointGiven2.I()-fI);

  KBVector3 point0(fA);
  point0.SetIJK(fR*TMath::Cos(alpha0)+fI, fR*TMath::Sin(alpha0)+fJ, alpha0*fS+fK);
  Double_t k0 = std::abs(point0.K() - pointGiven2.K());

  Double_t k1; 
  Double_t alpha1 = alpha0;
  KBVector3 point1 = point0;

  Double_t kLengthInPeriod = std::abs(KLengthInPeriod());
  if (kLengthInPeriod > 3*fRMST && kLengthInPeriod > 5 && std::abs(DipAngle()) < 1.5)
  {
    Int_t count = 0;
    while(1)
    {
      alpha1 = alpha1 + 2*TMath::Pi();
      point1.SetK(point1.K() + 2*TMath::Pi()*fS);
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
      point1.SetK(point1.K() - 2*TMath::Pi()*fS);
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
  Double_t length = alpha0 * fR / CosDip();

  return length;
}

Double_t
KBHelixTrack::ExtrapolateToPointK(TVector3 pointGiven, TVector3 &pointOnHelix, Double_t &alpha) const
{
  KBVector3 pointGiven2(pointGiven, fA);
  alpha = (pointGiven2.K() - fK)/fS;

  KBVector3 ph(fA, fR*TMath::Cos(alpha)+fI, fR*TMath::Sin(alpha)+fJ, pointGiven2.K());
  pointOnHelix = ph.GetXYZ();

  Double_t length = alpha * fR / CosDip();
  return length;
}

bool
KBHelixTrack::CheckExtrapolateToI(Double_t i) const
{
  Double_t iRef = fI - i;
  Double_t mult = (iRef + fR) * (iRef - fR);
  if (mult > 0)
    return false;
  return true;
}

bool
KBHelixTrack::CheckExtrapolateToJ(Double_t j) const
{
  Double_t jRef = fJ - j;
  Double_t mult = (jRef + fR) * (jRef - fR);
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

  Double_t jOff = sqrt(fR * fR - (i - fI) * (i - fI));
  Double_t j1 = fJ + jOff;
  Double_t j2 = fJ - jOff;

  alpha1 = TMath::ATan2(j1-fJ, i-fI);
  Double_t alpha1Temp = alpha1;
  Double_t d1Cand = std::abs(alpha1Temp-fH);
  Double_t d1Temp = d1Cand;

  while (1) {
    alpha1Temp = alpha1 + 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fH);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  while (1) {
    alpha1Temp = alpha1 - 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fH);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  pointOnHelix1 = PositionAtAlpha(alpha1);

  alpha2 = TMath::ATan2(j2-fJ, i-fI);
  Double_t alpha2Temp = alpha2;
  Double_t d2Cand = std::abs(alpha2Temp-fT);
  Double_t d2Temp = d2Cand;

  while (1) {
    alpha2Temp = alpha2 + 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fT);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  while (1) {
    alpha2Temp = alpha2 - 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fT);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  pointOnHelix2 = PositionAtAlpha(alpha2);

  return true;
}

bool
KBHelixTrack::ExtrapolateToJ(Double_t j,
    TVector3 &pointOnHelix1, Double_t &alpha1,
    TVector3 &pointOnHelix2, Double_t &alpha2) const
{
  if (CheckExtrapolateToJ(j) == false)
    return false;

  Double_t iOff = sqrt(fR * fR - (j - fJ) * (j - fJ));
  Double_t i1 = fI + iOff;
  Double_t i2 = fI - iOff;

  alpha1 = TMath::ATan2(j-fJ, i1-fI);
  Double_t alpha1Temp = alpha1;
  Double_t d1Cand = std::abs(alpha1Temp-fH);
  Double_t d1Temp = d1Cand;

  while (1) {
    alpha1Temp = alpha1 + 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fH);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  while (1) {
    alpha1Temp = alpha1 - 2*TMath::Pi();
    d1Temp = std::abs(alpha1Temp-fH);
    if (d1Temp >= d1Cand)
      break;
    else {
      alpha1 = alpha1Temp;
      d1Cand = d1Temp;
    }
  }
  pointOnHelix1 = PositionAtAlpha(alpha1);

  alpha2 = TMath::ATan2(j-fJ, i2-fI);
  Double_t alpha2Temp = alpha2;
  Double_t d2Cand = std::abs(alpha2Temp-fT);
  Double_t d2Temp = d2Cand;

  while (1) {
    alpha2Temp = alpha2 + 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fT);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  while (1) {
    alpha2Temp = alpha2 - 2*TMath::Pi();
    d2Temp = std::abs(alpha2Temp-fT);
    if (d2Temp >= d2Cand)
      break;
    else {
      alpha2 = alpha2Temp;
      d2Cand = d2Temp;
    }
  }
  pointOnHelix2 = PositionAtAlpha(alpha2);

  return true;
}

bool
KBHelixTrack::ExtrapolateToI(Double_t x, Double_t alphaRef, TVector3 &pointOnHelix) const
{
  if (CheckExtrapolateToI(x) == false)
    return false;

  Double_t zOff = sqrt(fR * fR - (x - fI) * (x - fI));
  Double_t z1 = fJ + zOff;

  Double_t alpha = TMath::ATan2(z1-fJ, x-fI);
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
  pointOnHelix = PositionAtAlpha(alpha);

  return true;
}

bool
KBHelixTrack::ExtrapolateToJ(Double_t j, Double_t alphaRef, TVector3 &pointOnHelix) const
{
  if (CheckExtrapolateToJ(j) == false)
    return false;

  Double_t iOff = sqrt(fR * fR - (j - fJ) * (j - fJ));
  Double_t i1 = fI + iOff;

  Double_t alpha = TMath::ATan2(j-fJ, i1-fI);
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
  pointOnHelix = PositionAtAlpha(alpha);

  return true;
}

bool
KBHelixTrack::ExtrapolateToJ(Double_t j, TVector3 &pointOnHelix) const
{
  TVector3 position1, position2;
  Double_t alpha1, alpha2;

  if (ExtrapolateToJ(j, position1, alpha1, position2, alpha2) == false)
    return false;

  Double_t alphaMid = (fH + fT)/2;

  if (std::abs(alpha1 - alphaMid) < std::abs(alpha2 - alphaMid))
    pointOnHelix = position1;
  else
    pointOnHelix = position2;

  return true;
}

TVector3 
KBHelixTrack::ExtrapolateHead(Double_t length) const
{
  Double_t alpha = fH;
  Double_t dAlpha = std::abs(AlphaAtTravelLength(length));

  if (fH > fT)
    alpha += dAlpha;
  else
    alpha -= dAlpha;

  return PositionAtAlpha(alpha);
}

TVector3 
KBHelixTrack::ExtrapolateTail(Double_t length) const
{
  Double_t alpha = fT;
  Double_t dAlpha = std::abs(AlphaAtTravelLength(length));

  if (fT > fH)
    alpha += dAlpha;
  else
    alpha -= dAlpha;

  return PositionAtAlpha(alpha);
}

TVector3 
KBHelixTrack::InterpolateByRatio(Double_t r) const
{
  TVector3 q;
  ExtrapolateToAlpha(r*fH+(1-r)*fT, q);
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
  Double_t lHead = ExtrapolateToAlpha(fH);
  Double_t lTail = ExtrapolateToAlpha(fT);
  Double_t lOff = lHead;
  if (lHead > lTail)
    lOff = lTail;

  Double_t alpha;
  Double_t length = ExtrapolateToPointAlpha(p, q, alpha);

  KBVector3 p2(p, fA);
  KBVector3 q2(q, fA);

  Double_t di = p2.I() - fI;
  Double_t dj = p2.J() - fJ;
  Double_t dr = sqrt(di*di + dj*dj) - fR;
  Double_t dk = p2.K() - q2.K();

  m = TVector3(dr, dk/CosDip(), length+dk*TMath::Sin(DipAngle()) - lOff);

  return alpha * fR / CosDip(); 
}

Double_t 
KBHelixTrack::Continuity(Double_t &totalLength, Double_t &continuousLength)
{
  auto hits = fHitList.GetHitArray();
  Int_t numHits = hits->size();
  if (numHits < 2) 
    return -1;

  SortHits();

  Double_t total = 0;
  Double_t continuous = 0;
  TVector3 before = Map(hits->at(0)->GetPosition());

  for (auto iHit = 1; iHit < numHits; iHit++) 
  {
    TVector3 current = Map(hits->at(iHit)->GetPosition());
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

/*
 * KBTrackret ESSENTIAL
 */
TVector3 KBHelixTrack::Momentum(Double_t B) const
{
  if (!IsHelix() && !IsGenfitTrack())
    return TVector3(-999,-999,-999);

  auto mom = Direction(fT);

  if (CosDip() < 0.01)
    return TMath::Abs(fR/0.01*0.3*B)*mom;

  return TMath::Abs(fR/CosDip() * 0.3 * B) * mom;
}
TVector3 KBHelixTrack::PositionAtHead() const { return PositionAtAlpha(fH); }
TVector3 KBHelixTrack::PositionAtTail() const { return PositionAtAlpha(fT); }
Double_t KBHelixTrack::TrackLength()    const { return std::abs(GetH() - GetT()) * fR / CosDip(); }

TVector3 KBHelixTrack::ExtrapolateTo(TVector3 point) const
{
  auto alpha = AlphaAtPosition(point);
  TVector3 position;
  ExtrapolateToAlpha(alpha, position);

  return position;
}

TVector3 KBHelixTrack::ExtrapolateByRatio(Double_t r) const { return InterpolateByRatio(r); }
TVector3 KBHelixTrack::ExtrapolateByLength(Double_t l) const { return InterpolateByLength(l); }
Double_t KBHelixTrack::LengthAt(TVector3 point) const
{
  auto alpha = AlphaAtPosition(point);
  return ExtrapolateToAlpha(alpha);
}
