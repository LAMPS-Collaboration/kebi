#include "KBG4RunMessenger.hh"

KBG4RunMessenger::KBG4RunMessenger(KBG4RunManager *manager)
{
  fRunManager = manager;

  fSetGeneratorFile = new G4UIcmdWithAString("/run/SetGeneratorFile", this);
  fSetOutputFile = new G4UIcmdWithAString("/run/SetOutputFile", this);
  fAddParFile = new G4UIcmdWithAString("/run/AddParFile", this);
}

KBG4RunMessenger::~KBG4RunMessenger()
{
  delete fSetGeneratorFile;
}

void KBG4RunMessenger::SetNewValue(G4UIcommand *command, G4String value)
{
  if (command == fSetGeneratorFile)
    fRunManager -> SetGeneratorFile(value);
  else if (command == fSetOutputFile)
    fRunManager -> SetOutputFile(value);
  else if (command == fAddParFile)
    fRunManager -> AddParFile(value);
}
