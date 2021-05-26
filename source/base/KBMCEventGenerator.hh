/**
 * @brief MC Event Generator
 * @author JungWoo Lee (Korea Univ.)
 *
 * -# EventGen file must have following format.
 * -# 1) Second line should be "p" or "e": "p" for momentum (Mev/c), "e" for energy (MeV).
 * -# 1) Second line is [nEvents].
 * -# 2) Repeat 3) to 5) [nEvents] times.
 * -# 3) First line of event is 3 components of vertex position in next event.
 * -# 4) Next line is [nTracks].
 * -# 5) Next [nTracks] lines follows [pdg], [px], [py] and [pz].
 *
 * [nEvents]
 * [eventID] [nTracks] [vx] [vy] [vz]
 * [pdg] [px] [py] [pz]
 * [pdg] [px] [py] [pz]
 * ...
 * 
 * [eventID] [nTracks] [vx] [vy] [vz]
 * [pdg] [px] [py] [pz]
 * [pdg] [px] [py] [pz]
 * ...
 * ...
 *
 * example)
 * 10
 * 0 181 0 0 0
 * 1000020040 0.592732 0.259583 3.45222
 * 1000020040 -0.506001 0.292528 2.11579
 * ...
 * 1 58 0 0 100
 * 1000020040 -0.192129 0.0461225 1.57347
 * 2212 -0.0527492 -0.214962 0.696101
 * ...
 * ...
 *
 * @param [nEvents] total number of events
 * @param [eventID] event ID
 * @param [nTracks] total number of tracks
 * @param [vx] x-component of vertex [mm]
 * @param [vy] y-component of vertex [mm]
 * @param [vz] z-component of vertex [mm]
 * @param [pdg] particle PDG code
 * @param [px] x-component of momentum [MeV/c] (option "p") or energy [MeV] (option "e")
 * @param [py] y-component of momentum [MeV/c] (option "p") or energy [MeV] (option "e")
 * @param [pz] z-component of momentum [MeV/c] (option "p") or energy [MeV] (option "e")
 */

#ifndef KBMCEVENTGENGENERATOR_HH
#define KBMCEVENTGENGENERATOR_HH

#include "TVector3.h"
#include "TClonesArray.h"
#include "TRandom3.h"
#include "KBVector3.hh"
#include "KBGear.hh"
#include <fstream>

class KBMCEventGenerator : public KBGear
{
  public:
    KBMCEventGenerator();
    KBMCEventGenerator(TString fileName);
    virtual ~KBMCEventGenerator();

    // read gen
    void AddGenFile(TString fileName);
    bool ReadNextEvent(Double_t &vx, Double_t &vy, Double_t &vz);
    bool ReadNextTrack(Int_t &pdg, Double_t &px, Double_t &py, Double_t &pz);
    void Summary();

    Int_t GetNumEvents() { return fNumEvents; };
    bool ReadMomentumOrEnergy() { return fReadMomentumOrEnergy; }

    // create gen
    const char *CreateGenFile(const char *runName, int runID, int numEvents);
    void AddEvent(int numTracks, double vX, double vY, double vZ);
    void AddTrack(TString particleName, double pX, double pY, double pZ);
    void AddTrack(int pdg, double pX, double pY, double pZ);
    const char *CreateGenRandom(const char *runName, int runID, int numEvents, int numTracks, double vX, double vY, double vZ, TString particleNames, double pVal, double theta1=0, double theta2=180, double phi1=-180, double phi2=180,
        KBVector3::Axis refAxis=KBVector3::kZ);

  private:
    std::ifstream fInputFile;
    bool fReadMomentumOrEnergy = true;
    Int_t fNumEvents = 0;
    Int_t fNumTracks = 0;
    Int_t fCurrentTrackID = -1;
    Int_t fCurrentEventID = -1;

    vector<TString> fInputFileNameArray;
    Int_t fCurrentInputFileIndex = 0;

    std::ofstream fOutputFile;

    TString fGenName;

    TRandom3 fRandom;

  ClassDef(KBMCEventGenerator, 1)
};

#endif
