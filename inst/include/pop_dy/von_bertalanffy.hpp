#ifndef VONBERTALANFFY_HPP
#define  VONBERTALANFFY_HPP

#include <vector>
#include "../common/def.hpp"
#include "../common/model_object.hpp"

//#include "../interface/interface.hpp"

/**
 * Portable von Bertalanffy model. Can be used in multiple
 * modeling platforms.
 */
template<typename Type>
class VonBertalanffy : public ModelObject<Type> {
  static uint32_t id_g; /**< The ID of the instance of the Population class */
  public:

//Changed from type to fims::Vector for std::unordered_map to work
// under this design, everything needs to be a fims::Vector, scalars are a vector size of 1
  fims::Vector<Type> logk;
  fims::Vector<Type> l_inf;
  fims::Vector<Type> a_min;
  fims::Vector<Type> alpha;
  fims::Vector<Type> beta;
  
  
  VonBertalanffy(){
    this->id = VonBertalanffy::id_g++;
  }
  
  /**
   * Objective function to compute least squares
   * of observed and predicted length.
   */
  Type evaluate(const Type& age){
    Type ret = this->l_inf[0] * (1.0 - exp(-exp(logk[0]) * (age - this->a_min[0])));
    return ret;
  }
  
  
};


template <class Type>
uint32_t VonBertalanffy<Type>::id_g = 0;

#endif
