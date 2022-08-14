#include <iostream>

#include <agrum/BN/BayesNet.h>

#include "CausalModel.h"
#include "doorCriteria.h"
#include "dSeparation.h"
#include "doCalculus.h"
#include "doAST.h"

using namespace gum;

int main(void) {
  // gum::BayesNet<double> h;
  // gum::CausalModel test(h);
  // std::cout << "hello" << std::endl;

  auto obs1 = BayesNet<double>::fastPrototype("Smoking->Cancer");


  obs1.cpt("Smoking").fillWith({0.6,0.4});
  obs1.cpt("Cancer").fillWith({0.9,0.1,0.7,0.3}); 

  std::cout << "the BN \n" << obs1 << std::endl;
  std::cout << "the joint distribution \n" << obs1.cpt("Smoking") * obs1.cpt("Cancer") << std::endl;
  std::cout << "the marginal for $smoking$ \n" << obs1.cpt("Smoking") << std::endl;
  std::cout << "the CPT for $cancer$ \n" << obs1.cpt("Cancer") << std::endl;


  // the Bayesian network is causal
  auto modele1 = CausalModel(obs1);

  // cslnb.showCausalImpact(modele1,"Cancer", "Smoking", values={"Smoking":1})


  // TESTS
  // ASTBinaryOp test("hello 3", std::make_unique<ASTtree>("hello 1 "), std::make_unique<ASTtree>("hello 2"));

  // std::cout << "\n\n test begins here: " << std::endl;
  // std::cout << test << std::endl;
  //
}
