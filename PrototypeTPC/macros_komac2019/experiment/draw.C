void draw()
{
  gStyle -> SetOptStat(0);

  auto run = new KBRun();
  auto tpc = new LAPTpc();
  run -> AddDetector(tpc);
  run -> SetInputFile("komac0036.tf");
  run -> Init();
  run -> Print();
  //auto hitArray = run -> GetBranchA("Hit");
  auto trackArray = run -> GetBranchA("Tracklet");

  auto cvsPP = new TCanvas();
  auto histPP = tpc -> GetPadPlane() -> GetHist();
  histPP -> Draw();

  vector<TH1D *> hists1;
  auto histNH = new TH1D("histNH",";Number of hits per event",100,0,500);
  auto histNT = new TH1D("histNT",";Number of tracks per event",20,0,20);
  auto histNHT = new TH1D("histNHT",";Number of hits per track",40,0,40);
  auto histDip = new TH1D("histDip",";Dip angle (deg)",100,-100,100);
  auto histDist = new TH1D("histDist",";Distance to TPC center-axis (mm)",100,0,200);
  auto histRMS = new TH1D("histRMS",";RMS",100,0,20);
  auto histPhi = new TH1D("histPhi",";#phi",100,-180,180);
  hists1.push_back(histNH);
  hists1.push_back(histNT);
  hists1.push_back(histNHT);
  hists1.push_back(histDip);
  hists1.push_back(histDist);
  hists1.push_back(histPhi);

  auto numEvents = run -> GetEntries();
  for (auto event=0; event<numEvents; ++event)
  //for (auto event=0; event<1; ++event)
  {
    run -> GetEvent(event);

    //auto numHits = hitArray -> GetEntries();
    //histNH -> Fill(numHits);
    //if (numHits>200) continue;

    auto numTracks = trackArray -> GetEntries();
    histNT -> Fill(numTracks);
    for (auto iTrack=0; iTrack<numTracks; ++iTrack)
    {
      auto track = (KBLinearTrack *) trackArray -> At(iTrack);

      auto p1 = track -> GetPoint1();
      auto p2 = track -> GetPoint2();
      auto posVertex0 = TVector3(0,0,0);
      auto lineZX = KBGeoLine(p1.Z(),p1.X(),0,p2.Z(),p2.X(),0);
      auto poca0 = lineZX.ClosestPointOnLine(posVertex0);
      auto dist = (poca0-posVertex0).Mag();
      histDist -> Fill(dist);

      if (abs(dist)>10)
        continue;

      if (sqrt(p1.Z()*p1.Z()+p1.X()*p1.X())>sqrt(p2.Z()*p2.Z()+p2.X()*p2.X())) {
        track -> SetLine(p2,p1);
        p1 = track -> GetPoint1();
        p2 = track -> GetPoint2();
      }

      auto dir = track -> Direction();
      auto dip = TMath::ATan2(dir.Y(),sqrt(dir.X()*dir.X()+dir.Z()*dir.Z()))*TMath::RadToDeg();
      auto dir2 = TVector3(dir.Z(),dir.X(),0).Unit();

      histDip -> Fill(dip);
      histNHT -> Fill(track -> GetNumHits());
      histRMS -> Fill(track -> GetRMS());
      histPhi -> Fill(dir2.Phi()*TMath::RadToDeg());

      if (1) {
        double length = 270;
        auto line = new TArrow(poca0.Z(),poca0.X(),poca0.Z()+length*dir2.X(),poca0.X()+length*dir2.Y());
        line -> SetLineColor(kGray);
        line -> Draw(">");
      }
    }
  }

  for (auto hist : hists1) {
    new TCanvas();
    hist -> Draw();
  }
}
