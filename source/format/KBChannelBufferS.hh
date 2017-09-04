#ifndef KBCHANNELBUFFERS_HH
#define KBCHANNELBUFFERS_HH

#include "KBChannelBufferX.hh"
#include "TArrayS.h"
#include "TH1S.h"

class KBChannelBufferS : public KBChannelBufferX, public TArrayS
{
  public:
    KBChannelBufferS() {};
    virtual ~KBChannelBufferS() {}

    TH1 *GetHist(bool update = true);

  ClassDef(KBChannelBufferS, 1)
};

#endif
