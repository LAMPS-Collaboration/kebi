#ifndef LADETECTORCONSTRUCTION_HH
#define LADETECTORCONSTRUCTION_HH

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

#include "KBParameterContainerHolder.hh"
#include "KBParameterContainer.hh"

class G4VPhysicalVolume;
class G4LogicalVolume;

class LADetectorConstruction : public G4VUserDetectorConstruction, public KBParameterContainerHolder
{
  public:
    LADetectorConstruction();
    virtual ~LADetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
};

#endif
