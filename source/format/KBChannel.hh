#ifndef KBCHANNEL_HH
#define KBCHANNEL_HH

#include "KBChannelHit.hh"

#include "TObject.h"
#include "TObjArray.h"
#include "TVector3.h"

class KBChannel : public TObject
{
  public:
    KBChannel() { Clear(); }
    virtual ~KBChannel() {}

    virtual void Clear(Option_t *option = "");

    void  SetID(Int_t id);
    Int_t GetID() const;

    void AddChannelHit(KBChannelHit *channelHit);
    TObjArray *GetChannelHitArray();

    void SetPos1(TVector3 pos);
    TVector3 GetPos1();

    void SetPos2(TVector3 pos);
    TVector3 GetPos2();

  protected:
    Int_t fID = -1;

    TVector3 fPos1; ///< geomentrical components user may concern 1
    TVector3 fPos2; ///< geomentrical components user may concern 2

    TObjArray fChannelHitArray; //!

  ClassDef(KBChannel, 1)
};

#endif
