#include "DUMMYDetectorConstruction.hh"
#include "KBG4RunManager.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"

DUMMYDetectorConstruction::DUMMYDetectorConstruction()
: G4VUserDetectorConstruction()
{
}

DUMMYDetectorConstruction::~DUMMYDetectorConstruction()
{
}

G4VPhysicalVolume* DUMMYDetectorConstruction::Construct()
{  
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();
  G4NistManager* nist = G4NistManager::Instance();

  // -----------------------------------------------------
  // World

  G4Material* world_mat = nist -> FindOrBuildMaterial("G4_AIR");
  G4double world_size = 100*mm;

  G4Box* solidWorld = new G4Box("World", 0.5*world_size, 0.5*world_size, 0.5*world_size);
  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
  G4VPhysicalVolume* physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, 0, true);

  // -----------------------------------------------------
  // Detector

  G4Material* detector_mat = nist -> FindOrBuildMaterial("G4_WATER");
  G4double detector_size = 20*mm;
  G4double detector_offset_z = 30*mm;

  G4Box* solidDetector = new G4Box("Detector", 0.5*detector_size, 0.5*detector_size, 0.5*detector_size);
  G4LogicalVolume* logicDetector = new G4LogicalVolume(solidDetector, detector_mat, "Detector");
  auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,detector_offset_z), logicDetector, "Detector", logicWorld, false, 0, true);
  runManager -> SetSensitiveDetector(pvp);

  return physWorld;
}
