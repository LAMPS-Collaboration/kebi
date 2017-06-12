#ifndef KBVERTEX_HH
#define KBVERTEX_HH

#include "TVector3.h"

#include "KBContainer.hh"
#include "KBTracklet.hh"

#include <vector>
#include <iostream>
using namespace std;

class KBVSSet
{
  public:
    KBVSSet(Int_t i=-1, Double_t x=0, Double_t s=0, Int_t n=0)
    : fI(i), fX(x), fS(s), fN(n) {}

    void Print() { cout << fI << " " << fX << " " << fS << " " << fN << endl; }

    Int_t fI;
    Double_t fX;
    Double_t fS;
    Int_t fN;
};

class KBVertex : public KBContainer
{
  public:
    KBVertex();
    virtual ~KBVertex() {}

    void Clear(Option_t *option = "");

    void SetPosition(TVector3 v) { fPosition = v; }
    TVector3 GetPosition() const { return fPosition; }

    void AddTrack(KBTracklet* hit);
    vector<KBTracklet*> *GetTrackArray() { return &fTrackArray; }
    vector<Int_t> *GetTrackIDArray() { return &fTrackIDArray; }
    Int_t GetNumTracks() const { return fTrackIDArray.size(); }

    void AddSSet(KBVSSet set) { fSSetArray.push_back(set); }
    vector<KBVSSet> *GetSSetArray() { return &fSSetArray; }

    virtual bool DrawByDefault();
    virtual bool IsEveSet();
    virtual TEveElement *CreateEveElement();
    virtual void SetEveElement(TEveElement *);
    virtual void AddToEveSet(TEveElement *eveSet);

  private:
    TVector3 fPosition;

    vector<KBTracklet*> fTrackArray; //!
    vector<Int_t> fTrackIDArray;

    vector<KBVSSet> fSSetArray;

  ClassDef(KBVertex, 1)
};

#endif
