//
// Created by t5w0rd on 19-4-21.
//

#include "tobject.h"
#include "tinterpreter.h"


CRB_ErrorDefinition crb_compile_error_message_format[] = {
        {"dummy", NULL},
        {"在($(token))附近发生语法错误", NULL},
        {"不正确的字符($(bad_char))", NULL},
        {"函数名重复($(name))", NULL},
        {"不正确的多字节字符。", NULL},
        {"正则表达式中不能含有换行。", NULL},
        {"不能生成正则表达式。$(message)", NULL},
        {"预期外的宽字符串。", NULL},
        {"数组的不能含有final元素。", NULL},
        {"复合运算符不能用于final值。", NULL},
        {"dummy", NULL}
};

CRB_ErrorDefinition crb_runtime_error_message_format[] = {
        {"dummy", NULL},
        {"找不到变量或函数($(name))。",
                "VariableNotFoundException"},
        {"传入的参数数量多于函数定义。",
                "ArgumentTooManyException"},
        {"传入的参数数量少于函数定义。",
                "ArgumentTooFewException"},
        {"条件表达式的值必须是boolean型。",
                "NotBooleanException"},
        {"减法运算的操作数必须是数值类型。",
                "MinusOperandTypeException"},
        {"双目操作符$(operator)的操作数类型不正确。",
                "BadOperandTypeException"},
        {"&&, ||运算符不能接受实数型的操作数。",
                "LogicalOperatorFloatOperandException"},
        {"&&, ||运算符不能接受整数型的操作数。",
                "LogicalOperatorIntegerOperandException"},
        {"$(operator)运算符不能用于boolean型。",
                "NotBooleanOperatorException"},
        {"null只能用于运算符 == 和 != (不能进行$(operator)操作)。",
                "NotNullOperatorException"},
        {"操作数不是左值。",
                "NotLValueException"},
        {"下标运算符的左边操作数不是数组类型。",
                "IndexOperandNotArrayException"},
        {"下标运算符的左边操作数不支持下标运算。",
                "IndexOperandNotSupportIndexingException"},
        {"下标运算符中的值不是整数类型。",
                "IndexOperandNotIntException"},
        {"数组下标越界。数组大小为$(size)，访问的下标为[$(index)]。",
                "ArrayIndexOutOfBoundsException"},
        {"数组切片下标越界。数组容量为$(size)，切片的下标为[$(begin):$(end)]。",
                "ArraySliceOutOfBoundsException"},
        {"对象中不存在名为$(method_name)的成员。",
                "NoSuchMethodException"},
        {"自增/自减的目标值不是整数类型。",
                "IncDecOperandTypeException"},
        {"自增/自减的目标值不存在。",
                "IncDecOperandNotExistException"},
        {"函数调用的目标不是函数。",
                "NotFunctionException"},
        {"不能更改对象以外的成员。",
                "NotObjectMemberUpdateException"},
        {"不能赋值给对象以外的成员",
                "NotObjectMemberAssignException"},
        {"对象中不存在成员$(member_name)。",
                "NoSuchMemberException"},
        {"数据类型没有任何成员。",
                "NoMemberTypeException"},
        {"运算符$(operator)不能用于字符串。",
                "BadOperatorForStringException"},
        {"不能被0除。",
                "DivisionByZeroException"},
        {"全局变量$(name)不存在。",
                "GlobalVariableNotFoundException"},
        {"不能在函数外使用global语句。",
                "GlobalStatementInToplevelException"},
        {"函数$(name)已经存在。",
                "FunctionExistsException"},
        {"数组的resize()必须传入整数类型(不能传入$(type))。",
                "ArrayResizeArgumentException"},
        {"数组insert()的第一个参数必须是整数值(不能传入$(type))。",
                "ArrayInsertArgumentException"},
        {"数组的remove()必须传入整数值(不能传入$(type))。",
                "ArrayRemoveArgumentException"},
        {"指定的位置超出字符串长度。为长度为$(len)的字符串指定了$(pos)。",
                "StringPositionOutOfBoundsException"},
        {"截取字符串的长度超出范围(超出$(len))。",
                "StringSubstrLengthException"},
        {"截取字符串的位置必须是整数值。(传递了$(type1)和$(type2))。",
                "StringSubstrArgumentException"},
        {"异常中没有message成员。",
                "ExceptionHasNoMessageException"},
        {"异常的message成员不是字符串值(不能传入$(type))。",
                "ExceptionMessageIsNotStringException"},
        {"异常中没有stack_trace成员。",
                "ExceptionHasNoStackTraceException"},
        {"异常的栈轨迹不是数组类型。",
                "StackTraceIsNotArrayException"},
        {"异常栈轨迹中的元素不是对象类型。",
                "StackTraceLineIsNotAssocException"},
        {"栈轨迹中的行没有包含行号。",
                "StackTraceLineHasNoLineNumberException"},
        {"栈轨迹中的行里不包含函数名。",
                "StackTraceLineHasNoFuncNameException"},
        {"对象之外的异常已经传播到了顶层结构",
                "ExceptionIsNotAssocException"},
        {"异常中没有print_stack_trace成员。",
                "ExceptionHasNoPrintStackTraceMethodException"},
        {"异常的print_stack_trace成员不是闭包",
                "PrintStackTraceIsNotClosureException"},
        {"不正确的多字节字符。",
                "BadMultibyteCharacterException"},
        {"异常类型($(type))不是对象。",
                "ExceptionClassIsNotAssocException"},
        {"异常类中没有create method。",
                "ExceptionClassHasNoCreateMethodException"},
        {"向函数$(func_name)的第$(idx)个参数传递了错误的类型$(type)。",
                "ArgumentTypeMismatchException"},
        {"预期外的宽字符串。",
                "UnexpectedWideStringException"},
        {"正则表达式检索失败。$(message)",
                "OnigSearchFailException"},
        {"正则表达式的组号过大。",
                "GroupIndexOverflowException"},
        {"没有与指定组号(\\$(g_idx))对应的组。",
                "GroupIndexOverflowException"},
        {"break或continue或return被传播到了顶层结构。",
                "BreakOrContinueReachedToplevelException"},
        {"变量或成员$(name)是final值。",
                "AssignToFinalVariableException"},
        {"找不到函数($(name))。",
                "FunctionNotFoundException"},
        {"dummy", NULL}
};

CRB_ErrorDefinition crb_native_error_message_format[] = {
        {"请为fopen()函数传入文件的路径和打开方式（两者都是字符串类型的）。",
                "FOpenArgumentTypeException"},
        {"请为fclose()函数传入文件指针。",
                "FCloseArgumentTypeException"},
        {"请为fgets()函数传入文件指针。",
                "FGetsArgumentTypeException"},
        {"文件已经关闭。",
                "FileAlreadyClosedException"},
        {"请为fputs()函数传入文件指针和字符串。",
                "FPutsArgumentTypeException"},
        {"请为array()函数传入整数类型（数组的大小）。",
                "NewArrayArgumentTypeException"},
        {"请为array()函数传递数组大小和维数。",
                "NewArrayArgumentTooFewException"},
        {"exit()的参数不是整数值(不能传入$(type))。",
                "ExitArgumentTypeException"},
        {"exception()的参数不是消息字符串(不能传入$(type))。",
                "NewExceptionArgumentException"},
        {"错误的多字节字符串",
                "FGetsBadMultibyteCharacterException"}
};
