void doSomthing()
{
  auto run = new KBRun();
  run -> SetRunID(0);
  run -> SetTag("test");
  run -> AddParameterFile("dummy.par");
  run -> AddDetector(new EXDetector());

  auto task = new EXDoSomethingTask();
  run -> Add(task);

  run -> Init();
  run -> Run();
}
