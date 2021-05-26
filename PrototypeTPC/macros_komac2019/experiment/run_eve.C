Int_t fEventID = 0;
TString eveOption = "";

void next(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID, eveOption);
  cout << "Event " << fEventID << endl;
}


//void run_eve(TString name = "komac0036.reco.cfcf723.root")
void run_eve(TString name = "/Users/ejungwoo/kebi/data/komac0036.tf.2e5c877.root")
{
  auto run = new KBRun();
  run -> SetInputFile(name);
  run -> AddPar("prototype_eve.par");
  run -> SetTag("eve");
  run -> AddDetector(new LAPTpc());
  run -> Init();
  run -> SetGeoTransparency(80);
  run -> Print();
  run -> RunEve(fEventID, eveOption);
}
