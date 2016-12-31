#include "KBTask.hh"

#include <iostream>
using namespace std;

KBTask::KBTask()
:TTask()
{
}

KBTask::KBTask(const char* name, const char *title)
:TTask(name, title)
{
}

bool KBTask::InitTask() 
{
  if (!fActive) 
    return false;

  bool initialized = Init();
  if (!initialized)
    return false;

  return InitTasks();
}

bool KBTask::Init() 
{
  return true;
}

bool KBTask::InitTasks()
{
  TIter iter(GetListOfTasks());
  KBTask* task;

  while ( (task = dynamic_cast<KBTask*>(iter())) ) {
    cout << "  " << "Initializing " << task -> GetName() << "." << endl;
    if (task -> Init() == false) {
      cout << "  Initialization failed!" << endl;
      return false;
    }
  }

  return true;
}
