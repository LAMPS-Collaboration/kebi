#ifndef KBCHANNEL_HH
#define KBCHANNEL_HH

#include "KBChannelHit.hh"

#include "TObject.h"
#include "TObjArray.h"

class KBChannel : public TObject
{
  public:
    KBChannel() {};
    virtual ~KBChannel() {}

    void  SetID(Int_t id);
    Int_t GetID() const;

    void AddChannelHit(KBChannelHit *channelHit);
    TObjArray *GetChannelHitArray();

  protected:
    Int_t fID = -1;

    TObjArray fChannelHitArray; //!

  ClassDef(KBChannel, 1)
};

#endif
