#include "KBTracklet.hh"
#ifdef ACTIVATE_EVE
#include "TEveLine.h"
#endif

ClassImp(KBTracklet)

void KBTracklet::AddHit(KBHit *hit)
{
  fHitList -> AddHit(hit);
}

void KBTracklet::RemoveHit(KBHit *hit)
{
  fHitList -> RemoveHit(hit);
}

#ifdef ACTIVATE_EVE
bool KBTracklet::DrawByDefault() { return true; }
bool KBTracklet::IsEveSet() { return false; }

TEveElement *KBTracklet::CreateEveElement()
{
  auto element = new TEveLine();

  return element;
}

void KBTracklet::SetEveElement(TEveElement *element)
{
  auto line = (TEveLine *) element;
  line -> SetElementName("Tracklet");
  line -> Reset();

  line -> SetElementName(Form("Tracklet%d",fTrackID));

  if (fParentID > -1)
    line -> SetLineColor(kPink);
  else
    line -> SetLineColor(kGray);

  auto dr = 0.02;
  if (dr < 5./TrackLength())
    dr = 5./TrackLength();

  for (Double_t r = 0.; r < 1.0001; r += dr) {
    auto pos = ExtrapolateByRatio(r);
    line -> SetNextPoint(pos.X(), pos.Y(), pos.Z());
  }
}

void KBTracklet::AddToEveSet(TEveElement *)
{
}
#endif

bool KBTracklet::DoDrawOnDetectorPlane()
{
  return true;
}

TGraph *KBTracklet::TrajectoryOnPlane(KBVector3::Axis axis1, KBVector3::Axis axis2)
{
  if (fTrajectoryOnPlane == nullptr) {
    fTrajectoryOnPlane = new TGraph();
    fTrajectoryOnPlane -> SetLineColor(kRed);
  }

  fTrajectoryOnPlane -> Set(0);

  for (Double_t r = 0.; r < 1.001; r += 0.02) {
    auto pos = KBVector3(ExtrapolateByRatio(r),fReferenceAxis);
    fTrajectoryOnPlane -> SetPoint(fTrajectoryOnPlane->GetN(), pos.At(axis1), pos.At(axis2));
  }

  return fTrajectoryOnPlane;
}
