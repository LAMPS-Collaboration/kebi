Int_t fEventID = 0;
TString eveOption = "";

void next(Int_t eventID = -1) {
  if (eventID < 0) KBRun::GetRun() -> RunEve(++fEventID);
  else KBRun::GetRun() -> RunEve(fEventID = eventID, eveOption);
  cout << "Event " << fEventID << endl;
}


void run_eve(TString name = "/Users/ejungwoo/kebi/data/sim_komac0005.recomc.2e5c877.root")
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
