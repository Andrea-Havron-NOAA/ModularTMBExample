#ifndef FIMS_COMMON_INFORMATION_HPP
#define FIMS_COMMON_INFORMATION_HPP

#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

#include "../nll/nll_base.hpp"
#include "../nll/normal_nll.hpp"
#include "../pop_dy/population.hpp"
#include "../pop_dy/von_bertalanffy.hpp"

template <typename Type>
class Information {
  public:
    static std::shared_ptr<Information<Type> >
        information;          /**< singleton instance >*/
    std::vector<Type*> parameters;
    std::map<uint32_t, std::shared_ptr<NLLBase<Type> > >
        nll_models;
    typedef typename std::map<
        uint32_t, std::shared_ptr<NLLBase<Type> > >::iterator
        nll_iterator;

    std::map<uint32_t, std::shared_ptr<Population<Type> > >
        pop_models;
    typedef typename std::map<
        uint32_t, std::shared_ptr<Population<Type> > >::iterator
        pop_iterator;

    std::map<uint32_t, std::shared_ptr<VonBertalanffy<Type> > >
        vb_models;
    typedef typename std::map<
        uint32_t, std::shared_ptr<VonBertalanffy<Type> > >::iterator
        vb_iterator;

    
    Information() {}

    virtual ~Information() {}



    //singleton instance based on Type
    static std::shared_ptr<Information<Type> > instance;
  
    /**
     * Returns a single Information object for type T.
     *
     * @return singleton for type T
     */
    static std::shared_ptr<Information<Type> > getInstance() {
        return Information<Type>::instance; //TODO: make sure not null and create if null
    }


   
  /*
    void RegisterParameter(Type& p) {
      this->parameters.push_back(&p);
    }
*/
  fims::Vector<Type> assign_variable(size_t id, std::string module_name, std::string name){
  /*  std::map<uint32_t, RcppInterfaceBase::interface_objects > 
        interface_modules;
    typedef typename std::map<
        uint32_t, RcppInterfaceBase::interface_objects >::iterator
        interface_iterator;

   interface_iterator it = interface_modules.find(module_id);
    std::shared_ptr<RcppInterfaceBase<Type> > ptr = (*it).second;
*/
    std::shared_ptr<VonBertalanffy<Type> > vb_ptr;
    std::shared_ptr<Population<Type> > pop_ptr;
    if(module_name == "growth"){
        vb_iterator it = vb_models.find(id);
        vb_ptr = (*it).second;
    }
    if(module_name == "population"){
        pop_iterator it = pop_models.find(id);
        pop_ptr = (*it).second;
    }
/*
    std::unordered_map<std::string, fims::Vector<Type>> variable_map = {
        {"length", pop_ptr->length},
        {"k", vb_ptr->k},
        {"l_inf", vb_ptr->l_inf}
      };
*/
      //return variable_map[name];
        return pop_ptr->length;

  }

    void setup_priors(){
      for(nll_iterator it = nll_models.begin(); it!= nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "prior"){
         n->observed_value = assign_variable(n->module_id, n->module_name, 
                                   n->member_name);
      }
    }
    }
//currently id, module_name, and name are members of information
//these should instead be members of individual likelihoods and n points to them
    void setup_data(){
      for(nll_iterator it = this->nll_models.begin(); it!= this->nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "data"){
        n->expected_value = assign_variable(n->module_id, n->module_name, 
                                  n->member_name); 
      }
    }
    }
};

template<typename Type>
std::shared_ptr<Information<Type> > Information<Type>::instance = std::make_shared<Information<Type> >();


#endif