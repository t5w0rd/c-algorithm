//
// Created by t5w0rd on 19-4-20.
//

#include "tinterpreter.h"
#include "tnative.h"
#include "tmisc.h"
#include "theap.h"


typedef enum {
    FOPEN_ARGUMENT_TYPE_ERR = 0,
    FCLOSE_ARGUMENT_TYPE_ERR,
    FGETS_ARGUMENT_TYPE_ERR,
    FILE_ALREADY_CLOSED_ERR,
    FPUTS_ARGUMENT_TYPE_ERR,
    NEW_ARRAY_ARGUMENT_TYPE_ERR,
    NEW_ARRAY_ARGUMENT_TOO_FEW_ERR,
    EXIT_ARGUMENT_TYPE_ERR,
    NEW_EXCEPTION_ARGUMENT_ERR,
    FGETS_BAD_MULTIBYTE_CHARACTER_ERR
} NativeErrorCode;

extern CRB_ErrorDefinition crb_native_error_message_format[];

static CRB_NativeLibInfo st_lib_info = {
        crb_native_error_message_format,
};

// functions
static CRB_Value nv_print_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment* env, int arg_count, CRB_Value *args){
    CRB_Value value;
    value.type = CRB_NULL_VALUE;

    CRB_check_argument_count(interpreter, env, arg_count, 1);

    CRB_Char* str = CRB_value_to_string(interpreter, env, __LINE__, &args[0]);
    CRB_print_wcs(stdout, str);
    MEM_free(str);

    return value;
}

static CRB_Value nv_fopen_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment* env, int arg_count, CRB_Value *args){
    CRB_Value value = {};
    return value;
}

static CRB_Value nv_fclose_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment* env, int arg_count, CRB_Value *args){
    CRB_Value value = {};
    return value;
}

static CRB_Value nv_fgets_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment* env, int arg_count, CRB_Value *args){
    CRB_Value value = {};
    return value;
}

static CRB_Value nv_fputs_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment* env, int arg_count, CRB_Value *args){
    CRB_Value value = {};
    return value;
}

static CRB_Value nv_new_array_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment* env, int arg_count, CRB_Value *args){
    CRB_Value value = {};
    return value;
}

static CRB_Value
nv_new_object_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment *env, int arg_count, CRB_Value *args) {
    CRB_check_argument_count(interpreter, env, arg_count, 0);
    CRB_Value value;
    value.type = CRB_ASSOC_VALUE;
    value.u.object = CRB_create_assoc(interpreter, env);
    return value;
}

static CRB_Value nv_new_exception_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment *env, int arg_count, CRB_Value *args) {
    CRB_check_argument_count(interpreter, env, arg_count, 1);
    if (args[0].type != CRB_STRING_VALUE) {
        CRB_error(interpreter, env, &st_lib_info, __LINE__, (int)NEW_EXCEPTION_ARGUMENT_ERR, CRB_STRING_MESSAGE_ARGUMENT, "type", CRB_get_type_name(args[0].type), CRB_MESSAGE_ARGUMENT_END);
    }
    CRB_Value value;
    value.type = CRB_ASSOC_VALUE;
    value.u.object = CRB_create_exception(interpreter, env->next, args[0].u.object, env->caller_line_number);
    return value;
}

static CRB_Value nv_exit_proc(CRB_Interpreter *interpreter, CRB_LocalEnvironment *env, int arg_count, CRB_Value *args) {
    CRB_check_argument_count(interpreter, env, arg_count, 1);
    if (args[0].type != CRB_INT_VALUE) {
        CRB_error(interpreter, env, &st_lib_info, __LINE__, (int)EXIT_ARGUMENT_TYPE_ERR, CRB_STRING_MESSAGE_ARGUMENT, "type", CRB_get_type_name(args[0].type), CRB_MESSAGE_ARGUMENT_END);
    }

    exit(args[0].u.int_value);
    CRB_Value value = {};
    return value;
}

void crb_add_native_functions(CRB_Interpreter *inter) {
    CRB_add_native_function(inter, "print", nv_print_proc);
    CRB_add_native_function(inter, "fopen", nv_fopen_proc);
    CRB_add_native_function(inter, "fclose", nv_fclose_proc);
    CRB_add_native_function(inter, "fgets", nv_fgets_proc);
    CRB_add_native_function(inter, "fputs", nv_fputs_proc);
    CRB_add_native_function(inter, "new_array", nv_new_array_proc);
    CRB_add_native_function(inter, "new_object", nv_new_object_proc);
    CRB_add_native_function(inter, "new_exception", nv_new_exception_proc);
    CRB_add_native_function(inter, "exit", nv_exit_proc);
}


// file pointers
static void file_finalizer(CRB_Interpreter *inter, CRB_Object *obj) {
    FILE* fp = (FILE*)CRB_object_get_native_pointer(obj);
    if (fp && fp != stdin && fp != stdout && fp != stderr) {
        fclose(fp);
    }
}

static CRB_NativePointerInfo st_file_type_info = {
        "tlang.lang.file",
        file_finalizer
};

void crb_add_std_fp(CRB_Interpreter *inter) {
    CRB_Value fp_value;

    fp_value.type = CRB_NATIVE_POINTER_VALUE;
    fp_value.u.object = crb_create_native_pointer_i(inter, stdin, &st_file_type_info);
    CRB_add_global_variable(inter, "STDIN", &fp_value, CRB_TRUE);

    fp_value.u.object = crb_create_native_pointer_i(inter, stdout, &st_file_type_info);
    CRB_add_global_variable(inter, "STDOUT", &fp_value, CRB_TRUE);

    fp_value.u.object = crb_create_native_pointer_i(inter, stderr, &st_file_type_info);
    CRB_add_global_variable(inter, "STDERR", &fp_value, CRB_TRUE);
}

void CRB_array_set(CRB_Interpreter *inter, CRB_LocalEnvironment *env, CRB_Object *obj, int index, CRB_Value *value) {
    DBG_assert(obj->type == ARRAY_OBJECT, ("obj->type..%d\n", obj->type));
    CRB_Value_array_set(obj->u.array.array, index, *value);
//    obj->u.array.array[index] = *value;
}

void CRB_set_function_definition(const char *name, CRB_NativeFunctionProc *proc, CRB_FunctionDefinition *fd) {
    fd->name = name;
    fd->type = CRB_NATIVE_FUNCTION_DEFINE;
    fd->is_closure = CRB_TRUE;
    fd->u.native_f.proc = proc;
}
