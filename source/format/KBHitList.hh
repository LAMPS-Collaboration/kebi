#ifndef KBHITLIST_HH
#define KBHITLIST_HH

#include "TObject.h"
#include <vector>
#include <iomanip>
using namespace std;

class KBHit;

class KBHitList : public TObject
{
  public:
    KBHitList();
    virtual ~KBHitList() {}

    virtual void Clear(Option_t *option = "");
    virtual void Print(Option_t *option = "") const;

    void AddHit(KBHit *hit);
    void RemoveHit(KBHit *hit);

    vector<KBHit*> *GetHitArray();
    vector<Int_t> *GetHitIDArray();

    Int_t GetNumHits() const;
    KBHit *GetHit(Int_t idx) const;

  private:
    vector<KBHit*> fHitArray; //!
    vector<Int_t> fHitIDArray;

  ClassDef(KBHitList, 1)
};

#endif
