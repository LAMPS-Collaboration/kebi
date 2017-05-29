#ifndef KBG4RUNMESSENGER_HH
#define KBG4RUNMESSENGER_HH

#include "KBG4RunManager.hh"
#include "G4UImessenger.hh"
#include "G4UIcommand.hh"
#include "G4UIcmdWithAString.hh"
#include "globals.hh"

class KBG4RunManager;

class KBG4RunMessenger : public G4UImessenger
{
  public:
    KBG4RunMessenger(KBG4RunManager *manager);
    virtual ~KBG4RunMessenger();

    void SetNewValue(G4UIcommand *command, G4String value);

  private:
    KBG4RunManager *fRunManager;

    G4UIcmdWithAString *fSetGeneratorFile;
    G4UIcmdWithAString *fSetOutputFile;
    G4UIcmdWithAString *fAddParFile;
};

#endif
