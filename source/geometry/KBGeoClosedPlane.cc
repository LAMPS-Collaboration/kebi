#include "KBGeoClosedPlane.hh"
#include "TObjArray.h"
#include <cmath>
#include <iostream>


ClassImp(KBGeoClosedPlane)

KBGeoClosedPlane::KBGeoClosedPlane()
{
}

KBGeoClosedPlane::KBGeoClosedPlane(TVector3 c1, TVector3 c2, TVector3 c3, TVector3 c4)
{
  SetClosedPlane(c1,c2,c3,c4);
}

void KBGeoClosedPlane::Print(Option_t *) const
{
  kb_out << "[KBGeoClosedPlane] containing 4 corners:" << std::endl;
  fCorner[0].Print("sr1");
  fCorner[1].Print("sr1");
  fCorner[2].Print("sr1");
  fCorner[3].Print("sr1");
}

void KBGeoClosedPlane::SetClosedPlane(TVector3 c0, TVector3 c1, TVector3 c2, TVector3 c3)
{
  KBVector3 v0(c0);
  KBVector3 v1(c1);
  KBVector3 v2(c2);
  KBVector3 v3(c3);

  auto center = 0.25*(v0+v1+v2+v3);
  auto normal = (v0-center).Cross(v1-center);
  v0.SetSortBy((v0-center).Angle2(v0-center,normal));
  v1.SetSortBy((v1-center).Angle2(v0-center,normal));
  v2.SetSortBy((v2-center).Angle2(v0-center,normal));
  v3.SetSortBy((v3-center).Angle2(v0-center,normal));

  TObjArray array;
  array.Add(&v0);
  array.Add(&v1);
  array.Add(&v2);
  array.Add(&v3);
  array.Sort();

  auto s0 = ((KBVector3 *) array.At(0));
  auto s1 = ((KBVector3 *) array.At(1));
  auto s2 = ((KBVector3 *) array.At(2));
  auto s3 = ((KBVector3 *) array.At(3));

  fCorner[0] = KBVector3(s0->X(),s0->Y(),s0->Z());
  fCorner[1] = KBVector3(s1->X(),s1->Y(),s1->Z());
  fCorner[2] = KBVector3(s2->X(),s2->Y(),s2->Z());
  fCorner[3] = KBVector3(s3->X(),s3->Y(),s3->Z());

  KBGeoPlane::SetPlane(c1,(c1-c2).Cross(c1-c3));
}

TVector3 KBGeoClosedPlane::GetCorner(Int_t idx) const
{
  return fCorner[idx];
}

TGraph *KBGeoClosedPlane::Draw(kbaxis_t axis1, kbaxis_t axis2)
{
  auto graph = new TGraph();
  graph -> SetPoint(0,fCorner[0].At(axis1),fCorner[0].At(axis2));
  graph -> SetPoint(1,fCorner[1].At(axis1),fCorner[1].At(axis2));
  graph -> SetPoint(3,fCorner[3].At(axis1),fCorner[3].At(axis2));
  graph -> SetPoint(2,fCorner[2].At(axis1),fCorner[2].At(axis2));
  graph -> SetPoint(4,fCorner[0].At(axis1),fCorner[0].At(axis2));
  return graph;
}
