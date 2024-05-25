#ifndef NLL_BASE_HPP
#define NLL_BASE_HPP

#include "../common/def.hpp"
#include "../common/model_object.hpp"
#include "../common/fims_vector.hpp"
template <typename Type>
struct NLLBase : public ModelObject<Type> {
static uint32_t id_g; /**< The ID of the instance of the NLLBase class */

fims::Vector<Type> observed_value; 
fims::Vector<Type> expected_value;
fims::Vector<Type> nll_vec;
std::string nll_type;
bool simulate_flag;
bool osa_flag;
std::vector<std::string> key; 
#ifdef TMB_MODEL
::objective_function<Type>
      *of;  
#endif

  /** @brief Constructor.
   */
  NLLBase() : ModelObject<Type>() {
    // increment id of the singleton selectivity class
    this->id = NLLBase::id_g++;
  }

  virtual ~NLLBase() {}

  /**
   * @brief Calculates the NLL.
   */
  virtual const Type evaluate() = 0;
};

// default id of the singleton selectivity class
template <typename Type>
uint32_t NLLBase<Type>::id_g = 0;

#endif