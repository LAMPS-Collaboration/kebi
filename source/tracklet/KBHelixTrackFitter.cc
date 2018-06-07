//#define DEBUG_HELIX_FITTER

#include "KBHelixTrackFitter.hh"

#include <iostream>
using namespace std;

ClassImp(KBHelixTrackFitter)

bool
KBHelixTrackFitter::FitPlane(KBHelixTrack *track) 
{
  if (track -> GetNumHits() < 4)
    return false;

  fODRFitter -> Reset();

  fODRFitter -> SetCentroid(track -> GetIMean(),
                            track -> GetJMean(),
                            track -> GetKMean());

  fODRFitter -> SetMatrixA(track -> CovWII(),
                           track -> CovWIJ(),
                           track -> CovWJK(),
                           track -> CovWJJ(),
                           track -> CovWJK(),
                           track -> CovWKK());

  fODRFitter -> SetWeightSum(track -> GetChargeSum());
  fODRFitter -> SetNumPoints(track -> GetNumHits());

  if (fODRFitter -> Solve() == false)
    return false;

  fODRFitter -> ChooseEigenValue(2);

  TVector3 normal = fODRFitter -> GetDirection();

  if (normal.Z() < 1.e-10) {
    track -> SetIsLine();
    fODRFitter -> ChooseEigenValue(0);
    track -> SetLineDirection(fODRFitter -> GetDirection());
    track -> SetRMSH(fODRFitter -> GetRMSLine());
    return true;
  }

  track -> SetIsPlane();
  track -> SetPlaneNormal(normal);
  track -> SetRMSH(fODRFitter -> GetRMSPlane());

  return true;
}

bool
KBHelixTrackFitter::Fit(KBTracklet *tracklet)
{
  auto track = (KBHelixTrack *) tracklet;

  if (track -> GetNumHits() < 3)
    return false;

  Double_t scale = 1;
  Double_t trackLength = track -> TrackLength();
  Double_t meanCharge = track -> GetChargeSum()/track -> GetNumHits();

  if (trackLength < 500.) {
    Double_t scaleTrackLength = 0.5*(500. - trackLength)/500.;
    if (meanCharge < 500.)
      scaleTrackLength *= (1 - (250000. - meanCharge*meanCharge)/250000.);
    scale = 1 + scaleTrackLength;
  }

  fODRFitter -> Reset();

  Double_t iMean = track -> GetIMean();
  Double_t jMean = track -> GetJMean();
  Double_t iCov  = track -> GetICov();
  Double_t jCov  = track -> GetJCov();
  Double_t RSR = 2 * sqrt(iCov + jCov);
#ifdef DEBUG_HELIX_FITTER
  kb_print << "riemann circle radius: " << RSR << endl;
#endif
  iMean = 0; // TODO @todo
  jMean = 0; // TODO @todo

  Double_t iMapMean = 0;
  Double_t jMapMean = 0;
  Double_t kMapMean = 0;

  auto hitArray = track -> GetHitArray();

  Double_t i = 0;
  Double_t j = 0;

  for (auto hit : *hitArray)
  {
    KBVector3 pos(hit -> GetPosition(), fReferenceAxis);
    i = pos.I() - iMean;
    j = pos.J() - jMean;
    Double_t w = hit -> GetCharge();
    w = TMath::Power(w, scale);

    Double_t rEff = sqrt(i*i + j*j) / (2*RSR);
    Double_t denominator = 1 + rEff*rEff;

    Double_t iMap = i / denominator;
    Double_t jMap = j / denominator;
    Double_t kMap = 2 * RSR * rEff * rEff / denominator;

    iMapMean += w * iMap;
    jMapMean += w * jMap;
    kMapMean += w * kMap;
  }

  Double_t weightSum = track -> GetChargeSum();
  iMapMean = iMapMean / weightSum;
  jMapMean = jMapMean / weightSum;
  kMapMean = kMapMean / weightSum;

  fODRFitter -> SetCentroid(iMapMean, jMapMean, kMapMean);
  KBVector3 mapMean(fReferenceAxis, iMapMean, jMapMean, kMapMean);

  for (auto hit : *hitArray)
  {
    KBVector3 pos(hit -> GetPosition(), fReferenceAxis);
    i = pos.I() - iMean;
    j = pos.J() - jMean;
#ifdef DEBUG_HELIX_FITTER
    kb_print << "hit position in circle: " << i << " " << j << endl;
#endif
    Double_t w = hit -> GetCharge();
    w = TMath::Power(w, scale);

    Double_t rEff = sqrt(i*i + j*j) / (2*RSR);
    Double_t denominator = 1 + rEff*rEff;

    Double_t iMap = i / denominator;
    Double_t jMap = j / denominator;
    Double_t kMap = 2 * RSR * rEff * rEff / denominator;

    fODRFitter -> AddPoint(iMap, jMap, kMap, w);
  }

  if (fODRFitter -> Solve() == false)
    return false;

  fODRFitter -> ChooseEigenValue(0); KBVector3 uOnPlane(fReferenceAxis, fODRFitter -> GetDirection());
  fODRFitter -> ChooseEigenValue(1); KBVector3 vOnPlane(fReferenceAxis, fODRFitter -> GetDirection());
  fODRFitter -> ChooseEigenValue(2); KBVector3 nToPlane(fReferenceAxis, fODRFitter -> GetDirection());

  if (std::abs(nToPlane.K()) < 1.e-8) {
    track -> SetIsLine();
    return false;
  }

  KBVector3 RSC(fReferenceAxis,0,0,RSR);
  Double_t tCC = nToPlane.Dot(mapMean - RSC) / nToPlane.Mag2();
  KBVector3 RCC(tCC * nToPlane + RSC, fReferenceAxis);
  Double_t dCS = (RCC - RSC).Mag();
  Double_t RCR = sqrt(RSR*RSR - dCS*dCS);

  Double_t uConst = uOnPlane.K() / sqrt(uOnPlane.K()*uOnPlane.K() + vOnPlane.K()*vOnPlane.K());
  Double_t vConst = sqrt(1-uConst*uConst);

  Double_t ref1 = uConst*uOnPlane.K() + vConst*vOnPlane.K();
  Double_t ref2 = uConst*uOnPlane.K() - vConst*vOnPlane.K();

  if (ref1 < 0) ref1 = -ref1;
  if (ref2 < 0) ref2 = -ref2;
  if (ref1 < ref2) vConst = -vConst;

  KBVector3 toLouu = uConst * uOnPlane + vConst * vOnPlane;
  KBVector3 louu = RCC + toLouu*RCR;
  KBVector3 high = RCC - toLouu*RCR;

  KBVector3 louuInvMap(fReferenceAxis, louu.I()/(1-louu.K()/(2*RSR)), louu.J()/(1-louu.K()/(2*RSR)), 0);
  KBVector3 highInvMap(fReferenceAxis, high.I()/(1-high.K()/(2*RSR)), high.J()/(1-high.K()/(2*RSR)), 0);

  KBVector3 FCC = 0.5 * (louuInvMap + highInvMap);

  Double_t iC = FCC.I() + iMean;
  Double_t jC = FCC.J() + jMean;
  Double_t radius = 0.5 * (louuInvMap - highInvMap).Mag();

  if (radius > 1.e+8) {
    track -> SetIsLine();
    return false;
  }

  track -> SetHelixCenter(iC, jC);
  track -> SetHelixRadius(radius);
#ifdef DEBUG_HELIX_FITTER
  kb_print << "helix center: " << iC << " " << jC << endl;
  kb_print << "helix radius: " << radius << endl;
#endif

  track -> SetIsHelix();

  if (fReferenceAxis==KBVector3::kX) sort(hitArray->begin(),hitArray->end(),KBHitSortX());
  if (fReferenceAxis==KBVector3::kY) sort(hitArray->begin(),hitArray->end(),KBHitSortY());
  if (fReferenceAxis==KBVector3::kZ) sort(hitArray->begin(),hitArray->end(),KBHitSortZ());

  KBVector3 posHit0(hitArray->at(0)->GetPosition(), fReferenceAxis);
  i = posHit0.I() - iC;
  j = posHit0.J() - jC;
  Double_t k = 0;

  Double_t alphaInit = TMath::ATan2(j, i);
  TVector2 iAxis(i,j);
  TVector2 jAxis(-j,i);

  iAxis = iAxis.Unit();
  jAxis = jAxis.Unit();

  Double_t expA  = 0;
  Double_t expA2 = 0;
  Double_t expK  = 0;
  Double_t expAK = 0;

  Double_t alphaStack = 0;
  Double_t alphaLast = 0;

  Double_t alphaMin = alphaInit;
  Double_t alphaMax = alphaInit;

  for (auto hit : *hitArray)
  {
    KBVector3 posHit(hit -> GetPosition(), fReferenceAxis);
    i = posHit.I() - iC;
    j = posHit.J() - jC;
    k = posHit.K();

    TVector2 v(i,j);

    Double_t iRot = v*iAxis;
    Double_t jRot = v*jAxis;

    alphaLast = TMath::ATan2(jRot, iRot);
    if (alphaLast > .5*TMath::Pi() || alphaLast < -.5*TMath::Pi())
    {
      Double_t t0 = alphaLast;

      alphaLast += alphaStack;
      alphaStack += t0;

      iAxis = v;
      jAxis = TVector2(-v.Y(), v.X());

      iAxis = iAxis.Unit();
      jAxis = jAxis.Unit();
    }
    else
      alphaLast += alphaStack;

    alphaLast = alphaLast + alphaInit;

    Double_t w = hit -> GetCharge();

    expA  += w * alphaLast;
    expA2 += w * alphaLast * alphaLast;
    expK  += w * k;
    expAK += w * alphaLast * k;

    if (alphaLast < alphaMin)
      alphaMin = alphaLast;
    if (alphaLast > alphaMax)
      alphaMax = alphaLast;
  }

  if (track -> IsPositiveChargeParticle()) {
    track -> SetAlphaHead(alphaMin);
    track -> SetAlphaTail(alphaMax);
  } else {
    track -> SetAlphaHead(alphaMax);
    track -> SetAlphaTail(alphaMin);
  }

  expA  /= weightSum;
  expA2 /= weightSum;
  expK  /= weightSum;
  expAK /= weightSum;

  Double_t slope  = (expAK - expA*expK) / (expA2 - expA*expA);
  Double_t offset = (expA2*expK - expA*expAK) / (expA2 - expA*expA);

  if (std::isinf(slope)) {
    track -> SetIsLine();
    return false;
  }

  track -> SetAlphaSlope(slope);
  track -> SetKInitial(offset);

  Double_t s1 = 0;
  Double_t s2 = 0;
  for (auto hit : *hitArray)
  {
    TVector3 q = track -> Map(hit -> GetPosition());
    s1 += hit -> GetCharge() * q.X() * q.X();
    s2 += hit -> GetCharge() * q.Y() * q.Y();
  }

  Double_t rmsr = sqrt(s1 / (track -> GetChargeSum() * (1 - 3/hitArray -> size())));
  Double_t rmsk = sqrt(s2 / (track -> GetChargeSum() * (1 - 3/hitArray -> size())));

  track -> SetRMSW(rmsr);
  track -> SetRMSH(rmsk);

  return true;
}
