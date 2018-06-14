#ifndef KBHELIXTRACKFITTER_HH
#define KBHELIXTRACKFITTER_HH

#include <vector>
#include "KBHit.hh"
#include "KBHelixTrack.hh"
#include "KBODRFitter.hh"
#include "KBTracklet.hh"
#include "KBTrackFitter.hh"
#include "KBVector3.hh"

class KBHelixTrack;

class KBHelixTrackFitter : public KBTrackFitter
{
  public:
    static KBHelixTrackFitter* GetFitter();

    KBHelixTrackFitter();
    ~KBHelixTrackFitter() {};

    bool Fit(KBTracklet *track);

    bool FitPlane(KBHelixTrack *track);

    void SetReferenceAxis(KBVector3::Axis axis) { fReferenceAxis  = axis; }

  private:
    KBODRFitter *fODRFitter;
    KBVector3::Axis fReferenceAxis = KBVector3::kZ;

  private:
    static KBHelixTrackFitter *fInstance;

  ClassDef(KBHelixTrackFitter, 1)
};

#endif
