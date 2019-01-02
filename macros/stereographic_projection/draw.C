#include "style.h"
using namespace style;

void draw()
{
  auto helix = new KBHelixTrack();
  auto graph = new TGraph();

  ifstream hits("hits.dat");
  Double_t x, y, z;
  while (hits>>x>>y>>z) {
    //x = x-2972;
    //y = y+10850;
    x = x-500;
    y = y+1000;
    helix -> AddHit(new KBHit(x,y,z,1));
    graph -> SetPoint(graph->GetN(),x,y);
  }

  make(graph);
  cv1();
  helix -> Fit();
  helix -> Print();

  cv1();
  graph -> SetMarkerStyle(24);
  graph -> Draw("ap");
  helix -> TrajectoryOnPlane(KBVector3::kX,KBVector3::kY) -> Draw("samel");
}
