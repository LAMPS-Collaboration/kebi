#include "KBParameterContainer.hh"
#include "LHDetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Trap.hh"
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

#include "LHMagneticField.hh"
#include "LHMagneticFieldSetup.hh"

LHDetectorConstruction::LHDetectorConstruction()
: G4VUserDetectorConstruction()
{
}

LHDetectorConstruction::~LHDetectorConstruction()
{
}

G4VPhysicalVolume *LHDetectorConstruction::Construct()
{
  auto runManager = (KBG4RunManager *) G4RunManager::GetRunManager();

  auto par = runManager -> GetParameterContainer();

  G4NistManager *nist = G4NistManager::Instance();
  G4double STPTemperature = 273.15;
  G4double labTemperature = STPTemperature + 20.*kelvin;
  
  G4Element *elementH = new G4Element("elementH", "H", 1., 1.00794*g/mole);
  G4Element *elementC = new G4Element("elementC", "C", 6., 12.011*g/mole);

  G4double densityArGas = 1.782e-3*g/cm3*STPTemperature/labTemperature;
  G4Material *matArGas = new G4Material("ArgonGas", 18, 39.948*g/mole, densityArGas, kStateGas, labTemperature);
 
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

  G4Material *matAir = nist -> FindOrBuildMaterial("G4_AIR");
	G4Material *matVac = nist -> FindOrBuildMaterial("G4_Galactic");
	G4Material *matSC = nist -> FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");


	//World
  G4double worlddX = par -> GetParDouble("worlddX");
  G4double worlddY = par -> GetParDouble("worlddY");
  G4double worlddZ = par -> GetParDouble("worlddZ");

  G4Box *solidWorld = new G4Box("World", worlddX, worlddY, worlddZ);
	G4LogicalVolume *logicWorld;
	if ( par -> GetParInt("worldOpt")==0 ) {
		logicWorld = new G4LogicalVolume(solidWorld, matVac, "World");
	}else {
		logicWorld = new G4LogicalVolume(solidWorld, matAir, "World");
	}
  G4PVPlacement *physWorld = new G4PVPlacement(0, G4ThreeVector(), logicWorld, "World", 0, false, -1, true);


	//TPC
	G4double tpcInnerRadius = par -> GetParDouble("TPCrMin");
	G4double tpcOuterRadius = par -> GetParDouble("TPCrMax");
	G4double tpcLength = par -> GetParDouble("TPCLength");
  G4double tpcZOffset = par -> GetParDouble("zOffset");

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
		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset), logicTPC, "TPC", logicWorld, false, 10, true);
		runManager -> SetSensitiveDetector(pvp);
	}


	//B-TOF
	G4double btofX = par -> GetParDouble("BTOFlength");
	G4double btofY = par -> GetParDouble("BTOFx");
	G4double btofZ = par -> GetParDouble("BTOFy");
	G4int btofNum = par -> GetParInt("BTOFnum");
	G4int btofOpt = par -> GetParInt("BTOFopt");

	if ( par -> GetParBool("BTOFIn") )
	{
		G4double dphi = 2*M_PI/btofNum, half_dphi = 0.5*dphi;
		G4double cosdphi = cos(half_dphi);
		G4double tandphi = tan(half_dphi);

		G4double radiusIn = 0.5*btofY/tandphi;
		G4double radiusOut = (radiusIn + btofZ)/cosdphi;

		G4cout << "Radius In: " << radiusIn << ", RadiusOut: " << radiusOut << G4endl;

		G4double dz = btofX;

		G4Tubs *solidBTOF = new G4Tubs("BTOF", radiusIn, radiusOut, 0.5*dz, 0., 360*deg);
		G4LogicalVolume *logicBTOF = new G4LogicalVolume(solidBTOF, matVac, "BTOF");
		logicBTOF -> SetVisAttributes (G4VisAttributes::GetInvisible());
		/*
		{
			G4VisAttributes * attBTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOF -> SetForceWireframe(true);
			logicBTOF -> SetVisAttributes(attBTOF);
		}
		*/

		G4Box *solidBTOFScint = new G4Box("BTOFScintillator", 0.5*btofX, 0.5*btofY, 0.5*btofZ);
		G4LogicalVolume *logicBTOFScint = new G4LogicalVolume(solidBTOFScint, matSC, "BTOFScintillator"); 
		{
			G4VisAttributes * attBTOFScint = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attBTOFScint -> SetForceWireframe(true);
			logicBTOFScint -> SetVisAttributes(attBTOFScint);
		}

		for (int ii=0; ii<btofNum; ii++){
			if ( btofOpt>0 && abs(ii-36)==btofOpt ) continue;

			G4double phi = ii*2*M_PI/btofNum;
			G4RotationMatrix rotm  = G4RotationMatrix();
			rotm.rotateY(90*deg);
			rotm.rotateZ(phi);

			G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);
			G4ThreeVector position = (radiusIn + 0.5*btofZ)*uz;
			G4Transform3D transform = G4Transform3D(rotm, position);

			new G4PVPlacement(
					transform, //rotation,position
					logicBTOFScint,        //its logical volume
					Form("BTOFScintillator_%02d",ii), //its name
					logicBTOF, //its mother  volume
					false, //no boolean operation
					2000+ii, //copy number
					false); // checking overlaps
		}

		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset), logicBTOF, "BTOF", logicWorld, false, 20, true);
		runManager -> SetSensitiveDetector(pvp);
	}


	//F-TOF
	G4double ftofX1 = par -> GetParDouble("FTOFx1");
	G4double ftofX2 = par -> GetParDouble("FTOFx2");
	G4double ftofLength = par -> GetParDouble("FTOFlength");
	G4double ftofThickness = par -> GetParDouble("FTOFthickness");
	G4double ftofzOffset = par -> GetParDouble("FTOFzOffset");

	if ( par -> GetParBool("FTOFIn") )
	{

		G4double dphi = 2*M_PI/btofNum, half_dphi = 0.5*dphi;
		G4double cosdphi = cos(half_dphi);
		G4double tandphi = tan(half_dphi);

		G4double radiusIn = 0.5*btofY/tandphi;
		G4double radiusOut = (radiusIn + btofZ)/cosdphi;

		G4Tubs *solidFTOF = new G4Tubs("FTOF", 0, radiusOut, 0.5*5, 0., 360*deg);
		G4LogicalVolume *logicFTOF = new G4LogicalVolume(solidFTOF, matVac, "FTOF");
		logicFTOF -> SetVisAttributes (G4VisAttributes::GetInvisible());
		/*
		{
			G4VisAttributes * attFTOF = new G4VisAttributes(G4Colour(G4Colour::Red()));
			attFTOF -> SetForceWireframe(true);
			logicFTOF -> SetVisAttributes(attFTOF);
		}
		*/


		G4Trap *solidFTOFScint = new G4Trap("FTOFScintillator", 0.5*ftofX1, 0.5*ftofX2, 0.5*ftofThickness, 0.5*ftofThickness, 0.5*ftofLength);
		G4LogicalVolume *logicFTOFScint = new G4LogicalVolume(solidFTOFScint, matSC, "FTOFScintillator"); 
		{
			G4VisAttributes * attFTOFScint = new G4VisAttributes(G4Colour(G4Colour::Blue()));
			attFTOFScint -> SetForceWireframe(true);
			logicFTOFScint -> SetVisAttributes(attFTOFScint);
		}

		for (int ii=0; ii<btofNum; ii++){

			G4double phi = ii*2*M_PI/btofNum;
			G4RotationMatrix rotm  = G4RotationMatrix();
			rotm.rotateX(90*deg);
			rotm.rotateZ(-90*deg + phi);

			G4ThreeVector uz = G4ThreeVector(std::cos(phi),  std::sin(phi),0.);
			G4ThreeVector position = (radiusOut - 0.5*ftofLength)*uz;
			G4Transform3D transform = G4Transform3D(rotm, position);

			new G4PVPlacement(
					transform, //rotation,position
					logicFTOFScint,        //its logical volume
					Form("FTOFScintillator_%02d",ii), //its name
					logicFTOF, //its mother  volume
					false, //no boolean operation
					3000+ii, //copy number
					false); // checking overlaps

		}

		auto pvp = new G4PVPlacement(0, G4ThreeVector(0,0,tpcZOffset+ftofzOffset), logicFTOF, "FTOF", logicWorld, false, 30, true);
		runManager -> SetSensitiveDetector(pvp);
	}


	//Neutron
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

	if ( par -> GetParBool("bfieldmap") )
	{

		TString name = par -> GetParString("bfieldmapfile");
		G4cout << "Use bfield map, name: " << name << G4endl;

		LHMagneticField* bField = new LHMagneticField();
		//bField->SetVerbosity(3);
		//bField->SetFieldBoundX(-worldXYZ.getX(), worldXYZ.getX());
		//bField->SetFieldBoundY(-worldXYZ.getY(), worldXYZ.getY());
		//bField->SetFieldBoundZ(-worldXYZ.getZ(), worldXYZ.getZ());
		//bField->SetFieldOffset(1, 1, 1);
		//bField->SetUniformField(0, 0, 1.);
		//bField->SetUnitDistance(cm);
		//bField->SetUnitField(kilogauss);
		bField->MakeFieldMap(name.Data());

		LHMagneticFieldSetup* bFieldSetup = new LHMagneticFieldSetup();
		//bFieldSetup->SetStepperType(2);
		//bFieldSetup->SetStepMin(1 * mm);
		//bFieldSetup->SetDeltaChord(0.1 * mm);
		//bFieldSetup->SetDeltaIntersection(1.e-4);
		//bFieldSetup->SetDeltaOneStep(1.e-3);
		//bFieldSetup->SetEpsilonMax(1.e-4);
		//bFieldSetup->SetEpsilonMin(1.e-6);
		//bFieldSetup->SetFieldManager(G4TransportationManager::GetTransportationManager()->GetFieldManager());
		bFieldSetup->MakeSetup(bField);
		fFieldCache.Put(bFieldSetup);
		logicWorld->SetFieldManager(fFieldCache.Get()->GetFieldManager(), true);
	}
	else
	{
		G4double bfieldx = par -> GetParDouble("bfieldx");
		G4double bfieldy = par -> GetParDouble("bfieldy");
		G4double bfieldz = par -> GetParDouble("bfieldz");

		new G4GlobalMagFieldMessenger(G4ThreeVector(bfieldx*tesla, bfieldy*tesla, bfieldz*tesla));
	}

  return physWorld;
}
