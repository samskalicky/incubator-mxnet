#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <functional>


#ifndef _MXNET_CUSTOM_OP_H_
#define _MXNET_CUSTOM_OP_H_

enum MXDType {
  kFloat32 = 0,
  kFloat64 = 1,
  kFloat16 = 2,
  kUint8 = 3,
  kInt32 = 4,
  kInt8  = 5,
  kInt64 = 6,
};

struct MXTensor {
  MXTensor() { data = nullptr; };
MXTensor(void *data, const std::vector<int64_t> &shape, MXDType dtype)
: data{data}, shape{shape}, dtype{dtype} {}
  
  void *data; // not owned
  std::vector<int64_t> shape;
  MXDType dtype;
};

typedef void (*void_t)(void);
typedef void_t* void_ptr;

typedef void* (*mxAlloc_t)(void*, unsigned);

typedef void (*fcomp_t)(const char* const*, const char* const*, int,
                        const long int**, int*, void**, int*,
                        const long int**, int*, void**, int*);
                        
typedef int (*parseAttrs_t)(const char* const*, const char* const*, int,
                            int*, int*);
typedef int (*inferType_t)(const char* const*, const char* const*, int,
                            int*, int*);
typedef int (*inferShape_t)(mxAlloc_t, void*,
                            const char* const*, const char* const*, int,
                            unsigned int**, int*, unsigned int***, int**);

typedef int (*get_size_t)(void);
typedef void (*get_op_t)(int, char**, fcomp_t*, parseAttrs_t*, inferType_t*,
                         inferShape_t*);

class CustomOp {
 public:
  static void* CallAllocator(CustomOp* op, unsigned size) {
    void* ptr = op->allocator(size);
    return ptr;
  }
  
  void* allocator(unsigned size) {
    void* ptr = malloc(size);
    allocations.push_back(ptr);
    return ptr;
  }
  void freeAll() {
    for(auto ptr : allocations) {
      free(ptr);
    }
  }

  
  CustomOp& setFCompute(fcomp_t fcomp) {
    fcompute = fcomp;
    return *this;
  }
  CustomOp& setParseAttrs(parseAttrs_t func) {
    parse_attrs = func;
    return *this;
  }
  CustomOp& setInferType(inferType_t func) {
    infer_type = func;
    return *this;
  }
  CustomOp& setInferShape(inferShape_t func) {
    infer_shape = func;
    return *this;
  }
  inferShape_t getInferShape() {
    return infer_shape;
  }
  inferType_t getInferType() {
    return infer_type;
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
    infer_type = nullptr;
    infer_shape = nullptr;
  }
  ~CustomOp() {}
 private:
  std::vector<void*> allocations;
  inferShape_t infer_shape;
  inferType_t infer_type;
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
             fcomp_t* func, parseAttrs_t* parse, inferType_t *type,
             inferShape_t* shape) {
    std::string op_name = names[idx];
    CustomOp* op = entries[op_name];
    *name = op->getName();
    *func = op->getFCompute();
    *parse = op->getParseAttrs();
    *type = op->getInferType();
    *shape = op->getInferShape();
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
  void _opRegGet(int idx, const char** name,
                 fcomp_t* func, parseAttrs_t* parse, inferType_t* type,
                 inferShape_t* shape) {
    OpRegistry::get()->getOp(idx,name,func,parse,type,shape);
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
