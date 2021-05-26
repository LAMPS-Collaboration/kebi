void draw_gen()
{
  auto mcgen = new KBMCEventGenerator();
  mcgen -> AddGenFile("sim_komac0000.gen");
  mcgen -> AddGenFile("sim_komac0001.gen");
  mcgen -> AddGenFile("sim_komac0002.gen");
  mcgen -> AddGenFile("sim_komac0003.gen");
  mcgen -> AddGenFile("sim_komac0004.gen");
  mcgen -> AddGenFile("sim_komac0005.gen");
  mcgen -> AddGenFile("sim_komac0006.gen");
  mcgen -> AddGenFile("sim_komac0007.gen");
  mcgen -> AddGenFile("sim_komac0008.gen");
  mcgen -> AddGenFile("sim_komac0009.gen");
  mcgen -> AddGenFile("sim_komac0010.gen");
  mcgen -> AddGenFile("sim_komac0011.gen");
  mcgen -> AddGenFile("sim_komac0012.gen");
  mcgen -> Summary();
}
