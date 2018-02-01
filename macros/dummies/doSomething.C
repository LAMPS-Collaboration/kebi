void doSomething()
{
  auto run = new KBRun();
  run -> SetRunID(0);
  run -> SetTag("test");
  run -> AddParameterFile("dummy.par");
  run -> AddDetector(new DUMMYDetector());

  auto task = new DUMMYDoSomethingTask();
  run -> Add(task);

  run -> Init();
  run -> Run();
}
