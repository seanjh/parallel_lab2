#include <list>
#include <string>
#include <iostream>
#include <memory>
#include <gmpxx.h>

#include "MW_API.hpp"
#include "DivisorApplication.hpp"
#include "Work.hpp"
#include "Result.hpp"

using namespace std;

int main(int argc, char* argv[])
{
  //cout <<argc << endl;
  shared_ptr<DivisorApplication> app;
  if (argc > 2) {
    string valueString(argv[1]);
    string workSizeString(argv[2]);
    app = make_shared<DivisorApplication>(valueString, workSizeString);
  } else {
    string valueString(argv[1]);
    app = make_shared<DivisorApplication>(valueString);
  }

  // runApp(app);
  MW_Run(argc, argv, app);

  // delete app;
  return 0;
}
