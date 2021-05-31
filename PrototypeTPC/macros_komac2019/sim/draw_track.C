#include "KBGlobal.hh"

//void draw_track(int runID=15, TString runName="sim_komac")
//void draw_track(int runID=0, TString runName="sim_komac")
void draw_track(int runID=25, TString runName="sim_komac")
{
  double distCut = 10.; // mm
  int numLimitPrint = 20;
  bool drawTrajectory = true;

  auto run = new KBRun();
  auto tpc = new LAPTpc();
  run -> AddDetector(tpc);
  run -> AddInputFile(Form("%s%04d.recomc",runName.Data(),runID));
  run -> Init();
  auto hitArray = run -> GetBranchA("Hit");
  auto trackArray = run -> GetBranchA("Tracklet");

  auto par = run -> GetPar();
  auto vPos = par -> GetParV3("genVertexPosition");
  auto r1 = par -> GetParDouble("rMinTPC");
  auto r2 = par -> GetParDouble("rMaxTPC");
  auto genTheta = (par -> GetParDouble("genThetaRange",0) + par -> GetParDouble("genThetaRange",1))/2.;
  auto genPhi = (par -> GetParDouble("genPhiRange",0) + par -> GetParDouble("genPhiRange",1))/2.;

  cout << genTheta << " " << genPhi << endl;

  TVector3 dirTrue(0,0,1);
  dirTrue.SetTheta(genTheta*TMath::DegToRad());
  dirTrue.SetPhi(genPhi*TMath::DegToRad());
  dirTrue.SetMag(400);
  dirTrue = KBVector3(KBVector3::kY,dirTrue).GetXYZ();
  auto trackTrue = new KBLinearTrack(vPos,vPos+dirTrue);

  int idx=0;
  TCanvas *cvsTop;
  TCanvas *cvsSide;
  if (drawTrajectory) {
    auto histPP = tpc -> GetPadPlane() -> GetHist();
    auto histTop = new TH2D("histTop",";z;x;",100,-250,250,100,-250,250);
    cvsTop = new TCanvas(histTop->GetName(),histTop->GetName(),idx*25,idx*25,550,550);
    histTop -> Draw();
    tpc -> GetPadPlane() -> DrawFrame();
    histPP -> Draw("same");
    trackTrue -> ResetTrajectory();
    auto traj = trackTrue -> TrajectoryOnPlane(KBVector3::kZ,KBVector3::kX);
    traj -> SetLineColor(kBlack);
    traj -> Draw("same");

    //auto histSide = new TH2D("histSide",";z;y;",100,-250,250,100,-570,0);
    auto histSide = new TH2D("histSide",";z;y;",100,-250,250,100,-600,30);
    cvsSide = new TCanvas(histSide->GetName(),histSide->GetName(),idx*25,idx*25,550,550);
    histSide -> Draw();
    auto line1 = new TLine( r1,-570, r1,0); line1 -> Draw();
    auto line2 = new TLine(-r1,-570,-r1,0); line2 -> Draw();
    auto line3 = new TLine( r2,-570, r2,0); line3 -> Draw();
    auto line4 = new TLine(-r2,-570,-r2,0); line4 -> Draw();
    auto line5 = new TLine(-r2,-570, r2,-570); line5 -> Draw();
    auto line6 = new TLine(-r2,-0, r2,0); line6 -> Draw();
    trackTrue -> ResetTrajectory();
    traj = trackTrue -> TrajectoryOnPlane(KBVector3::kZ,KBVector3::kY);
    traj -> SetLineColor(kBlack);
    traj -> Draw("same");

    idx++;
  }

  auto histNH = new TH1D("histNH",";Number of hits per event",200,0,200);
  auto histNT = new TH1D("histNT",";Number of tracks per event",20,0,20);
  auto histNHT = new TH1D("histNHT",";Number of hits per track",40,0,40);
  auto histDip = new TH1D("histDip",";Dip angle [deg]",100,-100,100);
  auto histDist = new TH1D("histDist",";Distance to TPC center-axis (mm)",100,0,10);
  auto histRMS = new TH1D("histRMS",";RMS",100,0,20);
  auto histTheta = new TH1D("histTheta",";#theta [deg]",100,0,180);
  auto histThetaError = new TH1D("histThetaError",";#theta error (#theta_{true} - #theta) [deg]",100,-5,5);
  auto histPhi = new TH1D("histPhi",";#phi [deg]",100,-35,35);
  auto histPhiError = new TH1D("histPhiError",";#phi error (#phi_{true} - #phi) [deg]",100,-5,5);

  vector<TH1D *> hists1;
  hists1.push_back(histNH);
  hists1.push_back(histNT);
  hists1.push_back(histNHT);
  //hists1.push_back(histDip);
  hists1.push_back(histDist);
  hists1.push_back(histTheta);
  hists1.push_back(histThetaError);
  hists1.push_back(histPhi);
  hists1.push_back(histPhiError);

  int countFakeTracks = 0;
  int countTrueTracks = 0;
  vector<int> eventsWithOneTrueTracks;
  vector<int> eventsWithMoreThanOneTrueTracks;
  vector<int> eventsWithNoTrueTracks;

  int numHitsPerEvent = 0;

  auto numEvents = run -> GetEntries();
  for (auto event=0; event<numEvents; ++event)
  {
    run -> GetEvent(event);

    auto numHits = hitArray -> GetEntries();
    histNH -> Fill(numHits);
    numHitsPerEvent += numHits;

    auto numTracks = trackArray -> GetEntries();
    histNT -> Fill(numTracks);

    int countTrueTracksInEvent = 0;

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

      if (sqrt(p1.Z()*p1.Z()+p1.X()*p1.X())>sqrt(p2.Z()*p2.Z()+p2.X()*p2.X())) {
        track -> SetLine(p2,p1);
        p1 = track -> GetPoint1();
        p2 = track -> GetPoint2();
      }

      auto dir = track -> Direction();
      auto dip = TMath::ATan2(dir.Y(),sqrt(dir.X()*dir.X()+dir.Z()*dir.Z()))*TMath::RadToDeg();
      auto dirY = TVector3(dir.Z(),dir.X(),dir.Y());
      auto dirZX = TVector3(dir.Z(),dir.X(),0).Unit();
      auto dirZY = TVector3(dir.Z(),dir.Y(),0).Unit();

      if (abs(dist)>distCut)
      {
        countFakeTracks++;
        if (drawTrajectory) {
          cvsTop -> cd(); 
          double length = 200;
          track -> ResetTrajectory();
          auto traj = track -> TrajectoryOnPlane(KBVector3::kZ,KBVector3::kX);
          traj -> SetLineColor(kRed);
          traj -> Draw("samel");

          cvsSide -> cd(); 
          track -> ResetTrajectory();
          traj = track -> TrajectoryOnPlane(KBVector3::kZ,KBVector3::kY);
          traj -> SetLineColor(kRed);
          traj -> Draw("samel");
        }
        continue;
      }
      else
      {
        if (drawTrajectory) {
          cvsTop -> cd(); 
          double length = 200;
          auto traj = new TArrow(poca0.Z(),poca0.X(),poca0.Z()+length*dirZX.X(),poca0.X()+length*dirZX.Y());
          traj -> SetLineColor(kBlue);
          traj -> Draw(">");

          cvsSide -> cd(); 
          traj = new TArrow(poca0.Z(),poca0.Y()+vPos.Y(),poca0.Z()+length*dirZY.X(),poca0.X()+length*dirZY.Y()+vPos.Y());
          traj -> SetLineColor(kBlue);
          traj -> Draw(">");
        }
        countTrueTracks++;
        countTrueTracksInEvent++;
      }

      histDip -> Fill(dip);
      histNHT -> Fill(track -> GetNumHits());
      histRMS -> Fill(track -> GetRMS());
      histTheta -> Fill(dirY.Theta()*TMath::RadToDeg());
      histThetaError -> Fill(genTheta - dirY.Theta()*TMath::RadToDeg());
      histPhi -> Fill(dirY.Phi()*TMath::RadToDeg());
      histPhiError -> Fill(genPhi - dirY.Phi()*TMath::RadToDeg());
    }

    if (countTrueTracksInEvent==1) eventsWithOneTrueTracks.push_back(event);
    if (countTrueTracksInEvent>1) eventsWithMoreThanOneTrueTracks.push_back(event);
    if (countTrueTracksInEvent==0) eventsWithNoTrueTracks.push_back(event);
  }

  for (auto hist : hists1) {
    new TCanvas(hist->GetName(),hist->GetName(),idx*25,idx*25,550,500);
    hist -> Draw();
    idx++;
  }


  cout << endl;
  cout_info << "true-tracks (dist<"<<distCut<<") = " << countTrueTracks << endl;
  cout_info << "secondary-tracks = " << countFakeTracks << endl;
  cout << endl;

  cout_info << "dist        = " << histDist       -> GetMean() << " +- " << histDist       -> GetStdDev() << endl;
  cout_info << "theta error = " << histThetaError -> GetMean() << " +- " << histThetaError -> GetStdDev() << endl;
  cout_info << "phi error   = " << histPhiError   -> GetMean() << " +- " << histPhiError   -> GetStdDev() << endl;
  cout << endl;

  TString summaryName = Form("%s/summary_%s%04d.txt",runName.Data(),runName.Data(),runID);
  cout_info << "summary : " << summaryName << endl;
  ofstream summary_file(summaryName);
  summary_file << genTheta << " " << genPhi << " " << par -> GetParDouble("genMomentum") << endl;
  summary_file << histDist       -> GetMean() << " " << histDist       -> GetStdDev() << endl;
  summary_file << histThetaError -> GetMean() << " " << histThetaError -> GetStdDev() << endl;
  summary_file << histPhiError   -> GetMean() << " " << histPhiError   -> GetStdDev() << endl;
  summary_file << countTrueTracks << " " << par -> GetParInt("genNumEvents") << endl;
  summary_file << double(numHitsPerEvent) / par -> GetParInt("genNumEvents") << endl;
  cout << endl;

  cout_info << "events with 1 true-tracks = " << eventsWithOneTrueTracks.size() << ":  ";
  if (eventsWithOneTrueTracks.size()<numLimitPrint)
    for (auto event : eventsWithOneTrueTracks)
      cout << event << ", ";
  cout << endl;

  cout_info << "events with >1 true-tracks = " << eventsWithMoreThanOneTrueTracks.size() << ":  ";
  if (eventsWithMoreThanOneTrueTracks.size()<numLimitPrint)
    for (auto event : eventsWithMoreThanOneTrueTracks)
      cout << event << ", ";
  cout << endl;

  cout_info << "events with no true-tracks = " << eventsWithNoTrueTracks.size() << ":  ";
  if (eventsWithNoTrueTracks.size()<numLimitPrint)
    for (auto event : eventsWithNoTrueTracks)
      cout << event << ", ";
  cout << endl;
}
