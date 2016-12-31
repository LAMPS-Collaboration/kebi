#include "KBMCEventGenerator.hh"
#include "KBMCTrack.hh"
#include "TSystem.h"

KBMCEventGenerator::KBMCEventGenerator()
{
}

KBMCEventGenerator::KBMCEventGenerator(TString fileName)
{
  fInputFile.open(fileName.Data());
  fInputFile >> fNumEvents;
}

KBMCEventGenerator::~KBMCEventGenerator()
{
  if(fInputFile.is_open()) fInputFile.close();
}

bool KBMCEventGenerator::ReadNextEvent(Double_t &vx, Double_t &vy, Double_t &vz)
{
  Int_t eventID;
  if (!(fInputFile >> eventID >> fNumTracks >> vx >> vy >> vz))
    return false;

  fCurrentTrackID = 0;
  return true;
}
bool KBMCEventGenerator::ReadNextTrack(Int_t &pdg, Double_t &px, Double_t &py, Double_t &pz)
{
  if (fCurrentTrackID >= fNumTracks)
    return false;

  fInputFile >> pdg >> px >> py >> pz;
  fCurrentTrackID++;

  return true;
}
