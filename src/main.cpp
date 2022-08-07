#include <iostream>

#include <agrum/BN/BayesNet.h>

#include "CausalModel.h"
#include "doorCriteria.h"
#include "dSeparation.h"




int main(void) {
  gum::BayesNet<double> h;
  gum::CausalModel test(h);
  std::cout << "hello" << std::endl;
}
