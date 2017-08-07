#include "LAPTpc.hh"
#include "LAPPadPlane.hh"

LAPTpc::LAPTpc()
:KBTpc("LampsTpcPrototype","LAMPS TPC Prototype")
{
}

bool LAPTpc::BuildGeometry()
{
  if (fGeoManager == nullptr) {
    new TGeoManager();
    fGeoManager = gGeoManager;
    fGeoManager -> SetVerboseLevel(0);
    fGeoManager -> SetNameTitle("LAMPS TPC Prototype", "LAMPS TPC Prototype Geometry");
  }

  Double_t rMinTPC, rMaxTPC, dyTPC, yOffset;
  fPar -> GetParDouble("rMinTPC", rMinTPC);
  fPar -> GetParDouble("rMaxTPC", rMaxTPC);
  fPar -> GetParDouble("dyTPC", dyTPC);
  fPar -> GetParDouble("yOffset", yOffset);

  TGeoMedium *p10 = new TGeoMedium("p10", 1, new TGeoMaterial("p10"));

  TGeoVolume *top = new TGeoVolumeAssembly("TOP");
  fGeoManager -> SetTopVolume(top);
  fGeoManager -> SetTopVisible(true);

  TGeoVolume *tpc = new TGeoVolumeAssembly("TPC");
  TGeoTranslation *offTPC = new TGeoTranslation("TPC offset",0,yOffset,0);

  TGeoVolume *gas = fGeoManager -> MakeTube("gas",p10,rMinTPC,rMaxTPC,dyTPC/2);
  gas -> SetVisibility(true);
  gas -> SetLineColor(kBlue-10);
  gas -> SetTransparency(90);

  TGeoRotation *rotation = new TGeoRotation("rotation",0,90,0); 

  top -> AddNode(tpc, 1, offTPC);
  tpc -> AddNode(gas, 1, rotation);

  fGeoManager -> CloseGeometry();

  return true;
}

bool LAPTpc::BuildDetectorPlane()
{
  LAPPadPlane *padplane = new LAPPadPlane();
  padplane -> SetParameterContainer(fPar);
  padplane -> SetPlaneID(0);
  padplane -> SetPlaneK(fPlaneK[0]);
  padplane -> Init();

  AddPlane(padplane);

  return true;
}
