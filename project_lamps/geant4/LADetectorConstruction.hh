#ifndef LADETECTORCONSTRUCTION_HH
#define LADETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class LADetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    LADetectorConstruction();
    virtual ~LADetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
