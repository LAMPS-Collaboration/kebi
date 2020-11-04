#include "KBParameterContainer.hh"
#include "TB20ADetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4UserLimits.hh"
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4UniformMagField.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4SubtractionSolid.hh"

TB20ADetectorConstruction::TB20ADetectorConstruction()
: G4VUserDetectorConstruction()
{
}

TB20ADetectorConstruction::~TB20ADetectorConstruction()
{
}

G4VPhysicalVolume *TB20ADetectorConstruction::Construct()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();

  auto par = runManager -> GetParameterContainer();

	G4double tpcInnerRadius = par -> GetParDouble("TPCrMin");
	G4double tpcOuterRadius = par -> GetParDouble("TPCrMax");
	G4double tpcLength = par -> GetParDouble("TPCLength");
	G4double tpcZOffset = par -> GetParDouble("TPCzOffset");

  G4NistManager *nist = G4NistManager::Instance();
  G4double STPTemperature = 273.15;
  G4double labTemperature = STPTemperature + 20.*kelvin;
  
	G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
	G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);
	G4Element *elementO = new G4Element("elementO", "O", 8., 16.000*g/mole);
	//G4Element *elementCu = new G4Element("elementCu","Cu", 29., 63.546*g/mole);
	//G4Element *elementAl = new G4Element("elementAl","Al", 13., 26.982*g/mole);

  G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);

	G4Material *matAcrylic = new G4Material("Acrylic", 1.19*g/cm3, 3);
	matAcrylic->AddElement(elementC,5);
	matAcrylic->AddElement(elementH,8);
	matAcrylic->AddElement(elementO,2);

	//G4Material *matCH2 = new G4Material("CH2", 0.91*g/cm3, 2);
	//matCH2->AddElement(elementH,2);
	//matCH2->AddElement(elementC,1);

	G4Material *matB = nist->FindOrBuildMaterial("G4_B");
	G4Material *matAl = nist->FindOrBuildMaterial("G4_Al");
	G4Material *matCu = nist->FindOrBuildMaterial("G4_Cu");
	G4Material *matFe = nist->FindOrBuildMaterial("G4_Fe");
	G4Material *matSC = nist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");
	G4Material *matCH2 = nist->FindOrBuildMaterial("G4_POLYETHYLENE");
	G4Material *matAir = nist->FindOrBuildMaterial("G4_AIR");
	G4Material *matVac = nist->FindOrBuildMaterial("G4_Galactic");
 
  G4double densityMethane = 0.717e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matMethaneGas = new G4Material("matMethaneGas ", densityMethane, 2, kStateGas, labTemperature);
  matMethaneGas -> AddElement(elementH, 4);
  matMethaneGas -> AddElement(elementC, 1);

  TString gasPar = "p10";
  if (par -> CheckPar("TPCgasPar")) {
    gasPar = par -> GetParString("TPCgasPar");
    gasPar.ToLower();
         if (gasPar.Index("p10")>=0) gasPar = "p10";
    else if (gasPar.Index("p20")>=0) gasPar = "p20";
    else gasPar = "p10";
  }

  G4Material *matGas = nullptr;
  if (gasPar == "p10") {
    G4double densityGas = .9*densityArGas + .1*densityMethane;
    matGas = new G4Material("matP10", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.9*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.1*densityMethane/densityGas);
  }
  else if (gasPar == "p20") {
    G4double densityGas = .8*densityArGas + .2*densityMethane;
    matGas = new G4Material("matP20", densityGas, 2, kStateGas, labTemperature);
    matGas -> AddMaterial(matArGas, 0.8*densityArGas/densityGas);
    matGas -> AddMaterial(matMethaneGas, 0.2*densityMethane/densityGas);
  }

  G4double worlddX = par -> GetParDouble("worlddX");
  G4double worlddY = par -> GetParDouble("worlddY");
  G4double worlddZ = par -> GetParDouble("worlddZ");

	//world
  G4Box *solidWorld = new G4Box("World", worlddX, worlddY, worlddZ);
  G4LogicalVolume *logicWorld;
	if ( par -> GetParInt("worldOpt")==0 ){
		logicWorld = new G4LogicalVolume(solidWorld, matVac, "World");
	}else if ( par -> GetParInt("worldOpt")==1 ){
		logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
	}else{
		logicWorld = new G4LogicalVolume(solidWorld, matVac, "World");
	}
  G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, -1, true);


	//1st Collimator
	if ( par -> GetParBool("CollimatorIn") )
	{
		G4double Collimatorx = par -> GetParDouble("Collimatorx");
		G4double Collimatory = par -> GetParDouble("Collimatory");
		G4double Collimatorz = par -> GetParDouble("Collimatorz");
		G4double CollimatorzOffset = par -> GetParDouble("CollimatorzOffset");

		G4double Holex = par -> GetParDouble("Holex");
		G4double Holey = par -> GetParDouble("Holey");

		G4Box *solidCollimator1 = new G4Box("Collimator1", Collimatorx/2.0, Collimatory/2.0, Collimatorz/2.0);
		G4Box *solidHole1 = new G4Box("Hole1", Holex/2.0, Collimatory/2.0, Collimatorz/2.0);
		G4SubtractionSolid *solidSubC1 = new G4SubtractionSolid("SubC1", solidCollimator1, solidHole1, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubC1;
		if ( par -> GetParInt("CollimatorOpt")==0 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matAl, "SubC1");
		}else if ( par -> GetParInt("CollimatorOpt")==1 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matCu, "SubC1");
		}else if ( par -> GetParInt("CollimatorOpt")==2 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matFe, "SubC1");
		}else if ( par -> GetParInt("CollimatorOpt")==3 ){
			logicSubC1 = new G4LogicalVolume(solidSubC1, matAcrylic, "SubC1");
		}else{
			logicSubC1 = new G4LogicalVolume(solidSubC1, matAl, "SubC1");
		}

		{
			G4VisAttributes * attSubC1 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubC1 -> SetVisAttributes(attSubC1);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,CollimatorzOffset+Collimatorz/2), logicSubC1, "SubC1", logicWorld, false, 0, true);

		G4Box *solidCollimator2 = new G4Box("Collimator2", Collimatorx/2.0, Collimatory/2.0, Collimatorz/2.0);
		G4Box *solidHole2 = new G4Box("Hole2", Collimatorx/2.0, Holey/2.0, Collimatorz/2.0);
		G4SubtractionSolid *solidSubC2 = new G4SubtractionSolid("SubC2", solidCollimator2, solidHole2, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubC2;
		if ( par -> GetParInt("CollimatorOpt")==0 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matAl, "SubC2");
		}else if ( par -> GetParInt("CollimatorOpt")==1 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matCu, "SubC2");
		}else if ( par -> GetParInt("CollimatorOpt")==2 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matFe, "SubC2");
		}else if ( par -> GetParInt("CollimatorOpt")==3 ){
			logicSubC2 = new G4LogicalVolume(solidSubC2, matAcrylic, "SubC2");
		}else{
			logicSubC2 = new G4LogicalVolume(solidSubC2, matAl, "SubC2");
		}

		{
			G4VisAttributes * attSubC2 = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubC2 -> SetVisAttributes(attSubC2);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,CollimatorzOffset+3*Collimatorz/2), logicSubC2, "SubC2", logicWorld, false, 0, true);
	}

	//1st Shield
	if ( par -> GetParBool("ShieldIn") )
	{
		G4double Shieldx = par -> GetParDouble("Collimatorx");
		G4double Shieldy = par -> GetParDouble("Collimatory");
		G4double Shieldz = par -> GetParDouble("Shieldz");
		G4double ShieldzOffset = par -> GetParDouble("ShieldzOffset");

		G4double Holedia = par -> GetParDouble("Holedia");

		G4Box *solidShield = new G4Box("Shield", Shieldx/2.0, Shieldy/2.0, Shieldz/2.0);
		G4Tubs *solidHole = new G4Tubs("Hole", 0, Holedia/2, Shieldz/2.0, 0, 2*M_PI);
		G4SubtractionSolid *solidSubS = new G4SubtractionSolid("SubC", solidShield, solidHole, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubS = new G4LogicalVolume(solidSubS, matB, "SubS");
		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Brown()));
			logicSubS -> SetVisAttributes(attSubS);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,ShieldzOffset+Shieldz/2), logicSubS, "SubS", logicWorld, false, 0, true);
	}

	//1st Block 
	if ( par -> GetParBool("Block1In") )
	{
		//G4double Block1x = par -> GetParDouble("Collimatorx");
		//G4double Block1y = par -> GetParDouble("Collimatory");
		G4double Block1x = 800;
		G4double Block1y = 400;
		G4double Block1z = par -> GetParDouble("Block1z");
		G4double Block1zOffset = par -> GetParDouble("Block1zOffset");

		G4Box *solidBlock1 = new G4Box("Block1", Block1x/2.0, Block1y/2.0, Block1z/2.0);
		G4Box *solidHole = new G4Box("Hole", 50.0/2.0, 50.0/2.0, Block1z/2.0);
		G4SubtractionSolid *solidSubS = new G4SubtractionSolid("SubC", solidBlock1, solidHole, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicSubS = new G4LogicalVolume(solidSubS, matAcrylic, "SubS");
		{
			G4VisAttributes * attSubS = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			logicSubS -> SetVisAttributes(attSubS);
		}
		new G4PVPlacement(0, G4ThreeVector(0,0,Block1zOffset+Block1z/2), logicSubS, "Block1", logicWorld, false, 0, true);
	}

	//2nd Block 
	if ( par -> GetParBool("Block2In") )
	{
		//G4double Block2x = par -> GetParDouble("Collimatorx");
		//G4double Block2y = par -> GetParDouble("Collimatory");
		G4double Block2x = 100;
		G4double Block2y = 200;
		G4double Block2z = 400;
		G4double Block2zOffset = par -> GetParDouble("Block2zOffset");

		G4Box *solidBlock2 = new G4Box("Block2", Block2x/2.0, Block2y/2.0, Block2z/2.0);
		G4LogicalVolume *logicBlock2 = new G4LogicalVolume(solidBlock2, matAcrylic, "Block2");
		{
			G4VisAttributes * attBlock2 = new G4VisAttributes(G4Colour(G4Colour::Brown()));
			logicBlock2 -> SetVisAttributes(attBlock2);
		}
		new G4PVPlacement(0, G4ThreeVector(+450,0,Block2zOffset+Block2z/2), logicBlock2, "Block2", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(-450,0,Block2zOffset+Block2z/2), logicBlock2, "Block2", logicWorld, false, 0, true);
	}

	//3rd Block 
	if ( par -> GetParBool("Block3In") )
	{
		//G4double Block3x = par -> GetParDouble("Collimatorx");
		//G4double Block3y = par -> GetParDouble("Collimatory");
		G4double Block3x = 100;
		G4double Block3y = 200;
		G4double Block3z = 400;
		G4double Block3zOffset = par -> GetParDouble("Block3zOffset");

		G4Box *solidBlock3 = new G4Box("Block3", Block3x/2.0, Block3y/2.0, Block3z/2.0);
		G4LogicalVolume *logicBlock3 = new G4LogicalVolume(solidBlock3, matAcrylic, "Block3");
		{
			G4VisAttributes * attBlock3 = new G4VisAttributes(G4Colour(G4Colour::Brown()));
			logicBlock3 -> SetVisAttributes(attBlock3);
		}
		new G4PVPlacement(0, G4ThreeVector(+450,0,Block3zOffset+Block3z/2), logicBlock3, "Block3", logicWorld, false, 0, true);
		new G4PVPlacement(0, G4ThreeVector(-450,0,Block3zOffset+Block3z/2), logicBlock3, "Block3", logicWorld, false, 0, true);
	}

	//Start counter
	if ( par -> GetParBool("StartCounterIn") )
	{
		G4double SCx = par -> GetParDouble("SCx");
		G4double SCy = par -> GetParDouble("SCy");
		G4double SCz = par -> GetParDouble("SCz");
		G4double SCzOffset = par -> GetParDouble("SCzOffset");

		G4Box *solidSC = new G4Box("SC", SCx/2.0, SCy/2.0, SCz/2.0);
		G4LogicalVolume *logicSC = new G4LogicalVolume(solidSC, matSC, "SC");
		{
			G4VisAttributes * attSC = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attSC -> SetForceWireframe(true);
			logicSC -> SetVisAttributes(attSC);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,SCzOffset+SCz/2), logicSC, "SC", logicWorld, false, 1, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//Veto counter
	if ( par -> GetParBool("VetoIn") )
	{

		G4double Vetox = par -> GetParDouble("Vetox");
		G4double Vetoy = par -> GetParDouble("Vetoy");
		G4double Vetoz = par -> GetParDouble("Vetoz");
		G4double VetozOffset = par -> GetParDouble("VetozOffset");

		G4Box *solidVeto1 = new G4Box("Veto1", Vetox/2.0, Vetoy/2.0, Vetoz/2.0);
		G4Box *solidHole1 = new G4Box("VetoHole1", Vetox/2.0, 10/2.0, Vetoz/2.0);
		G4SubtractionSolid *solidSubVeto1 = new G4SubtractionSolid("SubVeto1", solidVeto1, solidHole1, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicVeto1 = new G4LogicalVolume(solidSubVeto1, matSC, "Veto1");
		{
			G4VisAttributes * attVeto = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			logicVeto1 -> SetVisAttributes(attVeto);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,VetozOffset+Vetoz/2), logicVeto1, "Veto1", logicWorld, false, 2, true);
		runManager -> SetSensitiveDetector(pvp);

		G4Box *solidVeto2 = new G4Box("Veto2", Vetox/2.0, Vetoy/2.0, Vetoz/2.0);
		G4Box *solidHole2 = new G4Box("VetoHole2", 10/2.0, Vetoy/2.0, Vetoz/2.0);
		G4SubtractionSolid *solidSubVeto2 = new G4SubtractionSolid("SubVeto2", solidVeto2, solidHole2, 0, G4ThreeVector(0,0,0));

		G4LogicalVolume *logicVeto2 = new G4LogicalVolume(solidSubVeto2, matSC, "Veto2");
		{
			G4VisAttributes * attVeto = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			logicVeto2 -> SetVisAttributes(attVeto);
		}
		pvp = new G4PVPlacement(0, G4ThreeVector(0,0,VetozOffset+3*Vetoz/2), logicVeto2, "Veto2", logicWorld, false, 3, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//BDC1
	if ( par -> GetParBool("BDC1In") )
	{
		G4double BDC1x = par -> GetParDouble("BDC1x");
		G4double BDC1y = par -> GetParDouble("BDC1y");
		G4double BDC1z = par -> GetParDouble("BDC1z");
		G4double BDC1zOffset = par -> GetParDouble("BDC1zOffset");

		G4Box *solidBDC1 = new G4Box("BDC1", BDC1x/2.0, BDC1y/2.0, BDC1z/2.0);
		G4LogicalVolume *logicBDC1 = new G4LogicalVolume(solidBDC1, matGas, "BDC1");
		{
			G4VisAttributes * attBDC1 = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attBDC1 -> SetForceWireframe(true);
			logicBDC1 -> SetVisAttributes(attBDC1);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,BDC1zOffset+BDC1z/2), logicBDC1, "BDC1", logicWorld, false, 4, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//BDC2
	if ( par -> GetParBool("BDC2In") )
	{
		G4double BDC2x = par -> GetParDouble("BDC2x");
		G4double BDC2y = par -> GetParDouble("BDC2y");
		G4double BDC2z = par -> GetParDouble("BDC2z");
		G4double BDC2zOffset = par -> GetParDouble("BDC2zOffset");

		G4Box *solidBDC2 = new G4Box("BDC2", BDC2x/2.0, BDC2y/2.0, BDC2z/2.0);
		G4LogicalVolume *logicBDC2 = new G4LogicalVolume(solidBDC2, matGas, "BDC2");
		{
			G4VisAttributes * attBDC2 = new G4VisAttributes(G4Colour(G4Colour::Yellow()));
			attBDC2 -> SetForceWireframe(true);
			logicBDC2 -> SetVisAttributes(attBDC2);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,BDC2zOffset+BDC2z/2), logicBDC2, "BDC2", logicWorld, false, 5, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//Target1
	if ( par -> GetParBool("Target1In") )
	{
		G4double Target1x = par -> GetParDouble("Target1x");
		G4double Target1y = par -> GetParDouble("Target1y");
		G4double Target1z = par -> GetParDouble("Target1z");
		G4double Target1zOffset = par -> GetParDouble("Target1zOffset");

		G4Box *solidTarget1 = new G4Box("Target1", Target1x/2.0, Target1y/2.0, Target1z/2.0);
		G4LogicalVolume *logicTarget1 = new G4LogicalVolume(solidTarget1, matCH2, "Traget1");
		{
			G4VisAttributes * attTarget1 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget1 -> SetVisAttributes(attTarget1);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target1zOffset+Target1z/2), logicTarget1, "Target1", logicWorld, false, 10, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//Target2
	if ( par -> GetParBool("Target2In") )
	{
		G4double Target2x = par -> GetParDouble("Target2x");
		G4double Target2y = par -> GetParDouble("Target2y");
		G4double Target2z = par -> GetParDouble("Target2z");
		G4double Target2zOffset = par -> GetParDouble("Target2zOffset");

		G4Box *solidTarget2 = new G4Box("Target2", Target2x/2.0, Target2y/2.0, Target2z/2.0);
		G4LogicalVolume *logicTarget2 = new G4LogicalVolume(solidTarget2, matCH2, "Traget2");
		{
			G4VisAttributes * attTarget2 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget2 -> SetVisAttributes(attTarget2);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target2zOffset+Target2z/2), logicTarget2, "Target2", logicWorld, false, 11, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//Target3
	if ( par -> GetParBool("Target3In") )
	{
		G4double Target3x = par -> GetParDouble("Target3x");
		G4double Target3y = par -> GetParDouble("Target3y");
		G4double Target3z = par -> GetParDouble("Target3z");
		G4double Target3zOffset = par -> GetParDouble("Target3zOffset");

		G4Box *solidTarget3 = new G4Box("Target3", Target3x/2.0, Target3y/2.0, Target3z/2.0);
		G4LogicalVolume *logicTarget3 = new G4LogicalVolume(solidTarget3, matCH2, "Traget3");
		{
			G4VisAttributes * attTarget3 = new G4VisAttributes(G4Colour(G4Colour::Green()));
			logicTarget3 -> SetVisAttributes(attTarget3);
		}
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,Target3zOffset+Target3z/2), logicTarget3, "Target3", logicWorld, false, 12, true);
		runManager -> SetSensitiveDetector(pvp);
	}


	//TPC
	if ( par -> GetParBool("TPCIn") )
	{
		G4Tubs *solidTPC = new G4Tubs("TPC", tpcInnerRadius, tpcOuterRadius, .5*tpcLength, 0., 360*deg);
		G4LogicalVolume *logicTPC = new G4LogicalVolume(solidTPC, matGas, "TPC");
		{
			G4VisAttributes * attTPC = new G4VisAttributes(G4Colour(G4Colour::Gray()));
			attTPC -> SetForceWireframe(true);
			logicTPC -> SetVisAttributes(attTPC);
		}
		logicTPC -> SetUserLimits(new G4UserLimits(1.*mm));
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset+tpcLength/2), logicTPC, "TPC", logicWorld, false, 6, true);
		runManager -> SetSensitiveDetector(pvp);
	}

	//BTOF
	if ( par -> GetParBool("BTOFIn") )
	{

		G4double BTOFzOffset = tpcZOffset+tpcLength/2;

		G4Box *solidBTOF = new G4Box("BTOF", 10/2.0, 180/2.0, 1500/2.0);
		G4LogicalVolume *logicBTOF = new G4LogicalVolume(solidBTOF, matSC, "BTOF");
		{
			G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOF -> SetForceWireframe(true);
			logicBTOF -> SetVisAttributes(attBTOF);
		}
		auto pvp1 = new G4PVPlacement(0, G4ThreeVector(+450,0,BTOFzOffset), logicBTOF, "BTOF", logicWorld, false, 7, true);
		auto pvp2 = new G4PVPlacement(0, G4ThreeVector(-450,0,BTOFzOffset), logicBTOF, "BTOF", logicWorld, false, 7, true);
		runManager -> SetSensitiveDetector(pvp1);
		runManager -> SetSensitiveDetector(pvp2);
	}

	/*
  bool checkWall = par -> CheckPar("numNeutronWall");
  if (checkWall)
  {
    G4Material* scint_mat = nist -> FindOrBuildMaterial("G4_XYLENE");

    G4int numWall = par -> GetParInt("numNeutronWall");
    for (auto iwall = 0; iwall < numWall; ++iwall) {
      auto naStackAxis = par -> GetParAxis(Form("naStackAxis%d",iwall));
      auto naNumStack = par -> GetParInt(Form("naNumStack%d",iwall));
      auto nadX = par -> GetParDouble(Form("nadX%d",iwall));
      auto nadY = par -> GetParDouble(Form("nadY%d",iwall));
      auto nadZ = par -> GetParDouble(Form("nadZ%d",iwall));
      auto naXOffset = par -> GetParDouble(Form("naXOffset%d",iwall));
      auto naYOffset = par -> GetParDouble(Form("naYOffset%d",iwall));
      auto naZOffset = par -> GetParDouble(Form("naZOffset%d",iwall));

      G4Box* solidScint = new G4Box(Form("Scintillator_%d",iwall), 0.5*nadX, 0.5*nadY, 0.5*nadZ);
      G4LogicalVolume* logicScint = new G4LogicalVolume(solidScint, scint_mat, Form("Scintillator_%d",iwall));

      KBGeoBoxStack boxStack(naXOffset,naYOffset,naZOffset,nadX,nadY,nadZ,naNumStack,naStackAxis,KBVector3::kZ);

      for (auto copy = 0; copy < naNumStack; ++copy) {
        Int_t id = 10000+copy+iwall*100;
        G4String name = Form("Scintillator_%d_%d",iwall,copy);
        auto box = boxStack.GetBox(copy);
        auto pos = box.GetCenter();
        G4ThreeVector gpos(pos.X(),pos.Y(),pos.Z());
        auto cpvp = new G4PVPlacement(0, gpos, logicScint, name, logicWorld, false, id, true);
        runManager -> SetSensitiveDetector(cpvp);
      }
    }
  }
	*/

  //G4double bfieldx = par -> GetParDouble("bfieldx");
  //G4double bfieldy = par -> GetParDouble("bfieldy");
  //G4double bfieldz = par -> GetParDouble("bfieldz");
  //new G4GlobalMagFieldMessenger(G4ThreeVector(bfieldx*tesla, bfieldy*tesla, bfieldz*tesla));

  return physWorld;
}
