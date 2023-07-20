#include <iostream>

#include <agrum/BN/BayesNet.h>

#include "CausalModel.h"
#include "CausalFormula.h"
#include "CausalImpact.h"
#include "doorCriteria.h"
#include "dSeparation.h"
#include "doCalculus.h"
#include "doAST.h"
#include <memory>

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
  // auto test = ASTsum<double>(
  //   {"aa", "bb", "cc"}, 
  //   std::make_unique<ASTJointProba<double>>(
  //     std::make_unique<NameSet>({"pp1", "pp2"})
  //     ));
  // auto ns = std::shared_ptr<NameSet>(new Set<std::string>({"pp1", "pp2"}));
  // auto jp = std::make_unique<ASTJointProba<double>>(
  //     std::shared_ptr<NameSet>(new Set<std::string>({"pp1", "pp2"})));
  // std::cout << *jp << std::endl;

  // auto jp2 = *jp;

  // auto test = ASTsum<double>({"aa", "bb", "cc"}, std::move(jp));
  // std::cout << test << std::endl;

  auto xs = std::vector<std::unique_ptr<ASTtree<double>>>();
  xs.emplace_back(std::make_unique<ASTJointProba<double>>(
      std::shared_ptr<NameSet>(new Set<std::string>({"pp1", "pp2"}))));
  xs.emplace_back(std::make_unique<ASTJointProba<double>>(
      std::shared_ptr<NameSet>(new Set<std::string>({"pp3", "pp4"}))));
  // auto ret = productOfTreesI<double>(xs.begin(), xs.end());
  auto ret = productOfTrees(xs);

  // auto ii = backdoor_generator(obs1, 0, 1);

  return 0;
}
