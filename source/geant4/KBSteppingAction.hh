#ifndef KBSTEPPINGACTION_HH
#define KBSTEPPINGACTION_HH

#include "G4UserSteppingAction.hh"
#include "G4Step.hh"
#include "globals.hh"

class KBSteppingAction : public G4UserSteppingAction
{
  public:
    KBSteppingAction();
    virtual ~KBSteppingAction() {}

    virtual void UserSteppingAction(const G4Step*);
};

#endif
