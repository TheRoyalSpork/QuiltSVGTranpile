#include "Testing.h"

vector<Test *> tests;

void runTests() {
  for_each(tests.begin(),tests.end(),Test::check);
}

