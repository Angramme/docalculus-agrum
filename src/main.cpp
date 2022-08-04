#include <iostream>

#include <agrum/BN/BayesNet.h>

#include "CausalModel.cpp"




int main(void) {
  gum::BayesNet<double> h;
  CausalModel test(h);
  std::cout << "hello" << std::endl;
}
