#include "KBVector3.hh"
#include <iostream>

using namespace std;

ClassImp(KBVector3)

void KBVector3::Print(Option_t *) const
{
       if (fReferenceAxis == 2) cout << "[KBVector3] Reference axis : z" << endl;
  else if (fReferenceAxis == 1) cout << "[KBVector3] Reference axis : y" << endl;
  else                          cout << "[KBVector3] Reference axis : x" << endl;
  cout << "          > (i,j,k) = ("<<I()<<","<<J()<<","<<K()<<")" << endl;
  cout << "          > (x,y,z) = ("<<X()<<","<<Y()<<","<<Z()<<")" << endl;
}

void KBVector3::SetReferenceAxis(Short_t referenceAxis)
{
  if (referenceAxis!=2&&referenceAxis!=1&&referenceAxis!=0) {
    cout << "[KBVector3] Error! Reference axis should be one of; 0(x), 1(y), 2(z)" << endl;
    return;
  }
  fReferenceAxis = referenceAxis;
}

Short_t KBVector3::GetReferenceAxis() const { return fReferenceAxis; }

void KBVector3::SetIJKR(Double_t i, Double_t j, Double_t k, Short_t referenceAxis)
{
  fReferenceAxis = referenceAxis;
  SetIJK(i,j,k);
}

void KBVector3::SetIJK(Double_t i, Double_t j, Double_t k)
{
  /* SetI(i); SetJ(j); SetK(k); */

       if (fReferenceAxis == 2)  { SetX(i); SetY(j); SetZ(k); return; }
  else if (fReferenceAxis == 1)  { SetZ(i); SetX(j); SetY(k); return; }
/*else if (fReferenceAxis == 0)*/{ SetY(i); SetZ(j); SetX(k); return; }
}

void KBVector3::SetI(Double_t i)
{
       if (fReferenceAxis == 2) { SetX(i); return; }
  else if (fReferenceAxis == 1) { SetZ(i); return; }
/*else if (fReferenceAxis == 0)*/ SetY(i); return;
}

void KBVector3::SetJ(Double_t j)
{
       if (fReferenceAxis == 2) { SetY(j); return; }
  else if (fReferenceAxis == 1) { SetX(j); return; }
/*else if (fReferenceAxis == 0)*/ SetZ(j); return;
}

void KBVector3::SetK(Double_t k)
{
       if (fReferenceAxis == 2) { SetZ(k); return; }
  else if (fReferenceAxis == 1) { SetY(k); return; }
/*else if (fReferenceAxis == 0)*/ SetX(k); return;
}

Double_t KBVector3::I() const
{
       if (fReferenceAxis == 2)  return X();
  else if (fReferenceAxis == 1)  return Z();
/*else if (fReferenceAxis == 0)*/return Y();
}

Double_t KBVector3::J() const
{
       if (fReferenceAxis == 2)  return Y();
  else if (fReferenceAxis == 1)  return X();
/*else if (fReferenceAxis == 0)*/return Z();
}

Double_t KBVector3::K() const
{
       if (fReferenceAxis == 2)  return Z();
  else if (fReferenceAxis == 1)  return Y();
/*else if (fReferenceAxis == 0)*/return X();
}
