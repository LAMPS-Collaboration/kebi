#include "KBMCStep.hh"

#include <iostream>
#include <iomanip>
using namespace std;

ClassImp(KBMCStep)

KBMCStep::KBMCStep()
{
}

KBMCStep::~KBMCStep()
{
}

void KBMCStep::Print(Option_t *) const
{
  cout << "KBMCStep (xyz/et):"
       << setw(12) << fX
       << setw(12) << fY
       << setw(12) << fZ << " / "
       << setw(12) << fTime
       << setw(12) << fEdep << endl;
}

void KBMCStep::SetTrackID(Int_t val)  { fTrackID = val; }
void KBMCStep::SetX(Double_t val)     { fX = val; }
void KBMCStep::SetY(Double_t val)     { fY = val; }
void KBMCStep::SetZ(Double_t val)     { fZ = val; }
void KBMCStep::SetTime(Double_t val)  { fTime = val; }
void KBMCStep::SetEdep(Double_t val)  { fEdep = val; }

void KBMCStep::SetMCStep(Int_t trackID, Double_t x, Double_t y, Double_t z, Double_t time, Double_t edep)
{
  fTrackID = trackID;
  fX = x;
  fY = y;
  fZ = z;
  fTime = time;
  fEdep = edep;
}

Int_t KBMCStep::GetTrackID()  const { return fTrackID; }
Double_t KBMCStep::GetX()     const { return fX; }
Double_t KBMCStep::GetY()     const { return fY; }
Double_t KBMCStep::GetZ()     const { return fZ; }
Double_t KBMCStep::GetTime()  const { return fTime; }
Double_t KBMCStep::GetEdep()  const { return fEdep; }