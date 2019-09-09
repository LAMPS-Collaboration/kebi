#include "KBGeo2DBox.hh"
#include <iomanip>
#include <cmath>

ClassImp(KBGeo2DBox)

KBGeo2DBox::KBGeo2DBox() {}

KBGeo2DBox::KBGeo2DBox(Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
  Set2DBox(x1,x2,y1,y2);
}

void KBGeo2DBox::Print(Option_t *) const
{
  kb_out << std::endl;
  kb_out << "  (3)-----(e2)-----(2) y2=" << fY2 << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << " (e3) [KBGeo2DBox] (e1)" << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << "   |                |" << std::endl;
  kb_out << "  (0)-----(e0)-----(1) y1=" << fY1 << std::endl;
  kb_out << std::left;
  kb_out << "   x1=" << std::setw(13) << fX1 << " x2=" << fX2 << std::endl;
  kb_out << std::right;
}

TVector3 KBGeo2DBox::GetCenter() const { return TVector3((fX1+fX2)/2.,(fY1+fY2)/2.,0); }

void KBGeo2DBox::Set2DBox(Double_t x1, Double_t x2, Double_t y1, Double_t y2)
{
  if (x2 > x1) { fX1 = x1; fX2 = x2; }
  else         { fX2 = x1; fX1 = x2; }

  if (y2 > y1) { fY1 = y1; fY2 = y2; }
  else         { fY2 = y1; fY1 = y2; }
}

Double_t KBGeo2DBox::GetXCenter() const { return (fX1+fX2)/2.; }
Double_t KBGeo2DBox::GetdX() const { return fX2-fX1; }
Double_t KBGeo2DBox::GetX1() const { return fX1; }
Double_t KBGeo2DBox::GetX2() const { return fX2; }

Double_t KBGeo2DBox::GetYCenter() const { return (fY1+fY2)/2.; }
Double_t KBGeo2DBox::GetdY() const { return fY2-fY1; }
Double_t KBGeo2DBox::GetY1() const { return fY1; }
Double_t KBGeo2DBox::GetY2() const { return fY2; }

TVector3 KBGeo2DBox::GetCorner(Int_t idx) const
{
  TVector3 corner;

       if (idx == 0) corner = TVector3(fX1,fY1,0);
  else if (idx == 1) corner = TVector3(fX1,fY2,0);
  else if (idx == 2) corner = TVector3(fX2,fY2,0);
  else if (idx == 3) corner = TVector3(fX2,fY1,0);
  else if (idx == 4) corner = TVector3(fX1,fY1,0);

  return Rotate(corner);
}

TVector3 KBGeo2DBox::GetCorner(Int_t xpm, Int_t ypm) const
{
  Double_t x, y;
  if (xpm < 0) x = fX1; else x = fX2;
  if (ypm < 0) y = fY1; else y = fY2;

  TVector3 corner(x,y,0);

  return Rotate(corner);
}

KBGeoLine KBGeo2DBox::GetEdge(Int_t idx) const
{
  auto c1 = GetCorner(idx);
  auto c2 = GetCorner(idx+1);

  KBGeoLine line(c1.X(),c1.Y(),0, c2.X(),c2.Y(),0);

  return line;
}

KBGeoLine KBGeo2DBox::GetEdge(Int_t xpm, Int_t ypm) const
{
  TVector3 c1;
  TVector3 c2;

  if (ypm == 0) {
    if (xpm < 0) { c1 = GetCorner(0); c2 = GetCorner(1); }
    else         { c1 = GetCorner(3); c2 = GetCorner(2); }
  } else if (xpm == 0) {
    if (ypm < 0)  { c1 = GetCorner(0); c2 = GetCorner(3); }
    else          { c1 = GetCorner(1); c2 = GetCorner(2); }
  }

  KBGeoLine line(c1.X(),c1.Y(),0, c2.X(),c2.Y(),0);

  return line;
}

TGraph *KBGeo2DBox::DrawGraph()
{
  auto graph = new TGraph();
  for (auto i : {0,1,2,3,0}) {
    TVector3 cn = GetCorner(i);
    graph -> SetPoint(graph -> GetN(),cn.X(),cn.Y());
  }

  return graph;
}

bool KBGeo2DBox::IsInside(Double_t x, Double_t y)
{
  TVector3 pos = InvRotate(TVector3(x,y,0));

  if (x>fX1&&x<fX2&&y>fY1&&y<fY2)
    return true;
  return false;
}
