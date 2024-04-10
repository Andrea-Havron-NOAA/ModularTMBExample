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

    std::unordered_map<std::string, fims::Vector<Type> > variable_map;

    
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
  fims::Vector<Type> assign_variable(std::string key){
  //fims::Vector<Type> assign_variable(size_t id, std::string module_name, std::string name){
//    
// 
//    std::shared_ptr<VonBertalanffy<Type> > vb_ptr;
//    std::shared_ptr<Population<Type> > pop_ptr;
//    if(module_name == "growth"){
//        vb_iterator it = vb_models.find(id);
//        vb_ptr = (*it).second;
//        variable_map["k"] = vb_ptr->l_inf;
//        variable_map["l_inf"] = vb_ptr->l_inf;
//    }
//    if(module_name == "population"){
//        pop_iterator it = pop_models.find(id);
//        pop_ptr = (*it).second;
//        variable_map["length"] = pop_ptr->length;
//    }
     return variable_map[key];
//    
//
  }
//
    void setup_priors(){
      for(nll_iterator it = nll_models.begin(); it!= nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "prior"){
         //n->observed_value = assign_variable(n->module_id, n->module_name, 
         //                          n->member_name);
         n->observed_value = assign_variable(n->key);
      }
    }
    }
    void setup_data(){
      for(nll_iterator it = this->nll_models.begin(); it!= this->nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "data"){
//        n->expected_value = assign_variable(n->module_id, n->module_name, 
//                                  n->member_name); 
         n->expected_value = assign_variable(n->key);
         Rcout << "length is: " << assign_variable(n->key)[0] << std::endl;
      }
    }
    }
};

template<typename Type>
std::shared_ptr<Information<Type> > Information<Type>::instance = std::make_shared<Information<Type> >();


#endif