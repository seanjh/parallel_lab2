

//#include <list>
//#include <gmpxx.h>
#include "Work.hpp"
#include "Result.hpp"
#include "MW_API.hpp"
#include "DivisorApplication.hpp"
#include <list>
#include <string>
#include <iostream>

void runApp(MW_API *app)
{
  std::list<Work *> *workToDo = app->work();
  std::cout << "done work" << std::endl;
  std::list<Result *> *results = new std::list<Result *>();
  

  std::cout << "created results" << std::endl;
  for(auto iter = workToDo->begin();
      iter != workToDo->end();
      iter++)
  {
    std::cout << "adding result" << std::endl;
    Work *work = *iter;
    results->push_back(work->compute());
  }

  std::cout << "propogating results" << std::endl;
  app->results(results);
  delete results;
}

int main(int argc, char* argv[])
{
  std::string inputString(argv[1]);
  DivisorApplication *app = new DivisorApplication(inputString);

  runApp(app);

  delete app;
  return 0;
}
