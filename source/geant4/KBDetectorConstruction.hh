#ifndef KBDETECTORCONSTRUCTION_HH 
#define KBDETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class KBDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    KBDetectorConstruction();
    virtual ~KBDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
