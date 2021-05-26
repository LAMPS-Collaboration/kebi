void make_gen()
{
  TString runName = "sim_komac";
  TString particleName = "proton";
  int numEvents = 100;
  int numTracks = 1;
  auto rotationAxis = KBVector3::kY;
  double pVal = 500;
  double theta1 = 60;
  double theta2 = 120;
  double dPhi = 5;

  std::ofstream fileAll(Form("all_%s.sh",runName.Data()));

  auto mcgen = new KBMCEventGenerator();
  mcgen -> AddPar("prototype_komac.par");
  int runID = 0;
  for (double phi=-30; phi<=30; phi+=dPhi)
  {
    auto fileName = mcgen -> CreateGenRandom(runName,runID,numEvents,numTracks,0,-300,0,
        particleName,pVal,theta1,theta2,phi-.5*dPhi,phi+.5*dPhi, rotationAxis);

    TString runNameFull = Form("%s%04d",runName.Data(),runID);
    fileAll << Form("./run_sim_komac.g4sim %s.conf",runNameFull.Data()) << endl;
    fileAll << Form("root -q -l -b 'run_digi.C(\"%s\")'",runNameFull.Data()) << endl;
    fileAll << Form("root -q -l -b 'run_reco_mc.C(\"%s\")'",runNameFull.Data()) << endl;

    runID++;
  }
}
