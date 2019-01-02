//#define DRAW_HELIX_FITTER

#ifdef DRAW_HELIX_FITTER
#include "KBGeoSphere.hh"
#include "KBGeoLine.hh"
#include "TMarker.h"
#endif

#include "KBHelixTrackFitter.hh"

#include <iostream>
using namespace std;

ClassImp(KBHelixTrackFitter)

KBHelixTrackFitter* KBHelixTrackFitter::fInstance = nullptr;

KBHelixTrackFitter* KBHelixTrackFitter::GetFitter() {
  if (fInstance != nullptr)
    return fInstance;
  return new KBHelixTrackFitter();
}

KBHelixTrackFitter::KBHelixTrackFitter()
:KBTrackFitter("KBHelixTrackFitter","KBHelixTrackFitter"), fODRFitter(KBODRFitter::GetFitter())
{
  fInstance = this;
}

bool
KBHelixTrackFitter::FitPlane(KBHelixTrack *track) 
{
  auto hitList = track -> GetHitList();
  if (hitList -> GetNumHits() < 4)
    return false;

  fODRFitter -> Reset();

  fODRFitter -> SetCentroid(hitList -> GetXMean(), hitList -> GetYMean(), hitList -> GetZMean());

  fODRFitter -> SetMatrixA(hitList -> GetCovWXX(), hitList -> GetCovWXY(), hitList -> GetCovWYZ(),
                           hitList -> GetCovWYY(), hitList -> GetCovWYZ(), hitList -> GetCovWZZ());

  fODRFitter -> SetWeightSum(hitList -> GetChargeSum());
  fODRFitter -> SetNumPoints(hitList -> GetNumHits());

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

  fReferenceAxis = track -> GetReferenceAxis();
  auto hitList = track -> GetHitList();

  if (hitList -> GetNumHits() < 3)
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

  auto mean = hitList -> GetMean(fReferenceAxis);
  auto var = hitList -> GetVar(fReferenceAxis);

  Double_t iMean = mean.I();
  Double_t jMean = mean.J();
  Double_t RSR = 2 * sqrt(var.I() + var.J());

  iMean = 0; // TODO @todo
  jMean = 0; // TODO @todo

  Double_t iMapMean = 0;
  Double_t jMapMean = 0;
  Double_t kMapMean = 0;

  auto hitArray = hitList -> GetHitArray();

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

  Double_t weightSum = hitList -> GetChargeSum();
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

  fODRFitter -> ChooseEigenValue(0); KBVector3 lll(fReferenceAxis, fODRFitter -> GetDirection());
  fODRFitter -> ChooseEigenValue(1); KBVector3 mmm(fReferenceAxis, fODRFitter -> GetDirection());
  fODRFitter -> ChooseEigenValue(2); KBVector3 nnn(fReferenceAxis, fODRFitter -> GetDirection());

  nnn.SetK(abs(nnn.K()));
  if (std::abs(nnn.K()) < 1.e-8) {
    track -> SetIsLine();
    return false;
  }

  KBVector3 RSC(fReferenceAxis,iMean,jMean,RSR); // Riemann Sphere Center

  auto kkk = nnn.I()*iMapMean + nnn.J()*jMapMean + nnn.K()*kMapMean;
  auto ddd = abs(nnn.Dot(RSC) - kkk);

  KBVector3 uuu = 1./sqrt(lll.K()*lll.K() + mmm.K()*mmm.K()) * (lll.K()*lll + mmm.K()*mmm);
  KBVector3 vvv = 1./sqrt(uuu.I()*uuu.I() + uuu.J()*uuu.J()) * KBVector3(fReferenceAxis, uuu.I(), uuu.J(), 0);

  if (nnn.Z()*RSR > kkk) //RSR is higher
    ddd = -ddd;

  KBVector3 FCC = RSC + ddd * nnn; // (riemann) Fit circle center

  auto w1 = ddd*nnn + sqrt(RSR*RSR - ddd*ddd)*uuu;
  auto w2 = ddd*nnn - sqrt(RSR*RSR - ddd*ddd)*uuu;
  auto vw1 = w1.Dot(vvv);
  auto vw2 = w2.Dot(vvv);

#ifdef DRAW_HELIX_FITTER
  auto dddk = RSR + ddd * nnn.K();
  auto vFCC = FCC.Dot(vvv);
  auto vuuu = uuu.Dot(vvv);
  auto vddd = ddd*(nnn.Dot(vvv));
  auto vcenter = iMean*vvv.I()+jMean*vvv.J();
  auto kcenter = RSR;
  auto FCR = sqrt(RSR*RSR - ddd*ddd);

  auto gc = new KBGeoSphere(0,vcenter,kcenter,RSR);                              gc->GetCircleYZ()->Draw("al");
  auto ld = new KBGeoLine(0,kcenter,0,vddd,dddk,0);                              ld->CreateArrowXY()->Draw("same>");
  auto m1 = new TMarker(vddd,dddk,20);                                           m1->Draw("samep");
  auto m2 = new TMarker(vFCC,FCC.Z(),25);                                        m2->Draw("samep");
  auto u1 = new KBGeoLine(vddd, dddk, 0, vddd+FCR*vuuu, dddk+FCR*uuu.K(), 0);    u1->CreateArrowXY()->Draw("same>");
  auto u2 = new KBGeoLine(vddd, dddk, 0, vddd-FCR*vuuu, dddk-FCR*uuu.K(), 0);    u2->CreateArrowXY()->Draw("same>");
  auto w1 = new KBGeoLine(vcenter, kcenter, 0, vcenter+ vw1, kcenter+w1.K(), 0); w1->CreateArrowXY()->Draw("same|>");
  auto w2 = new KBGeoLine(vcenter, kcenter, 0, vcenter+ vw2, kcenter+w2.K(), 0); w2->CreateArrowXY()->Draw("same|>");
#endif

  auto psi1 = TMath::ATan2(abs(vw1),-w1.K());
  auto psi2 = TMath::ATan2(abs(vw2),-w2.K());

  auto r1 = RSR * ( TMath::Tan(psi1/2) + TMath::Tan(psi2/2) );
  auto r2 = RSR * ( TMath::Tan(psi1/2) - TMath::Tan(psi2/2) );

  auto helixRadius = r1;
  auto helixCenter = r2 * vvv + RSC;

  if (helixRadius > 1.e+8) {
    track -> SetIsLine();
    return false;
  }

  track -> SetHelixCenter(helixCenter.I(), helixCenter.J());
  track -> SetHelixRadius(helixRadius);
  track -> SetIsHelix();

  if (fReferenceAxis==KBVector3::kX) sort(hitArray->begin(),hitArray->end(),KBHitSortX());
  if (fReferenceAxis==KBVector3::kY) sort(hitArray->begin(),hitArray->end(),KBHitSortY());
  if (fReferenceAxis==KBVector3::kZ) sort(hitArray->begin(),hitArray->end(),KBHitSortZ());

  KBVector3 posHit0(hitArray->at(0)->GetPosition(), fReferenceAxis);
  i = posHit0.I() - helixCenter.I();
  j = posHit0.J() - helixCenter.J();
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
    i = posHit.I() - helixCenter.I();
    j = posHit.J() - helixCenter.J();
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

  track -> SetAlphaHead(alphaMax);
  track -> SetAlphaTail(alphaMin);

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
