#ifndef KBG4RUNMANAGER_HH
#define KBG4RUNMANAGER_HH

#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "KBMCDataManager.hh"
#include "KBG4RunMessenger.hh"
#include "KBGear.hh"
#include "globals.hh"

class KBG4RunMessenger;

class KBG4RunManager : public G4RunManager, public KBGear
{
  public:
    KBG4RunManager();
    KBG4RunManager(const char *name);
    virtual ~KBG4RunManager();

    virtual void Initialize();

    void SetGeneratorFile(G4String value);
    void SetOutputFile(G4String value);
    void AddParFile(G4String value);

    void SetSensitiveDetector(G4VPhysicalVolume *physicalVolume);

  private:
    KBG4RunMessenger *fMessenger;

    vector<G4int> fCopyNoArray;
};

#endif
