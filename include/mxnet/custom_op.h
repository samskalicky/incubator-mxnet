#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <functional>

#include <stdint.h>

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
  MXTensor() { data = nullptr; }
  MXTensor(void *data, const std::vector<int64_t> &shape, MXDType dtype)
  : data{data}, shape{shape}, dtype{dtype} {}

  template<typename data_type>
  data_type* getData() {
    return (data_type*)data;
  }
  
  void *data; // not owned
  std::vector<int64_t> shape;
  MXDType dtype;
};

//User function templates
typedef int (*fcomp_t)(std::map<std::string,std::string>,
                        std::vector<MXTensor>, std::vector<MXTensor>);                        
typedef int (*parseAttrs_t)(std::map<std::string,std::string>,
                            int*, int*);
typedef int (*inferType_t)(std::map<std::string,std::string>,
                           std::vector<int>&, std::vector<int>&);
typedef int (*inferShape_t)(std::map<std::string,std::string>,
                            std::vector<std::vector<unsigned int>>&,
                            std::vector<std::vector<unsigned int>>&);

//internal function templates
typedef void (*void_t)(void);
typedef void_t* void_ptr;
typedef void* (*mxAlloc_t)(void*, unsigned);
typedef int (*get_size_t)(void);
typedef void (*get_op_t)(int, char**, fcomp_t*, parseAttrs_t*, inferType_t*,
                         inferShape_t*);
typedef int (*call_fcomp)(const char*, const char* const*, const char* const*, int,
                           const int64_t**, int*, void**, int*, int,
                           const int64_t**, int*, void**, int*, int);
typedef int (*call_parseAttrs)(const char*, const char* const*, const char* const*, int,
                                int*, int*);
typedef int (*call_inferType)(const char*, const char* const*, const char* const*, int,
                               int*, int, int*, int);
typedef int (*call_inferShape)(mxAlloc_t, void*,
                                const char*, const char* const*, const char* const*, int,
                                unsigned int**, int*, int, unsigned int***, int**, int);

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
  void release(void* ptr) {
    auto it = std::find(allocations.begin(),allocations.end(),ptr);
    if(it != allocations.end())
      allocations.erase(it);
    free(ptr);
  }
  void freeAll() {
    for(auto ptr : allocations) {
      free(ptr);
    }
    allocations.clear();
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

  call_parseAttrs _callParseAttrs;
  call_inferType _callInferType;
  call_inferShape _callInferShape;
  call_fcomp _callFCompute;
  
 private:
  std::map<std::string,CustomOp*> entries;
  std::vector<std::string> names;
  /*! \brief constructor */
  OpRegistry() {
    _callParseAttrs = nullptr;
    _callInferType = nullptr;
    _callInferShape = nullptr;
    _callFCompute = nullptr;
  
  }
  /*! \brief destructor */
  ~OpRegistry() {}
};

extern "C" {
#ifndef MXNET_CUSTOM_OP
  static inline
#endif
  int _opRegSize() {
    return OpRegistry::get()->size();
  }

#ifndef MXNET_CUSTOM_OP
  static inline
#endif
  void _opRegGet(int idx, const char** name,
                 fcomp_t* func, parseAttrs_t* parse, inferType_t* type,
                 inferShape_t* shape) {
    OpRegistry::get()->getOp(idx,name,func,parse,type,shape);
  }

#ifndef MXNET_CUSTOM_OP
  static inline
#endif
  int _opCallFCompute(const char* name, const char* const* keys, const char* const* vals, int num,
                       const int64_t** inshapes, int* indims, void** indata, int* intypes, int num_in,
                       const int64_t** outshapes, int* outdims, void** outdata, int* outtypes, int num_out) {
    CustomOp* op = OpRegistry::get()->op(name);

    //create map of attributes from list
    std::map<std::string,std::string> attrs;
    for(int i=0; i<num; i++) {
      attrs[std::string(keys[i])] = std::string(vals[i]);
    }

    //create a vector of tensors for inputs
    std::vector<MXTensor> inputs(num_in);
    for(int i=0; i<num_in; i++) {
      inputs[i].data = indata[i];
      inputs[i].dtype = (MXDType)intypes[i];
      for(int j=0; j<indims[i]; j++) {
        inputs[i].shape.push_back(inshapes[i][j]);
      }
    }

    //create a vector of tensors for outputs
    std::vector<MXTensor> outputs(num_out);
    for(int i=0; i<num_out; i++) {
      outputs[i].data = outdata[i];
      outputs[i].dtype = (MXDType)outtypes[i];
      for(int j=0; j<outdims[i]; j++) {
        outputs[i].shape.push_back(outshapes[i][j]);
      }
    }

    return op->getFCompute()(attrs,inputs,outputs);
  }


#ifndef MXNET_CUSTOM_OP
  static inline
#endif
  int _opCallParseAttrs(const char* name, const char* const* keys, const char* const* vals, int num,
                         int* num_in, int* num_out) {
    CustomOp* op = OpRegistry::get()->op(name);

    //create map of attributes from list
    std::map<std::string,std::string> attrs;
    for(int i=0; i<num; i++) {
      attrs[std::string(keys[i])] = std::string(vals[i]);
    }

    return op->getParseAttrs()(attrs,num_in,num_out);
  }

#ifndef MXNET_CUSTOM_OP
  static inline
#endif
  int _opCallInferType(const char* name, const char* const* keys, const char* const* vals, int num,
                        int* intypes, int num_in, int* outtypes, int num_out) {
    CustomOp* op = OpRegistry::get()->op(name);

    //create map of attributes from list
    std::map<std::string,std::string> attrs;
    for(int i=0; i<num; i++) {
      attrs[std::string(keys[i])] = std::string(vals[i]);
    }

    //create vector of types, copy input types, output types are empty
    std::vector<int> in_types(num_in);
    std::vector<int> out_types(num_out);
    for(int i=0; i<num_in; i++) {
      in_types[i] = intypes[i];
    }

    int retval = op->getInferType()(attrs,in_types,out_types);
    if(!retval) return retval;

    //copy output types
    for(int i=0; i<num_out; i++) {
      outtypes[i] = out_types[i];
    }

    return retval;
  }

#ifndef MXNET_CUSTOM_OP
  static inline
#endif
  int _opCallInferShape(mxAlloc_t mx_alloc, void* ptr,
                         const char* name, const char* const* keys, const char* const* vals, int num,
                         unsigned int** inshapes, int* indims, int num_in,
                         unsigned int*** outshapes, int** outdims, int num_out) {
    CustomOp* op = OpRegistry::get()->op(name);

    //create map of attributes from list
    std::map<std::string,std::string> attrs;
    for(int i=0; i<num; i++) {
      attrs[std::string(keys[i])] = std::string(vals[i]);
    }

    //create a vector of shapes for inputs
    std::vector<std::vector<unsigned int> > in_shapes(num_in);
    for(int i=0; i<num_in; i++) {
      for(int j=0; j<indims[i]; j++) {
        in_shapes[i].push_back(inshapes[i][j]);
      }
    }

    //create a vector of shapes for outputs
    std::vector<std::vector<unsigned int> > out_shapes(num_out);

    int retval = op->getInferShape()(attrs,in_shapes,out_shapes);
    if(!retval) return retval;

    //allocate space for output dims, shape
    *outdims = (int*)mx_alloc(ptr,num_out*sizeof(int));
    *outshapes = (unsigned**)mx_alloc(ptr,num_out*sizeof(unsigned*));

    //copy output shapes
    for(int i=0; i<num_out; i++) {
      (*outdims)[i] = out_shapes[i].size();
      (*outshapes)[i] = (unsigned*)mx_alloc(ptr,(*outdims)[i]*sizeof(unsigned));
      for(int j=0; j<indims[i]; j++) {
        (*outshapes)[i][j] = out_shapes[i][j];
      }
    }

    return retval;
  }
  
} //end extern "C"
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
