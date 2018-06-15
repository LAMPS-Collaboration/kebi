#include "KBLinearTrackFitter.hh"
#include "KBLinearTrack.hh"
#include <float.h>

ClassImp(KBLinearTrackFitter)

KBLinearTrackFitter* KBLinearTrackFitter::fInstance = nullptr;

KBLinearTrackFitter* KBLinearTrackFitter::GetFitter() {
  if (fInstance != nullptr)
    return fInstance;
  return new KBLinearTrackFitter();
}

KBLinearTrackFitter::KBLinearTrackFitter()
{
  fInstance = this;
  fODRFitter = KBODRFitter::GetFitter();
}

bool KBLinearTrackFitter::Fit(KBTracklet *track)
{
  fODRFitter -> Reset();

  auto linearTrack = (KBLinearTrack *) track;

  auto hitArray = linearTrack -> GetHitList() -> GetHitArray();
  if (hitArray -> size() < 4)
    return false;

  Double_t xMean = 0, yMean = 0, zMean = 0, chargeSum = 0;
  for (auto hit : *hitArray) {
    xMean += hit -> GetCharge() * hit -> GetX();
    yMean += hit -> GetCharge() * hit -> GetY();
    zMean += hit -> GetCharge() * hit -> GetZ();
    chargeSum += hit -> GetCharge();
  }

  xMean = xMean / chargeSum;
  yMean = yMean / chargeSum;
  zMean = zMean / chargeSum;

  fODRFitter -> SetCentroid(xMean, yMean, zMean);
  for (auto hit : *hitArray)
    fODRFitter -> AddPoint(hit -> GetX(), hit -> GetY(), hit -> GetZ(), hit -> GetCharge());

  fODRFitter -> FitLine();
  auto centroid = fODRFitter -> GetCentroid();
  auto direction = fODRFitter -> GetDirection();

  linearTrack -> SetTrack(centroid, centroid+direction);

  TVector3 posTail;
  TVector3 posHead;
  Double_t lTail = DBL_MAX;
  Double_t lHead = -DBL_MAX;

  for (auto hit : *hitArray) {
    auto l = linearTrack -> LengthAt(hit->GetPosition());
    if (l < lTail) {
      lTail = l;
      posTail = hit -> GetPosition();
    } else if (l > lHead) {
      lHead = l;
      posHead = hit -> GetPosition();
    }
  }

  linearTrack -> SetTrack(posTail, posHead);

  return true;
}
