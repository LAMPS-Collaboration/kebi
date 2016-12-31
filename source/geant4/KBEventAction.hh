#ifndef KBEVENTACTION_HH
#define KBEVENTACTION_HH

#include "G4UserEventAction.hh"
#include "G4Event.hh"
#include "globals.hh"

class KBEventAction : public G4UserEventAction
{
  public:
    KBEventAction() {}
    virtual ~KBEventAction() {}

    virtual void EndOfEventAction(const G4Event* event);
};

#endif
