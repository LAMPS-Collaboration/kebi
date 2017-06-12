#include "globals.hh"
#include "KBCompiled.h"

#include "KBG4RunManager.hh"
#include "G4VisExecutive.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4StepLimiterPhysics.hh"

#include "KBParameterContainer.hh"

#include "QGSP_BERT.hh"
#include "KBMCDataManager.hh"
#include "LADetectorConstruction.hh"
#include "KBPrimaryGeneratorAction.hh"

#include "KBEventAction.hh"
#include "KBTrackingAction.hh"
#include "KBSteppingAction.hh"

int main(int argc, char** argv)
{
  auto runManager = new KBG4RunManager();

  G4VModularPhysicsList* physicsList = new QGSP_BERT;
  physicsList -> RegisterPhysics(new G4StepLimiterPhysics());
  runManager -> SetUserInitialization(physicsList);

  auto dc = new LADetectorConstruction();
  dc -> SetParameterContainer(runManager->GetParameterContainer());
  runManager -> SetUserInitialization(dc);
  runManager -> SetUserAction(new KBPrimaryGeneratorAction());

  runManager -> SetUserAction(new KBEventAction());
  runManager -> SetUserAction(new KBTrackingAction());
  runManager -> SetUserAction(new KBSteppingAction());

  G4UImanager* uiManager = G4UImanager::GetUIpointer();

  if (argc != 1) {
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    uiManager -> ApplyCommand(command+fileName);
  }
  /*
  else 
  {
    runManager -> Initialize();
    G4VisManager* visManager = new G4VisExecutive;
    visManager -> Initialize();

    G4UIExecutive* uiExecutive = new G4UIExecutive(argc, argv);
    uiManager -> ApplyCommand("/control/execute vis.mac"); 
    uiExecutive -> SessionStart();

    delete uiExecutive;
    delete visManager;
  }
  */

  delete runManager;

  return 0;
}
