

//#include <list>
#include <gmpxx.h>
#include "Work.hpp"
#include "Result.hpp"
#include "MW_API.hpp"
#include "DivisorApplication.hpp"
#include <list>
#include <string>
#include <iostream>

// void runApp(MW_API *app)
// {
//   std::list<Work *> *workToDo = app->work();
//   std::cout << "done work" << std::endl;
//   std::list<Result *> *results = new std::list<Result *>();
//
//
//   std::cout << "created results" << std::endl;
//   for(auto iter = workToDo->begin();
//       iter != workToDo->end();
//       iter++)
//   {
//     std::cout << "adding result" << std::endl;
//     Work *work = *iter;
//     results->push_back(work->compute());
//   }
//
//   std::cout << "propogating results" << std::endl;
//   app->results(results);
//   delete results;
// }

int main(int argc, char* argv[])
{
  //std::cout <<argc << std::endl;
  DivisorApplication *app;
  if (argc > 2) {
    std::string valueString(argv[1]);
    std::string workSizeString(argv[2]);
    app = new DivisorApplication(valueString, workSizeString);
  } else {
    std::string valueString(argv[1]);
    app = new DivisorApplication(valueString);
  }

  // runApp(app);
  MW_Run(argc, argv, app);

  delete app;
  return 0;
}
