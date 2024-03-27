#ifndef POPULATION_HPP
#define POPULATION_HPP

#include "von_bertalanffy.hpp"
#include "../common/fims_vector.hpp"

template <typename Type>
struct Population {
  fims::Vector<Type> length;
  fims::Vector<Type> ages;

  std::shared_ptr< VonBertalanffy<Type> > vb;

  Population() { 
    //this->vb = std::make_shared<VonBertalanffy<Type> >();
  }

  inline void CalculateGrowth(){
    length.resize(ages.size());
    for(int i =0; i < ages.size(); i++){
        length[i] = vb -> evaluate(ages[i]);
    }
  }

  void evaluate() {
    CalculateGrowth();
    Rcout << "length address inside population is: " << &length << std::endl;
  }

};

#endif