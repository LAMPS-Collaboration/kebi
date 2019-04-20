#ifndef KBG4RUNMANAGER_HH
#define KBG4RUNMANAGER_HH

#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "KBMCDataManager.hh"
#include "KBG4RunMessenger.hh"
#include "KBGear.hh"
#include "TString.h"
#include "globals.hh"

class KBG4RunMessenger;

class KBG4RunManager : public G4RunManager, public KBGear
{
  public:
    KBG4RunManager();
    KBG4RunManager(const char *name);
    virtual ~KBG4RunManager();

    virtual void Initialize();
    void Run(G4int argc=0, char **argv=nullptr, const G4String &type="");

    void SetGeneratorFile(TString value);
    void SetOutputFile(TString value);

    void SetSensitiveDetector(G4VPhysicalVolume *physicalVolume);

  private:
    KBG4RunMessenger *fMessenger;

    vector<G4int> fCopyNoArray;
};

#endif
