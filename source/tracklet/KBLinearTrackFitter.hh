#ifndef KBLINEARTRACKFITTER_HH
#define KBLINEARTRACKFITTER_HH

#include "KBTrackFitter.hh"
#include "KBODRFitter.hh"

class KBLinearTrackFitter : public KBTrackFitter
{
  public:
    KBLinearTrackFitter();
    virtual ~KBLinearTrackFitter() {}

    bool Fit(KBTracklet *track);

  private:
    KBODRFitter *fODRFitter;

  ClassDef(KBLinearTrackFitter, 1)
};

#endif
