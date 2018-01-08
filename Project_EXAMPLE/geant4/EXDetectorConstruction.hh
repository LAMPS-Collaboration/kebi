#ifndef EXDETECTORCONSTRUCTION_HH 
#define EXDETECTORCONSTRUCTION_HH 

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class EXDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    EXDetectorConstruction();
    virtual ~EXDetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
