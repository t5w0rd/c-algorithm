//
// Created by t5w0rd on 19-4-19.
//

#ifndef TLANG_TLANG_H
#define TLANG_TLANG_H

#include <stdio.h>
#include <wchar.h>


// tinterpreter
typedef struct CRB_Interpreter_tag CRB_Interpreter;

CRB_Interpreter *CRB_create_interpreter(void);
void CRB_compile(CRB_Interpreter* interpreter, FILE *fp);
void CRB_compile_string(CRB_Interpreter* interpreter, const char** lines);
void CRB_set_command_line_args(CRB_Interpreter* interpreter, int argc, char** argv);
void CRB_interpret(CRB_Interpreter* interpreter);
void CRB_dispose_interpreter(CRB_Interpreter* interpreter);


// development
typedef wchar_t CRB_Char;

typedef enum {
    CRB_FALSE = 0,
    CRB_TRUE = 1
} CRB_Boolean;

typedef enum {
    CRB_BOOLEAN_VALUE = 1,
    CRB_INT_VALUE,
    CRB_DOUBLE_VALUE,
    CRB_STRING_VALUE,
    CRB_NATIVE_POINTER_VALUE,
    CRB_NULL_VALUE,
    CRB_ARRAY_VALUE,
    CRB_ASSOC_VALUE,
    CRB_CLOSURE_VALUE,
    CRB_FAKE_METHOD_VALUE,
    CRB_SCOPE_CHAIN_VALUE
} CRB_ValueType;

typedef struct CRB_Object_tag CRB_Object;
typedef struct CRB_Array_tag CRB_Array;
typedef struct CRB_String_tag CRB_String;
typedef struct CRB_Assoc_tag CRB_Assoc;
typedef struct CRB_ParameterList_tag CRB_ParameterList;
typedef struct CRB_Block_tag CRB_Block;
typedef struct CRB_FunctionDefinition_tag CRB_FunctionDefinition;
typedef struct CRB_LocalEnvironment_tag CRB_LocalEnvironment;


typedef struct {
    CRB_FunctionDefinition *function;
    CRB_Object          *environment; /* CRB_ScopeChain */
} CRB_Closure;

typedef struct {
    char        *method_name;
    CRB_Object  *object;
} CRB_FakeMethod;

typedef struct {
    CRB_ValueType       type;
    union {
        CRB_Boolean     boolean_value;
        int             int_value;
        double          double_value;
        CRB_Object      *object;
        CRB_Closure     closure;
        CRB_FakeMethod  fake_method;
    } u;
} CRB_Value;

typedef enum {
    CRB_CROWBAR_FUNCTION_DEFINITION = 1,
    CRB_NATIVE_FUNCTION_DEFINITION,
    CRB_FUNCTION_DEFINITION_TYPE_COUNT_PLUS_1
} CRB_FunctionDefinitionType;

typedef CRB_Value CRB_NativeFunctionProc(CRB_Interpreter *interpreter, CRB_LocalEnvironment *env, int arg_count, CRB_Value *args);  // TODO: *

struct CRB_FunctionDefinition_tag {
    char                *name;
    CRB_FunctionDefinitionType  type;
    CRB_Boolean         is_closure;
    union {
        struct {
            CRB_ParameterList   *parameter;
            CRB_Block           *block;
        } crowbar_f;
        struct {
            CRB_NativeFunctionProc      *proc;
        } native_f;
    } u;
    struct CRB_FunctionDefinition_tag   *next;
};

CRB_FunctionDefinition* CRB_add_native_function(CRB_Interpreter *interpreter, char *name, CRB_NativeFunctionProc *proc);


typedef void CRB_NativePointerFinalizeProc(CRB_Interpreter *inter, CRB_Object *obj);  // TODO: *
typedef struct {
    char                                *name;
    CRB_NativePointerFinalizeProc       *finalizer;
} CRB_NativePointerInfo;

typedef enum {
    CRB_INT_MESSAGE_ARGUMENT = 1,
    CRB_DOUBLE_MESSAGE_ARGUMENT,
    CRB_STRING_MESSAGE_ARGUMENT,
    CRB_CHARACTER_MESSAGE_ARGUMENT,
    CRB_POINTER_MESSAGE_ARGUMENT,
    CRB_MESSAGE_ARGUMENT_END
} CRB_MessageArgumentType;


// theap
CRB_Value* CRB_search_assoc_member(CRB_Object *assoc, char *member_name);

// teval
CRB_Value CRB_call_function(CRB_Interpreter *inter, CRB_LocalEnvironment *env, int line_number, CRB_Value *func, int arg_count, CRB_Value *args);

// tmisc
CRB_Value* CRB_add_global_variable(CRB_Interpreter *inter, char *identifier, CRB_Value *value, CRB_Boolean is_final);
void* CRB_object_get_native_pointer(CRB_Object *obj);








// memory
typedef enum {
    MEM_FAIL_AND_EXIT,
    MEM_FAIL_AND_RETURN
} MEM_FailMode;

typedef struct MEM_Controller_tag *MEM_Controller;
typedef void (*MEM_ErrorHandler)(MEM_Controller, char *, int, char *);
typedef struct MEM_Storage_tag *MEM_Storage;

extern MEM_Controller mem_default_controller;

#ifdef MEM_CONTROLLER
#define MEM_CURRENT_CONTROLLER MEM_CONTROLLER
#else /* MEM_CONTROLLER */
#define MEM_CURRENT_CONTROLLER mem_default_controller
#endif /* MEM_CONTROLLER */

/*
 * Don't use mem_*_func function.
 * There are private functions of MEM module.
 */
MEM_Controller MEM_create_controller(void);
void *MEM_malloc_func(MEM_Controller controller,
                      char *filename, int line, size_t size);
void *MEM_realloc_func(MEM_Controller controller,
                       char *filename, int line, void *ptr, size_t size);
char *MEM_strdup_func(MEM_Controller controller,
                      char *filename, int line, char *str);
MEM_Storage MEM_open_storage_func(MEM_Controller controller,
                                  char *filename, int line, int page_size);
void *MEM_storage_malloc_func(MEM_Controller controller,
                              char *filename, int line,
                              MEM_Storage storage, size_t size);
void MEM_free_func(MEM_Controller controller, void *ptr);
void MEM_dispose_storage_func(MEM_Controller controller,
                              MEM_Storage storage);

void MEM_set_error_handler(MEM_Controller controller,
                           MEM_ErrorHandler handler);
void MEM_set_fail_mode(MEM_Controller controller,
                       MEM_FailMode mode);
void MEM_dump_blocks_func(MEM_Controller controller, FILE *fp);
void MEM_check_block_func(MEM_Controller controller,
                          char *filename, int line, void *p);
void MEM_check_all_blocks_func(MEM_Controller controller,
                               char *filename, int line);

#define MEM_malloc(size)\
  (MEM_malloc_func(MEM_CURRENT_CONTROLLER,\
                   __FILE__, __LINE__, size))
#define MEM_realloc(ptr, size)\
  (MEM_realloc_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, ptr, size))
#define MEM_strdup(str)\
  (MEM_strdup_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, str))
#define MEM_open_storage(page_size)\
  (MEM_open_storage_func(MEM_CURRENT_CONTROLLER,\
                         __FILE__, __LINE__, page_size))
#define MEM_storage_malloc(storage, size)\
  (MEM_storage_malloc_func(MEM_CURRENT_CONTROLLER,\
                           __FILE__, __LINE__, storage, size))
#define MEM_free(ptr) (MEM_free_func(MEM_CURRENT_CONTROLLER, ptr))
#define MEM_dispose_storage(storage)\
  (MEM_dispose_storage_func(MEM_CURRENT_CONTROLLER, storage))
#ifdef DEBUG
#define MEM_dump_blocks(fp)\
  (MEM_dump_blocks_func(MEM_CURRENT_CONTROLLER, fp))
#define MEM_check_block(p)\
  (MEM_check_block_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__, p))
#define MEM_check_all_blocks()\
  (MEM_check_all_blocks_func(MEM_CURRENT_CONTROLLER, __FILE__, __LINE__))
#else /* DEBUG */
#define MEM_dump_blocks(fp) ((void)0)
#define MEM_check_block(p)  ((void)0)
#define MEM_check_all_blocks() ((void)0)
#endif /* DEBUG */

// debug
typedef struct DBG_Controller_tag *DBG_Controller;
void DBG_set(DBG_Controller controller, char *file, int line);
void DBG_set_expression(char *expression);

#ifdef DBG_NO_DEBUG
#define DBG_create_controller()         ((void)0)
#define DBG_set_debug_level(level)      ((void)0)
#define DBG_set_debug_write_fp(fp)      ((void)0)
#define DBG_assert(expression, arg)     ((void)0)
#define DBG_panic(arg)                  ((void)0)
#define DBG_debug_write(arg)            ((void)0)
#else /* DBG_NO_DEBUG */
#ifdef DBG_CONTROLLER
#define DBG_CURRENT_CONTROLLER  DBG_CONTROLLER
#else /* DBG_CONTROLLER */
#define DBG_CURRENT_CONTROLLER  dbg_default_controller
#endif /* DBG_CONTROLLER */
extern DBG_Controller DBG_CURRENT_CONTROLLER;

#define DBG_create_controller() (DBG_create_controller_func())
#define DBG_set_debug_level(level) \
(DBG_set_debug_level_func(DBG_CURRENT_CONTROLLER, (level)))
#define DBG_set_debug_write_fp(fp) \
(DBG_set_debug_write_fp(DBG_CURRENT_CONTROLLER, (fp))
#define DBG_assert(expression, arg) \
 ((expression) ? (void)(0) :\
  ((DBG_set(DBG_CURRENT_CONTROLLER, __FILE__, __LINE__)),\
   (DBG_set_expression(#expression)),\
   DBG_assert_func arg))
#define DBG_panic(arg) \
 ((DBG_set(DBG_CURRENT_CONTROLLER, __FILE__, __LINE__)),\
  DBG_panic_func arg)
#define DBG_debug_write(arg) \
 ((DBG_set(DBG_CURRENT_CONTROLLER, __FILE__, __LINE__)),\
  DBG_debug_write_func arg)
#endif /* DBG_NO_DEBUG */

typedef enum {
    DBG_TRUE = 1,
    DBG_FALSE = 0
} DBG_Boolean;

DBG_Controller DBG_create_controller_func(void);
void DBG_set_debug_level_func(DBG_Controller controller, int level);
void DBG_set_debug_write_fp_func(DBG_Controller controller, FILE *fp);
void DBG_assert_func(char *fmt, ...);
void DBG_panic_func(char *fmt, ...);
void DBG_debug_write_func(int level, char *fmt, ...);

#endif //TLANG_TLANG_H