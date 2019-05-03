#ifndef KBTRACKINGACTION_HH
#define KBTRACKINGACTION_HH

#include "KBParameterContainer.hh"
#include "G4UserTrackingAction.hh"
#include "G4Track.hh"
#include "globals.hh"

class KBTrackingAction : public G4UserTrackingAction
{
  public:
    KBTrackingAction();
    virtual ~KBTrackingAction() {}

    virtual void PreUserTrackingAction(const G4Track* track);

  private:
    KBParameterContainer *fProcessTable;
};

#endif
