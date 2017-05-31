#ifndef KBTRACKFITTER_HH
#define KBTRACKFITTER_HH

#include "KBTracklet.hh"

class KBTracklet;

class KBTrackFitter
{
  public:
    KBTrackFitter() {}
    virtual ~KBTrackFitter() {}

    virtual bool Fit(KBTracklet *track) = 0;

  ClassDef(KBTrackFitter, 1)
};

#endif
