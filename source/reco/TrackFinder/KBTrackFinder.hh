#ifndef KBTRACKFINDER_HH
#define KBTRACKFINDER_HH

#include "KBParameterContainerHolder.hh"

class KBTrackFinder : public KBParameterContainerHolder
{
  public:
    KBTrackFinder() {}
    virtual ~KBTrackFinder() {}

    /**
     * @param in   Array of KBHit
     * @param out  Array of KBTracklet
     */
    virtual void FindTrack(TClonesArray *in, TClonesArray *out) = 0;

  ClassDef(KBTrackFinder, 1)
};

#endif
