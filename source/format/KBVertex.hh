#ifndef KBVERTEX_HH
#define KBVERTEX_HH

#include "TVector3.h"

#include "KBContainer.hh"
#include "KBTracklet.hh"

#include <vector>
#include <iostream>
using namespace std;

class KBVertex : public KBContainer
{
  public:
    KBVertex();
    virtual ~KBVertex() {}

    void Clear(Option_t *option = "");

    void SetPosition(TVector3 v) { fPosition = v; }
    TVector3 GetPosition() const { return fPosition; }

    void AddTrack(KBTracklet* hit);

    vector<Int_t>       *GetTrackIDArray() { return &fTrackIDArray; }
    vector<KBTracklet*> *GetTrackArray()   { return &fTrackArray; }

    Int_t GetNumTracks() const { return fTrackIDArray.size(); }

#ifdef ACTIVATE_EVE
    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *);
    virtual void AddToEveSet(TEveElement *eveSet);
#endif

  private:
    TVector3 fPosition;

    vector<Int_t>       fTrackIDArray;
    vector<KBTracklet*> fTrackArray; //!

  ClassDef(KBVertex, 1)
};

#endif
