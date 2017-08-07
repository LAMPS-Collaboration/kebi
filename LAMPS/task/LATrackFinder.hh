#ifndef LATRACKFINDER_HH
#define LATRACKFINDER_HH

#include "KBTrackFinder.hh"
#include "KBHelixTrack.hh"
#include "KBHelixTrackFitter.hh"

#include "TClonesArray.h"

#include "LATpc.hh"
#include "LAPadPlane.hh"

#include <vector>

typedef std::vector<KBHit*> vKBHit;

class LATrackFinder : public KBTrackFinder
{
  public:
    LATrackFinder() {}
    virtual ~LATrackFinder() {}

    void Init();

    void SetTpc(LATpc *tpc) { fTpc = tpc; }

    /**
     * @param in   Array of KBHit
     * @param out  Array of KBTracklet
     */
    virtual void FindTrack(TClonesArray *in, TClonesArray *out);

  private:
    KBHelixTrack *NewTrack();
    bool InitTrack(KBHelixTrack* track);
    bool TrackContinuum(KBHelixTrack *track);
    bool TrackExtrapolation(KBHelixTrack *track);
    bool TrackConfirmation(KBHelixTrack* track);

    Int_t CheckHitOwner(KBHit *hit);
    Double_t Correlate(KBHelixTrack *track, KBHit *hit, Double_t scale=1);
    Double_t CorrelateSimple(KBHelixTrack *track, KBHit *hit);
    bool LengthAlphaCut(KBHelixTrack *track, Double_t dLength);

    bool ConfirmHits(KBHelixTrack* track, bool &tailToHead);
    bool AutoBuildByExtrapolation(KBHelixTrack *track, bool &buildHead, Double_t &extrapolationLength);
    bool AutoBuildByInterpolation(KBHelixTrack *track, bool &tailToHead, Double_t &extrapolationLength, Double_t scale);
    bool AutoBuildAtPosition(KBHelixTrack *track, TVector3 p, bool &tailToHead, Double_t &extrapolationLength, Double_t scale=1);
    bool TrackQualityCheck(KBHelixTrack *track);

  private:
    LATpc *fTpc = nullptr;
    LAPadPlane *fPadPlane = nullptr;
    TClonesArray *fTrackArray = nullptr;
    KBHelixTrackFitter *fFitter = nullptr;

    vKBHit *fCandHits = nullptr;
    vKBHit *fGoodHits = nullptr;
    vKBHit *fBadHits  = nullptr;

    Double_t fDefaultScale;
    Double_t fTrackWCutLL;  ///< Track width cut low limit
    Double_t fTrackWCutHL;  ///< Track width cut high limit
    Double_t fTrackHCutLL;  ///< Track height cut low limit
    Double_t fTrackHCutHL;  ///< Track height cut high limit

    Double_t fPadD;

  ClassDef(LATrackFinder, 1)
};

#endif
