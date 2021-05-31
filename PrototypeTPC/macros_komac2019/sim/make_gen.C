void make_gen()
{
  TString runName = "sim_komac";
  TString runName = "sim_komac_sm";
  TString dirName = runName;

  TString particleName = "proton";
  int numEvents = 100;
  int numTracks = 1;
  double pval = 200;
  double vx = 0;
  double vy = -285;
  double vz = 0;
  auto rotationAxis = KBVector3::kY;
  double theta1 = 20;
  double theta2 = 160;
  double dtheta = 5;
  double stheta = 0;
  double phi1 = 0;
  double phi2 = 24;
  double dphi = 1;
  double sphi = 0;

  if (runName="sim_komac_sm") {
    phi1 = 90;
    phi2 = 114;
  }

  auto mcgen = new KBMCEventGenerator();
  if (!dirName.IsNull())
    dirName = mcgen -> SetDirectory(dirName);
  mcgen -> AddPar("prototype_komac.par");
  mcgen -> GetPar() -> SetPar(Form("genNumEvents int %d", numEvents));
  mcgen -> GetPar() -> SetPar(Form("genMomentum double %f", pval));
  mcgen -> GetPar() -> SetPar(Form("genVertexPosition v3 %f %f %f", vx, vy, vz));
  mcgen -> GetPar() -> SetPar(Form("genTotalThetaBinning o double[] %f %f %f %f", theta1, theta2, dtheta, stheta));
  mcgen -> GetPar() -> SetPar(Form("genTotalPhiBinning   o double[] %f %f %f %f", phi1, phi2, dphi, sphi));


  int runID = 0;
  for (double theta=theta1; theta<=theta2; theta+=dtheta)
    for (double phi=phi1; phi<=phi2; phi+=dphi)
      runID++;
  mcgen -> GetPar() -> SetPar(Form("genNumRuns o int %d", runID+1));


  runID = 0;
  std::ofstream fileAll(Form("all_%s.sh",runName.Data()));
  for (double theta=theta1; theta<=theta2; theta+=dtheta) {
    for (double phi=phi1; phi<=phi2; phi+=dphi)
    {
      double thetaRange[2] = {theta-stheta, theta+stheta};
      double phiRange[2] = {phi-sphi, phi+sphi};

      mcgen -> GetPar() -> SetPar(Form("genThetaRange o double[] %f %f", thetaRange[0], thetaRange[1]));
      mcgen -> GetPar() -> SetPar(Form("genPhiRange o double[] %f %f", phiRange[0], phiRange[1]));

      auto genName = mcgen -> CreateGenRandom(runName,runID,numEvents,numTracks
                              ,vx, vy, vz
                              ,particleName, pval
                              ,thetaRange[0], thetaRange[1]
                              ,phiRange[0], phiRange[1]
                              ,rotationAxis);


      TString runNameFull = Form("%s%04d",runName.Data(),runID);
      fileAll << Form("./run_sim_komac.g4sim %s%s.conf",dirName.Data(),runNameFull.Data()) << endl;
      fileAll << Form("root -q -l -b 'run_digi_reco_mc.C(\"%s\")'",runNameFull.Data()) << endl;

      runID++;
    }
  }


  gApplication -> Terminate();
}
