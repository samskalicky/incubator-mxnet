#include <iostream>
#include <map>
#include <string>
#include <functional>


#ifndef _MXNET_CUSTOM_OP_H_
#define _MXNET_CUSTOM_OP_H_

using FCompute = std::function<void ()>;

class CustomOp {
 public:
  CustomOp& setFCompute(FCompute fcomp) {
    return *this;
  }
  CustomOp(const char* name) {}
  ~CustomOp() {}
};


class OpRegistry {
 public:
  static OpRegistry* get() {
    static OpRegistry inst;
    return &inst;
  }
  CustomOp& add(const char* name) {
    CustomOp *op = new CustomOp(name);
    entries[std::string(name)]=op;
    return *op;
  }
  void list() {
    std::map<std::string, CustomOp*>::const_iterator p;
    for (p = entries.begin(); p !=entries.end(); ++p) {
      std::cout << p->first << std::endl;
    }
  }
 private:
  std::map<std::string,CustomOp*> entries;
  /*! \brief constructor */
  OpRegistry() {}
  /*! \brief destructor */
  ~OpRegistry() {}
};

extern "C" OpRegistry* GlobalGet() {
  return OpRegistry::get();
}

/*********************************************************************************/
/*********************************************************************************/
//Macro to help with string concat
/* Annoyingly, the concat_ and concat macros are necessary to
 * be able to use __COUNTER__ in an identifier name */
#define _STR_CONCAT_(__a, __b) __a ## __b
#define _STR_CONCAT(__a, __b) _STR_CONCAT_(__a, __b)

//convert a token to a string
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

//declare a variable with custom name
#define _REGISTER_NAME_(Name) MXNet ## _CustomOp ## _
#define _REGISTER_DEF_(Name) CustomOp _REGISTER_NAME_(Name)

//assign a var to a value
#define REGISTER_OP(Name) _STR_CONCAT(_REGISTER_DEF_(Name), __COUNTER__) = OpRegistry::get()->add(TOSTRING(Name))

#endif
