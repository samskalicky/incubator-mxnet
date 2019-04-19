#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <functional>


#ifndef _MXNET_CUSTOM_OP_H_
#define _MXNET_CUSTOM_OP_H_

typedef void (*void_t)(void);
typedef void_t* void_ptr;
typedef void (*fcomp_t)(char const**, char const**, int);
typedef int (*parseAttrs_t)(char const**, char const**, int);

typedef int (*get_size_t)(void);
typedef void (*get_op_t)(int, char**, fcomp_t*, parseAttrs_t*);

class CustomOp {
 public:
  CustomOp& setFCompute(fcomp_t fcomp) {
    fcompute = fcomp;
    return *this;
  }
  CustomOp& setParseAttrs(parseAttrs_t func) {
    parse_attrs = func;
    return *this;
  }
  parseAttrs_t getParseAttrs() {
    return parse_attrs;
  }
  fcomp_t getFCompute() {
    return fcompute;
  }
  const char* getName() {
    return name;
  }
 CustomOp(const char* op_name) : name(op_name) {
    fcompute = nullptr;
    parse_attrs = nullptr;
  }
  ~CustomOp() {}
 private:
  parseAttrs_t parse_attrs;
  fcomp_t fcompute;
  const char* name;
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
    names.push_back(std::string(name));
    return *op;
  }
  int size() {
    return entries.size();
  }
  void getOp(int idx, const char** name,
             fcomp_t* func, parseAttrs_t* parse) {
    std::string op_name = names[idx];
    CustomOp* op = entries[op_name];
    *name = op->getName();
    *func = op->getFCompute();
    *parse = op->getParseAttrs();
  }
  bool hasOp(const std::string name) {
    return entries.find(name) != entries.end();
  }
  CustomOp* op(const std::string name) {
    return op(name.c_str());
  }
  CustomOp* op(const char* name) {
    return entries[name];
  }

 private:
  std::map<std::string,CustomOp*> entries;
  std::vector<std::string> names;
  /*! \brief constructor */
  OpRegistry() {}
  /*! \brief destructor */
  ~OpRegistry() {}
};

extern "C" {
#ifndef MXNET_CUSTOM_OP
  static
#endif
  int _opRegSize() {
    return OpRegistry::get()->size();
  }

#ifndef MXNET_CUSTOM_OP
  static
#endif
  void _opRegGet(int idx, const char** name, fcomp_t* func, parseAttrs_t* parse) {
    OpRegistry::get()->getOp(idx,name,func,parse);
  }
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
