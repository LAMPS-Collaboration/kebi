void doSomthing()
{
  auto run = new KBRun();
  run -> SetRunID(0);
  run -> SetTag("test");
  run -> AddParameterFile("dummy.par");
  run -> AddDetector(new DUMMYDetector());

  auto task = new PROEJECTDoSomethingTask();
  run -> Add(task);

  run -> Init();
  run -> Run();
}
