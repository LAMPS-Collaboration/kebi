#include "KBVector3.hh"
#include <iostream>

using namespace std;

ClassImp(KBVector3)

void KBVector3::Print(Option_t *) const
{
       if (fReferenceAxis == KBVector3::kZ)  cout << "[KBVector3] Reference axis : z" << endl;
  else if (fReferenceAxis == KBVector3::kY)  cout << "[KBVector3] Reference axis : y" << endl;
  else if (fReferenceAxis == KBVector3::kX)  cout << "[KBVector3] Reference axis : x" << endl;
  else if (fReferenceAxis == KBVector3::kMZ) cout << "[KBVector3] Reference axis : -z" << endl;
  else if (fReferenceAxis == KBVector3::kMY) cout << "[KBVector3] Reference axis : -y" << endl;
  else if (fReferenceAxis == KBVector3::kMX) cout << "[KBVector3] Reference axis : -x" << endl;
  else                                       cout << "[KBVector3] Reference axis : Non" << endl;
  cout << "          > (i,j,k) = ("<<I()<<","<<J()<<","<<K()<<")" << endl;
  cout << "          > (x,y,z) = ("<<X()<<","<<Y()<<","<<Z()<<")" << endl;
}

void KBVector3::Clear(Option_t *)
{
  SetX(-999);
  SetY(-999);
  SetZ(-999);
  fReferenceAxis = KBVector3::kY;
}

void KBVector3::SetReferenceAxis(KBVector3::Axis referenceAxis)
{
  if (referenceAxis!=KBVector3::kZ  &&
      referenceAxis!=KBVector3::kY  &&
      referenceAxis!=KBVector3::kX  &&
      referenceAxis!=KBVector3::kMZ &&
      referenceAxis!=KBVector3::kMY &&
      referenceAxis!=KBVector3::kMX)
  {
    cout << "[KBVector3] Error! Reference axis should be one of: kX(1), kY(2), kZ(3), kMX(4), kMY(5), kMZ(6)" << endl;
    return;
  }
  fReferenceAxis = referenceAxis;
}

KBVector3::Axis KBVector3::GetReferenceAxis() const { return fReferenceAxis; }

Double_t KBVector3::At(KBVector3::Axis axis) const
{
       if (axis == KBVector3::kX) return X();
  else if (axis == KBVector3::kY) return Y();
  else if (axis == KBVector3::kZ) return Z();
  else if (axis == KBVector3::kI) return I();
  else if (axis == KBVector3::kJ) return J();
  else if (axis == KBVector3::kK) return K();

  cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kI(7), kJ(8), kK(9)" << endl;

  return -999;
}

void KBVector3::AddAt(Double_t value, KBVector3::Axis axis)
{
       if (axis == KBVector3::kX) SetX(X()+value);
  else if (axis == KBVector3::kY) SetY(Y()+value);
  else if (axis == KBVector3::kZ) SetZ(Z()+value);
  else if (axis == KBVector3::kI) SetI(I()+value);
  else if (axis == KBVector3::kJ) SetJ(J()+value);
  else if (axis == KBVector3::kK) SetK(K()+value);
  else 
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kI(7), kJ(8), kK(9)" << endl;
}

void KBVector3::SetIJKR(Double_t i, Double_t j, Double_t k, KBVector3::Axis referenceAxis)
{
  if (referenceAxis!=KBVector3::kX  &&
      referenceAxis!=KBVector3::kY  &&
      referenceAxis!=KBVector3::kZ  &&
      referenceAxis!=KBVector3::kMX &&
      referenceAxis!=KBVector3::kMY &&
      referenceAxis!=KBVector3::kMZ)
  {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return;
  }

  fReferenceAxis = referenceAxis;
  SetIJK(i,j,k);
}

void KBVector3::SetIJK(Double_t i, Double_t j, Double_t k)
{
       if (fReferenceAxis == KBVector3::kZ)  { SetX(i); SetY(j); SetZ(k);  return; }
  else if (fReferenceAxis == KBVector3::kY)  { SetZ(i); SetX(j); SetY(k);  return; }
  else if (fReferenceAxis == KBVector3::kX)  { SetY(i); SetZ(j); SetX(k);  return; }
  else if (fReferenceAxis == KBVector3::kMZ) { SetY(i); SetX(j); SetZ(-k); return; }
  else if (fReferenceAxis == KBVector3::kMY) { SetX(i); SetZ(j); SetY(-k); return; }
  else if (fReferenceAxis == KBVector3::kMX) { SetZ(i); SetY(j); SetX(-k); return; }
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return;
  }
}

void KBVector3::SetI(Double_t i)
{
       if (fReferenceAxis == KBVector3::kZ)  { SetX(i); return; }
  else if (fReferenceAxis == KBVector3::kY)  { SetZ(i); return; }
  else if (fReferenceAxis == KBVector3::kX)  { SetY(i); return; }
  else if (fReferenceAxis == KBVector3::kMZ) { SetY(i); return; }
  else if (fReferenceAxis == KBVector3::kMY) { SetX(i); return; }
  else if (fReferenceAxis == KBVector3::kMX) { SetZ(i); return; }
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return;
  }
}

void KBVector3::SetJ(Double_t j)
{
       if (fReferenceAxis == KBVector3::kZ) { SetY(j); return; }
  else if (fReferenceAxis == KBVector3::kY) { SetX(j); return; }
  else if (fReferenceAxis == KBVector3::kX) { SetZ(j); return; }
  else if (fReferenceAxis == KBVector3::kZ) { SetX(j); return; }
  else if (fReferenceAxis == KBVector3::kY) { SetZ(j); return; }
  else if (fReferenceAxis == KBVector3::kX) { SetY(j); return; }
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return;
  }
}

void KBVector3::SetK(Double_t k)
{
       if (fReferenceAxis == KBVector3::kZ)  { SetZ(k);  return; }
  else if (fReferenceAxis == KBVector3::kY)  { SetY(k);  return; }
  else if (fReferenceAxis == KBVector3::kX)  { SetX(k);  return; }
  else if (fReferenceAxis == KBVector3::kMZ) { SetZ(-k); return; }
  else if (fReferenceAxis == KBVector3::kMY) { SetY(-k); return; }
  else if (fReferenceAxis == KBVector3::kMX) { SetX(-k); return; }
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return;
  }
}

Double_t KBVector3::I() const
{
       if (fReferenceAxis == KBVector3::kZ)  return X();
  else if (fReferenceAxis == KBVector3::kY)  return Z();
  else if (fReferenceAxis == KBVector3::kX)  return Y();
  else if (fReferenceAxis == KBVector3::kMZ) return Y();
  else if (fReferenceAxis == KBVector3::kMY) return X();
  else if (fReferenceAxis == KBVector3::kMX) return Z();
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return -999;
  }
}

Double_t KBVector3::J() const
{
       if (fReferenceAxis == KBVector3::kZ)  return Y();
  else if (fReferenceAxis == KBVector3::kY)  return X();
  else if (fReferenceAxis == KBVector3::kX)  return Z();
  else if (fReferenceAxis == KBVector3::kMZ) return X();
  else if (fReferenceAxis == KBVector3::kMY) return Z();
  else if (fReferenceAxis == KBVector3::kMX) return Y();
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return -999;
  }
}

Double_t KBVector3::K() const
{
       if (fReferenceAxis == KBVector3::kZ)  return Z();
  else if (fReferenceAxis == KBVector3::kY)  return Y();
  else if (fReferenceAxis == KBVector3::kX)  return X();
  else if (fReferenceAxis == KBVector3::kMZ) return -Z();
  else if (fReferenceAxis == KBVector3::kMY) return -Y();
  else if (fReferenceAxis == KBVector3::kMX) return -X();
  else {
    cout << "[KBVector3] Error! Reference axis should be one of; kX(1), kY(2), kZ(3), kMX(4), kMX(5), kMX(6)" << endl;
    return -999;
  }
}

TVector3 KBVector3::GetV3IJK() { return TVector3(I(), J(), K()); }
