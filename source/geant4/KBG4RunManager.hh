#ifndef KBG4RUNMANAGER_HH
#define KBG4RUNMANAGER_HH

#define g4_info KBLog("Geant4",__FUNCTION__,0,2)
#define g4_warning KBLog("Geant4",__FUNCTION__,0,3)

#include "G4RunManager.hh"
#include "G4VPhysicalVolume.hh"
#include "KBMCDataManager.hh"
#include "KBGear.hh"
#include "TString.h"
#include "globals.hh"

class KBG4RunManager : public G4RunManager, public KBGear
{
  public:
    KBG4RunManager();
    virtual ~KBG4RunManager();

    virtual void Initialize();
    void Run(G4int argc=0, char **argv=nullptr, const G4String &type="");

    void SetSensitiveDetector(G4VPhysicalVolume *physicalVolume);
    void SetVolume(G4VPhysicalVolume *physicalVolume);

    KBParameterContainer *GetVolumes();
    KBParameterContainer *GetSensitiveDetectors();
    KBParameterContainer *GetProcessTable();

  private:
    void SetGeneratorFile(TString value);
    void SetOutputFile(TString value);

    KBMCDataManager *fData;
    KBParameterContainer *fVolumes;
    KBParameterContainer *fSensitiveDetectors;
    KBParameterContainer *fProcessTable;
};

#endif
