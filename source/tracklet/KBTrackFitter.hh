#ifndef KBTRACKFITTER_HH
#define KBTRACKFITTER_HH

#include "KBTask.hh"
#include "KBTracklet.hh"

class KBTracklet;

class KBTrackFitter : public KBTask
{
  public:
    KBTrackFitter():KBTask("KBTrackFitter","KBTrackFitter") {}
    KBTrackFitter(const char *name, const char *title):KBTask(name,title) {}
    virtual ~KBTrackFitter() {}

    virtual bool Fit(KBTracklet *track) = 0;

  ClassDef(KBTrackFitter, 1)
};

#endif
