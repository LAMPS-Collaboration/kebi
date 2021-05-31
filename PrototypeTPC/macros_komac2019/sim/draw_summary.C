void draw_summary()
{
  gStyle -> SetOptStat(0);

  //gStyle -> SetPaintTextFormat("0.1g");

  auto par = new KBParameterContainer("sim_komac/sim_komac0000.conf");
  auto thetaBinning = par -> GetParVDouble("genTotalThetaBinning");
  auto phiBinning = par -> GetParVDouble("genTotalPhiBinning");
  auto wTheta = thetaBinning[2];
  auto nTheta = (thetaBinning[1] - thetaBinning[0]) / wTheta + 1;
  auto thetaSpacing = (thetaBinning[1] - thetaBinning[0]) / nTheta;
  auto theta1 = thetaBinning[0] - .5 * wTheta;
  auto theta2 = thetaBinning[1] + .5 * wTheta;
  auto wPhi = phiBinning[2];
  auto nPhi = (phiBinning[1] - phiBinning[0]) / wPhi + 1;
  auto phiSpacing = (phiBinning[1] - phiBinning[0]) / nPhi;
  auto phi1 = phiBinning[0] - .5 * wPhi;
  auto phi2 = phiBinning[1] + .5 * wPhi;

  auto histEff     = new TH2D("histEff"    ,"single track reconstruction efficiency [%] ;#phi_{true};#theta_{true}",nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histDistM   = new TH2D("histDistM"  ,"dist to vertex mean [mm];#phi_{true};#theta_{true}"             ,nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histDistE   = new TH2D("histDistE"  ,"dist to vertex stddev [mm];#phi_{true};#theta_{true}"           ,nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histDThetaM = new TH2D("histDThetaM","#Delta#theta (true-reco) mean [deg];#phi_{true};#theta_{true}"  ,nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histDThetaE = new TH2D("histDThetaE","#Delta#theta (true-reco) stddev [deg];#phi_{true};#theta_{true}",nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histDPhiM   = new TH2D("histDPhiM"  ,"#Delta#phi (true-reco) mean [deg];#phi_{true};#theta_{true}"    ,nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histDPhiE   = new TH2D("histDPhiE"  ,"#Delta#phi (true-reco) stddev [deg];#phi_{true};#theta_{true}"  ,nPhi,phi1,phi2,nTheta,theta1,theta2);
  auto histNH      = new TH2D("histNH"     ,"Number of hits per event;#phi_{true};#theta_{true}"             ,nPhi,phi1,phi2,nTheta,theta1,theta2);

  histEff     -> SetMinimum(0);
  histEff     -> SetMaximum(100);
  histDistM   -> SetMinimum(0);
  histDistM   -> SetMaximum(10);
  histDThetaM -> SetMinimum(-8);
  histDThetaM -> SetMaximum(8);
  histDPhiM   -> SetMinimum(-4);
  histDPhiM   -> SetMaximum(0);

  vector<TH2D *> hists1;
  //hists1.push_back(histEff  );
  hists1.push_back(histDistM  );
  hists1.push_back(histDThetaM);
  hists1.push_back(histDPhiM  );
  //hists1.push_back(histDistE  );
  //hists1.push_back(histDThetaE);
  //hists1.push_back(histDPhiE  );
  hists1.push_back(histNH);

  for (auto runID=0; runID<=724; ++runID) {
    ifstream file(Form("sim_komac/summary_sim_komac%04d.txt",runID));
    double phi, theta, pval, dist, diste, dphi, dphie, dtheta, dthetae;
    int numReco, numTrue, numHitsPerEvent;
    file >> theta >> phi >> pval;
    file >> dist >> diste;
    file >> dtheta >> dthetae;
    file >> dphi >> dphie;
    file >> numReco >> numTrue;
    file >> numHitsPerEvent;

    if (numReco<50) dist=-999;
    if (numReco<50) dtheta=-999;
    if (numReco<50) dphi=-999;

    histEff     -> Fill(phi,theta,double(numReco)/numTrue*100);
    histDistM   -> Fill(phi,theta,dist);
    histDThetaM -> Fill(phi,theta,dtheta);
    histDPhiM   -> Fill(phi,theta,dphi);

    histDistE   -> Fill(phi,theta,diste);
    histDThetaE -> Fill(phi,theta,dthetae);
    histDPhiE   -> Fill(phi,theta,dphie);

    histNH      -> Fill(phi,theta,numHitsPerEvent);
  }

  int idx = 0;

  for (auto hist : {histEff}) {
    new TCanvas(hist->GetName(),hist->GetName(),idx*25,idx*25,1200,800);
    hist -> Draw("colz");
    hist -> Draw("sametext");
    idx++;
  }

  for (auto hist : hists1) {
    new TCanvas(hist->GetName(),hist->GetName(),idx*25,idx*25,1200,800);
    hist -> Draw("colz");
    //hist -> Draw("sametext");
    idx++;
  }
}
