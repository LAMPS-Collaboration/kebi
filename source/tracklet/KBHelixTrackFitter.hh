#ifndef KBHELIXFITTER_HH
#define KBHELIXFITTER_HH

#include <vector>
#include "KBHit.hh"
#include "KBHelixTrack.hh"
#include "KBODRFitter.hh"
#include "KBTracklet.hh"
#include "KBTrackFitter.hh"

class KBHelixTrack;

class KBHelixTrackFitter : public KBTrackFitter
{
  public:
    KBHelixTrackFitter() : fODRFitter(new KBODRFitter()) {}
    ~KBHelixTrackFitter() {};

    bool Fit(KBTracklet *track);

    bool FitPlane(KBHelixTrack *track);

  private:
    KBODRFitter *fODRFitter;

  ClassDef(KBHelixTrackFitter, 1)
};

#endif
