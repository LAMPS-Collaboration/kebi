#ifndef KBHITLIST_HH
#define KBHITLIST_HH

#include "TObject.h"
#include "KBHit.hh"
#include <vector>
using namespace std;

class KBHitList : public TObject
{
  public:
    KBHitList();
    virtual ~KBHitList() {}

    void AddHit(KBHit* hit);

    vector<KBHit*> *GetHitArray();
    vector<Int_t> *GetHitIDArray();

  private:
    vector<KBHit*> fHitArray; //!
    vector<Int_t> fHitIDArray;

  ClassDef(KBHitList, 1)
};

#endif
