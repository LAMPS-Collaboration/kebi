#include "TB22HDetectorConstruction.hh"

#include "KBG4RunManager.hh"
#include "KBGeoBoxStack.hh"
#include "KBParameterContainer.hh"

#include "G4AutoDelete.hh"
#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4RunManager.hh"
#include "G4SubtractionSolid.hh"
#include "G4SystemOfUnits.hh"
#include "G4TransportationManager.hh"
#include "G4Trap.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include <iostream>
#include <cmath>
using namespace std;

//Constructor
TB22HDetectorConstruction::TB22HDetectorConstruction() : G4VUserDetectorConstruction()
{
	fNist = G4NistManager::Instance();
}

//Destructor
TB22HDetectorConstruction::~TB22HDetectorConstruction()
{
	G4AutoDelete::Register(fNist);
}

//=======================================================
G4VPhysicalVolume* TB22HDetectorConstruction::Construct()
{
	auto fRun = (KBG4RunManager*)G4RunManager::GetRunManager();
	auto fPar = fRun->GetParameterContainer();

	auto fVisFrame = new G4VisAttributes();
	fVisFrame->SetColor(G4Color::Gray());
	fVisFrame->SetForceWireframe(true);

	G4ThreeVector fZero(0, 0, 0);
	G4double fSTPTemp = 273.15 * kelvin;
	G4double fLabTemp = fSTPTemp + 20 * kelvin;

	//World volume
	//--------------------------------------------------------------------

	string worldMatStr;
	if      (fPar->GetParInt("worldOpt") == 0) worldMatStr = "G4_Galactic";
	else if (fPar->GetParInt("worldOpt") == 1) worldMatStr = "G4_AIR";
	else { cout <<"World material? Stop.\n"; assert(false); }
	G4Material* worldMat = fNist->FindOrBuildMaterial(worldMatStr.c_str());

	G4int    worldID = fPar->GetParInt("worldID");
	G4double worldDX = fPar->GetParDouble("worlddX");
	G4double worldDY = fPar->GetParDouble("worlddY");
	G4double worldDZ = fPar->GetParDouble("worlddZ");

	G4Box*             WorldSol = new G4Box("WorldSolid", worldDX, worldDY, worldDZ);
	G4LogicalVolume*   WorldLog = new G4LogicalVolume(WorldSol, worldMat, "World");//"worldLogic");
	G4VPhysicalVolume* WorldPhy = new G4PVPlacement(0, fZero, WorldLog, "WorldPhys", 0, false, worldID, true);

    auto fVisWorld = new G4VisAttributes();
    fVisWorld->SetColor(G4Color::White());
    fVisWorld->SetForceWireframe(true);
	WorldLog->SetVisAttributes(fVisWorld);

	//SC
	//--------------------------------------------------------------------

	if (fPar->GetParBool("SCIn"))
	{
		G4Material* scMat = fNist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

		G4int    scID    = fPar->GetParInt("scID");
		G4double scDimX  = fPar->GetParDouble("scDimX");
		G4double scDimY  = fPar->GetParDouble("scDimY");
		G4double scDimZ  = fPar->GetParDouble("scDimZ");
		G4double scPosZ0 = fPar->GetParDouble("scPosZ0");
		G4double scPosZ1 = fPar->GetParDouble("scPosZ1");

		G4Box*           SCSol = new G4Box("SCSolid", scDimX/2, scDimY/2, scDimZ/2);
		G4LogicalVolume* SCLog = new G4LogicalVolume(SCSol, scMat, "SCLogic");

		G4ThreeVector SCPos0(0, 0, scPosZ0 + scDimZ/2.);
		G4ThreeVector SCPos1(0, 0, scPosZ1 + scDimZ/2.);
		G4VPhysicalVolume* SCPhys0 = new G4PVPlacement(0, SCPos0, SCLog, "SC0", WorldLog, false, scID+0, true);
		G4VPhysicalVolume* SCPhys1 = new G4PVPlacement(0, SCPos1, SCLog, "SC1", WorldLog, false, scID+1, true);
		fRun->SetSensitiveDetector(SCPhys0);
		fRun->SetSensitiveDetector(SCPhys1);

		auto fVisSC = new G4VisAttributes();
		fVisSC->SetColor(G4Color::Cyan());
		fVisSC->SetForceWireframe(true);
		SCLog->SetVisAttributes(fVisSC);
	}//SC

	//BDC
	//--------------------------------------------------------------------

	if (fPar->GetParBool("BDCIn"))
	{
		//Argon
		G4double ArGasD = 1.7836 * mg/cm3 * fSTPTemp/fLabTemp;
		G4Material* ArGas = new G4Material("ArGas", 18, 39.948*g/mole, ArGasD, kStateGas, fLabTemp);

		//CH4 (Methane)
		G4double CH4GasD = 0.717e-3 * g/cm3 * fSTPTemp/fLabTemp; //Methane density
		G4Material* CH4Gas = new G4Material("CH4Gas", CH4GasD, 2, kStateGas, fLabTemp);
		G4Element* elH = new G4Element("Hydrogen", "H", 1., 1.00794 * g/mole);
		G4Element* elC = new G4Element("Carbon", "C", 6., 12.011 * g/mole);
		CH4Gas->AddElement(elC, 1);
		CH4Gas->AddElement(elH, 4);

		//P10
		G4double P10GasD = 0.9*ArGasD + 0.1*CH4GasD;
		G4Material* P10Gas = new G4Material("P10Gas", P10GasD, 2, kStateGas, fLabTemp);
		P10Gas->AddMaterial(ArGas, 0.9 * ArGasD/P10GasD);
		P10Gas->AddMaterial(CH4Gas, 0.1 * CH4GasD/P10GasD);

		//Mylar and Al oxide
		G4Material* Mylar   = fNist->FindOrBuildMaterial("G4_MYLAR");
		G4Material* AlOxide = fNist->FindOrBuildMaterial("G4_ALUMINUM_OXIDE");

		//+++++++++++++++++++++++++++++++++++++++

		G4int    bdcID    = fPar->GetParInt("bdcID");
		G4double bdcDimX  = fPar->GetParDouble("bdcDimX");
		G4double bdcDimY  = fPar->GetParDouble("bdcDimY");
		G4double bdcDimZ  = fPar->GetParDouble("bdcDimZ");
		G4double bdcPosZ0 = fPar->GetParDouble("bdcPosZ0");
		G4double bdcPosZ1 = fPar->GetParDouble("bdcPosZ1");

		G4double bdcMylarT = 50 * um;
		G4double bdcAlmT = 2.5 * um; //Aluminized mylar, one layer
		const int nAlm = 5; //Total # of Alm layers

		//Enclosure volume (all)
		G4Box*           BDCSol = new G4Box("BDCSolid", (bdcDimX+20)/2, (bdcDimY+20)/2, (bdcDimZ+bdcMylarT)/2);
		G4LogicalVolume* BDCLog = new G4LogicalVolume(BDCSol, fNist->FindOrBuildMaterial("G4_AIR"), "BDCLogic");
		BDCLog->SetVisAttributes(G4VisAttributes::GetInvisible());

		//p10
		G4Box*           BDCSolP10 = new G4Box("BDCSolidP10", bdcDimX/2, bdcDimY/2, bdcDimZ/2);
		G4LogicalVolume* BDCLogP10 = new G4LogicalVolume(BDCSolP10, P10Gas, "BDCLogicP10");
		new G4PVPlacement(0, fZero, BDCLogP10, "BDCP10", BDCLog, false, bdcID + 10, false);

		auto fVisBDC = new G4VisAttributes();
		fVisBDC->SetColor(G4Color::Yellow());
		fVisBDC->SetForceWireframe(true);
		BDCLogP10->SetVisAttributes(fVisBDC);

		if (fPar->GetParBool("DetFrameOn"))
		{
			//Mylar sheets (upstream/downstream)
			G4Box*           BDCSolMylar = new G4Box("BDCSolidMylar", (bdcDimX+20)/2, (bdcDimY+20)/2, bdcMylarT/2);
			G4LogicalVolume* BDCLogMylar = new G4LogicalVolume(BDCSolMylar, Mylar, "BDCLogicMylar");
			G4ThreeVector MylarPosU(0, 0, -(bdcDimZ + bdcMylarT)/2);
			G4ThreeVector MylarPosD(0, 0, +(bdcDimZ + bdcMylarT)/2);
			new G4PVPlacement(0, MylarPosU, BDCLogMylar, "BDCMylarU", BDCLog, false, bdcID + 20, false);
			new G4PVPlacement(0, MylarPosD, BDCLogMylar, "BDCMylarD", BDCLog, false, bdcID + 21, false);
			BDCLogMylar->SetVisAttributes(fVisFrame);

			//Aluminized mylar (assume entire thickness is Al oxide for now)
			G4Box*           BDCSolAlm = new G4Box("BDCSolidAlm", bdcDimX/2, bdcDimY/2, bdcAlmT/2);
			G4LogicalVolume* BDCLogAlm = new G4LogicalVolume(BDCSolAlm, AlOxide, "BDCLogicAlm");
			for (int i=0; i<nAlm; i++)
			{
				G4ThreeVector AlmPos(0, 0, -bdcDimZ/2 + (bdcDimZ/(float)nAlm) * (i+1));
				new G4PVPlacement(0, AlmPos, BDCLogAlm, Form("BDCAlm%i", i), BDCLog, false, bdcID + 30+i, false);
			}
			BDCLogAlm->SetVisAttributes(fVisFrame);
		}

		G4ThreeVector BDCPos0(0, 0, bdcPosZ0 + bdcDimZ/2.);
		G4ThreeVector BDCPos1(0, 0, bdcPosZ1 + bdcDimZ/2.);
		auto BDCPhys0 = new G4PVPlacement(0, BDCPos0, BDCLog, "BDC0", WorldLog, false, bdcID+0, true);
		auto BDCPhys1 = new G4PVPlacement(0, BDCPos1, BDCLog, "BDC1", WorldLog, false, bdcID+1, true);
		fRun->SetSensitiveDetector(BDCPhys0);
		fRun->SetSensitiveDetector(BDCPhys1);
	}//BDC

	//Target (C2H4)
	//--------------------------------------------------------------------

	if (fPar->GetParBool("TargetIn"))
	{
		G4Material* targetMat = fNist->FindOrBuildMaterial("G4_POLYETHYLENE");

		G4int    targetID   = fPar->GetParInt("targetID");
		G4double targetDimX = fPar->GetParDouble("targetDimX");
		G4double targetDimY = fPar->GetParDouble("targetDimY");
		G4double targetDimZ = fPar->GetParDouble("targetDimZ");
		G4double targetPosZ = fPar->GetParDouble("targetPosZ");
		G4ThreeVector targetPos(0, 0, targetPosZ + targetDimZ/2);

		G4Box*           TargetSol = new G4Box("TargetSolid", targetDimX/2, targetDimY/2, targetDimZ/2);
		G4LogicalVolume* TargetLog = new G4LogicalVolume(TargetSol, targetMat, "TargetLogic");
		new G4PVPlacement(0, targetPos, TargetLog, "Target", WorldLog, false, targetID, true);

		auto fVisTarget = new G4VisAttributes();
		fVisTarget->SetColor(G4Color::White());
		fVisTarget->SetForceWireframe(true);
		TargetLog->SetVisAttributes(fVisTarget);
	}//Target

	//TOF
	//--------------------------------------------------------------------

	if (fPar->GetParBool("BTOFIn") || fPar->GetParBool("FTOFIn"))
	{
		G4Material* tofMat = fNist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

		G4double tofSlatL_b = fPar->GetParDouble("tofSlatL_b");
		G4double tofSlatL_f = fPar->GetParDouble("tofSlatL_f");
		G4double tofSlatW   = fPar->GetParDouble("tofSlatW");
		G4double tofSlatT   = fPar->GetParDouble("tofSlatT");

		//A module
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++

		G4RotationMatrix* rotZ90 = new G4RotationMatrix(); rotZ90->rotateZ(90 * deg);
		G4RotationMatrix* rotZ180 = new G4RotationMatrix(); rotZ180->rotateZ(180 * deg);

		//Enclosure volume (box + fishtails)
		G4Box* TOFSolM = new G4Box("TOFSolM", tofSlatW/2, (tofSlatL_b+2*tofSlatL_f)/2, tofSlatT/2);
		G4LogicalVolume* TOFLogM = new G4LogicalVolume(TOFSolM, WorldLog->GetMaterial(), "TOFLogM");
		TOFLogM->SetVisAttributes(G4VisAttributes::GetInvisible());

		//Box at center
		G4Box*           TOFSolM_b = new G4Box("TOFSolM_b", tofSlatW/2, tofSlatL_b/2, tofSlatT/2);
		G4LogicalVolume* TOFLogM_b = new G4LogicalVolume(TOFSolM_b, tofMat, "TOFLogM_b");

		//Fish tails
		G4Trap* TOFSolM_f = new G4Trap("TOFSolM_f");
		TOFSolM_f->SetAllParameters(
				tofSlatT/2, 0, 0,
				tofSlatL_f/2, tofSlatW/2, tofSlatW/4, 0,
				tofSlatL_f/2, tofSlatW/2, tofSlatW/4, 0);
		G4LogicalVolume* TOFLogM_f = new G4LogicalVolume(TOFSolM_f, tofMat, "TOFLogM_f");

		//Visualization
		auto fVisTOF = new G4VisAttributes();
		fVisTOF->SetColor(G4Color::Brown());
		fVisTOF->SetForceWireframe(true);
		TOFLogM_b->SetVisAttributes(fVisTOF);
		TOFLogM_f->SetVisAttributes(fVisTOF);

		//FTOF
		//++++++++++++++++++++++++++++++++++++++++++++++++++++++

		if (fPar->GetParBool("FTOFIn"))
		{
			G4int    ftofID    = fPar->GetParInt("ftofID");
			G4int    ftofSlatN = fPar->GetParInt("ftofSlatN");
			G4double ftofPosY  = fPar->GetParDouble("ftofPosY");
			G4double ftofPosZ  = fPar->GetParDouble("ftofPosZ");

			new G4PVPlacement(0, fZero, TOFLogM_b, "TOFM_b", TOFLogM, false, ftofID, true);
			new G4PVPlacement(0, G4ThreeVector(0, (tofSlatL_b + tofSlatL_f)/2, 0),
					TOFLogM_f, "TOFM_f0", TOFLogM, false, ftofID, true);
			new G4PVPlacement(rotZ180, G4ThreeVector(0, -(tofSlatL_b + tofSlatL_f)/2, 0),
					TOFLogM_f, "TOFM_f1", TOFLogM, false, ftofID, true);

			for (int a=0; a<ftofSlatN; a++) //# of slat pairs, by radial distance from beam axis
			for (int b=0; b<2; b++) //Pair
			{
				const int mID = a*10 + b;
				const float posY = tofSlatW*a + tofSlatW/2 + ftofPosY;
				auto FTOFPhy = new G4PVPlacement(rotZ90, G4ThreeVector(0, b==0?posY:(-posY), ftofPosZ + tofSlatT/2),
						TOFLogM, Form("FTOF_%i", mID), WorldLog, false, ftofID+mID, true);
				fRun->SetSensitiveDetector(FTOFPhy);
			}//a, b
		}//FTOF

	}//B/FTOF

	//ND
	//--------------------------------------------------------------------

	if (fPar->GetParBool("NDIn"))
	{
		G4Material* ndMat = fNist->FindOrBuildMaterial("G4_PLASTIC_SC_VINYLTOLUENE");

        G4int    ndID    = fPar->GetParInt("ndID");
		G4int    ndSlatN = fPar->GetParInt("ndSlatN"); //# of pairs: i.e., 1 means two slats
        G4double ndSlatL = fPar->GetParDouble("ndSlatL"); //Slat length (x)
        G4double ndSlatW = fPar->GetParDouble("ndSlatW"); //Slat width (y)
        G4double ndSlatT = fPar->GetParDouble("ndSlatT"); //Slat thickness (z)
		G4double ndPosY  = fPar->GetParDouble("ndPosY");
		G4double ndPosZ  = fPar->GetParDouble("ndPosZ");

		G4Box*           NDSol = new G4Box("NDSol", ndSlatL/2, ndSlatW/2, ndSlatT/2);
		G4LogicalVolume* NDLog = new G4LogicalVolume(NDSol, ndMat, "NDLog");

		auto fVisND = new G4VisAttributes();
		fVisND->SetColor(G4Color::Magenta());
		fVisND->SetForceWireframe(true);
		NDLog->SetVisAttributes(fVisND);
		NDLog->SetVisAttributes(fVisND);

		for (int a=0; a<ndSlatN; a++) //# of slat pairs,  by radial distance from beam axis
		for (int b=0; b<2; b++) //Pair
		{
			const short mID  = a*10 + b + 1;
			const float posY = ndSlatW*a + ndSlatW/2 + ndPosY;
			G4ThreeVector NDPos(0, b==0?posY:(-posY), ndSlatT/2 + ndPosZ);
			auto NDPhy = new G4PVPlacement(0, NDPos, NDLog, Form("ND_%i", mID), WorldLog, false, ndID+mID, true);
			fRun->SetSensitiveDetector(NDPhy);
		}//a, b
	}//ND

	return WorldPhy;
}//Construct