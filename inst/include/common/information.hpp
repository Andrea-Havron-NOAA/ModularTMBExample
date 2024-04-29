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

    std::unordered_map<std::string, fims::Vector<Type>* > variable_map;
    typedef typename std::unordered_map<std::string, fims::Vector<Type>* >::iterator 
      variable_map_iterator;

    
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


    void setup_population(){
      for (pop_iterator it = this->pop_models.begin();
         it != this->pop_models.end(); ++it) {
        std::shared_ptr<Population<Type> > pop = (*it).second;
        uint32_t growth_uint = static_cast<uint32_t>(pop->growth_id);
        vb_iterator vbit = this->vb_models.find(growth_uint);  
        pop->vb = (*vbit).second; 
      }
    }
    
    void setup_priors(){
      for(nll_iterator it = nll_models.begin(); it!= nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "prior"){
        size_t key_size = n->key.size();
        variable_map_iterator vmit;
        for(size_t i=0; i<key_size; i++){
          vmit = this->variable_map.find(n->key[i]); 
          n->observed_value.insert(std::end(n->observed_value), 
            std::begin(*(*vmit).second), std::end(*(*vmit).second));
        } 
      }
    }
    }
    void setup_data(){
      for(nll_iterator it = this->nll_models.begin(); it!= this->nll_models.end(); ++it){
      std::shared_ptr<NLLBase<Type> > n = (*it).second;
      if(n->nll_type == "data"){
        variable_map_iterator vmit;
        vmit = this->variable_map.find(n->key[0]); 
        n->expected_value = *(*vmit).second;
        
        for(size_t i=1; i<n->key.size(); i++){
          vmit = this->variable_map.find(n->key[i]); 
          n->expected_value.insert(std::end(n->expected_value), 
            std::begin(*(*vmit).second), std::end(*(*vmit).second));
        } 
      }
    }
  }
};

template<typename Type>
std::shared_ptr<Information<Type> > Information<Type>::instance = std::make_shared<Information<Type> >();


#endif