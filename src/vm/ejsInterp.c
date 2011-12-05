/*
 *  ejsInterp.c - Virtual Machine Interpreter for Ejscript.
 *
 *  Copyright (c) All Rights Reserved. See details at the end of the file.
 *
 *  NEXT
 *      - Optimize and cache stack.top
 *      - Fix BAD BINDING
 *      - Move DEBUG op codes out of line
 */

/********************************** Includes **********************************/

#include    "ejs.h"

/********************************** Inline Code *******************************/
/*
 *  The stack is a stack of pointers to EjsVar. The top of stack (stack.top) always points to the current top item 
 *  on the stack. To push a new value, top is incremented then the value is stored. To pop, simply copy the value at 
 *  top and decrement top ptr.
 */
#define top                     (*state.stack)
#define pop(ejs)                (*state.stack--)

#define push(value)             (*(++(state.stack))) = ((EjsVar*) (value))
#define popString(ejs)          ((EjsString*) pop(ejs))
#define popOutside(ejs)         *(ejs->state->stack)--
#define pushOutside(ejs, value) (*(++(ejs->state->stack))) = ((EjsVar*) (value))

#define FRAME                   state.fp
#define FUNCTION                state.fp.function
#define BLOCK                   state.bp
#define HANDLE_GETTER           0x1

#define SWAP if (1) { \
        EjsVar *swap = state.stack[0]; \
        state.stack[0] = state.stack[-1]; \
        state.stack[-1] = swap; \
    }

#define TRACE if (1) { \
        FRAME->filename = GET_STRING(); \
        FRAME->lineNumber = GET_INT(); \
        FRAME->currentLine = GET_STRING(); \
    }

#define DO_GETTER(thisObj) \
    if (top && unlikely(top->isFunction) && !ejs->exception) { \
        handleGetters(ejs, (EjsFunction*) top, (EjsVar*) (thisObj)); \
        CHECK; \
    } else

/*
 *  Get a slot value when we don't know if the object is an EjsObject
 */
#define GET_SLOT(obj, slotNum) getSlot(ejs, (EjsObject*) obj, slotNum)

static MPR_INLINE EjsVar *getSlot(Ejs *ejs, EjsObject *obj, int slotNum) {
    if (ejsIsObject(obj) && slotNum < obj->numProp) {
        return obj->slots[slotNum];
    } else {
        return ejsGetProperty(ejs, (EjsVar*) obj, slotNum);
    }
}

/*
 *  Set a slot value when we don't know if the object is an EjsObject
 */
#define SET_SLOT(obj, slotNum, value, thisObj) \
    if (!storePropertyToSlot(ejs, (EjsObject*) obj, slotNum, (EjsVar*) value, (EjsVar*) thisObj)) { \
        return; \
    } else 

static MPR_INLINE void handleSetters(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, EjsVar *value);
static bool payAttention(Ejs *ejs);
static void callFunction(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, int stackAdjust);
static void callProperty(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, int stackAdjust);

#define GET_BYTE()      *(FRAME)->pc++
#define GET_DOUBLE()    ejsDecodeDouble(ejs, &(FRAME)->pc)
#define GET_INT()       (int) ejsDecodeNum(&(FRAME)->pc)
#define GET_NUM()       ejsDecodeNum(&(FRAME)->pc)
#define GET_NAME()      getNameArg(FRAME)
#define GET_STRING()    getStringArg(FRAME)
#define GET_TYPE()      ((EjsType*) getGlobalArg(ejs, FRAME))
#define GET_WORD()      ejsDecodeWord(&(FRAME)->pc)
#undef THIS
#define THIS            FRAME->function.thisObj
#define FILL(mark)      while (mark < FRAME->pc) { *mark++ = EJS_OP_NOP; }

#if BLD_DEBUG
    static EjsOpCode traceCode(Ejs *ejs, EjsOpCode opcode);
    static int opcount[256];
#else
    #define traceCode(ejs, opcode) opcode
#endif

#if BLD_UNIX_LIKE || (VXWORKS && !BLD_CC_DIAB)
    #define CASE(opcode) opcode
    #define BREAK \
        if (1) { \
            opcode = GET_BYTE(); \
            goto *opcodeJump[traceCode(ejs, opcode)]; \
        } else
    #define CHECK \
        if (unlikely(ejs->attention) && !payAttention(ejs)) { \
            ejs->state = priorState; \
            return; \
        }
#else
    /*
     *  Traditional switch for compilers (looking at you MS) without computed goto.
     */
    #define BREAK break
    #define CHECK
    #define CASE(opcode) case opcode
#endif

/******************************** Forward Declarations ************************/

static void callConstructor(Ejs *ejs, EjsFunction *vp, int argc, int stackAdjust);
static void callInterfaceInitializers(Ejs *ejs, EjsType *type);
static void callFunction(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, int stackAdjust);
static EjsVar *evalBinaryExpr(Ejs *ejs, EjsVar *lhs, EjsOpCode opcode, EjsVar *rhs);
static EjsVar *evalUnaryExpr(Ejs *ejs, EjsVar *lhs, EjsOpCode opcode);
static inline uint findEndException(Ejs *ejs);
static inline EjsEx *findExceptionHandler(Ejs *ejs, int kind);
static EjsName getNameArg(EjsFrame *fp);
static EjsVar *getNthBase(Ejs *ejs, EjsVar *obj, int nthBase);
static EjsVar *getNthBaseFromBottom(Ejs *ejs, EjsVar *obj, int nthBase);
static MPR_INLINE EjsVar *getNthBlock(Ejs *ejs, int nth);
static char *getStringArg(EjsFrame *fp);
static EjsVar *getGlobalArg(Ejs *ejs, EjsFrame *fp);
static bool manageExceptions(Ejs *ejs);
static void checkExceptionHandlers(Ejs *ejs);
static void handleGetters(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj);
static EjsBlock *popExceptionBlock(Ejs *ejs);
static void createExceptionBlock(Ejs *ejs, EjsEx *ex, int flags);
static void storeProperty(Ejs *ejs, EjsVar *obj, EjsName *name, bool dup);
static MPR_INLINE int storePropertyToSlot(Ejs *ejs, EjsObject *obj, int slotNum, EjsVar *value, EjsVar *thisObj);
static void storePropertyToScope(Ejs *ejs, EjsName *qname, bool dup);
static void throwNull(Ejs *ejs);

/************************************* Code ***********************************/
/*
 *  Virtual Machine byte code evaluation
 */
static void VM(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, int stackAdjust)
{
    /*
     *  Ordered to assist debugging
     */
    EjsState        *priorState, state;
    EjsName         qname;
    EjsVar          *result, *vp, *v1, *v2;
    EjsObject       *obj;
    int             slotNum, nthBase;

    EjsBlock        *blk;
    EjsVar          *vobj;
    EjsString       *nameVar, *spaceVar;
    EjsType         *type;
    EjsObject       *global;
    EjsLookup       lookup;
    EjsEx           *ex;
    EjsFrame        *newFrame;
    char            *str;
    int             i, offset, count, opcode;

#if BLD_UNIX_LIKE || (VXWORKS && !BLD_CC_DIAB)

    /*
     *  Direct threading computed goto processing. Include computed goto jump table.
     */
#include    "ejsByteGoto.h"
#endif
    mprAssert(ejs);
    mprAssert(!mprHasAllocError(ejs));

    vp = 0;
    slotNum = -1;
    global = (EjsObject*) ejs->global;

    priorState = ejs->state;
    state = *ejs->state;
    ejs->state = &state;

    callFunction(ejs, fun, thisObj, argc, stackAdjust);
    FRAME->caller = 0;
    FRAME->currentLine = 0;
    FRAME->filename = 0;
    FRAME->lineNumber = 0;

#if BLD_UNIX_LIKE || (VXWORKS && !BLD_CC_DIAB)
    /*
     *  Direct threading computed goto processing. Include computed goto jump table.
     */
    CHECK; 
    BREAK;
#else
    /*
     *  Traditional switch for compilers (looking at you MS) without computed goto.
     */
    while (1) {
        opcode = (EjsOpCode) GET_BYTE();
        traceCode(ejs, opcode);
        switch (opcode) {
#endif
        /*
         *  Symbolic source code debug information
         *      Debug <filename> <lineNumber> <sourceLine>
         */
        CASE (EJS_OP_DEBUG):
            TRACE; BREAK;

        /*
         *  End of a code block. Used to mark the end of a script. Saves testing end of code block in VM loop.
         *      EndCode
         */
        CASE (EJS_OP_END_CODE):
            /*
             *  The "ejs" command needs to preserve the current ejs->result for interactive sessions.
             */
            if (ejs->result == 0) {
                // OPT - remove this
                ejs->result = ejs->undefinedValue;
            }
            if (FRAME->function.getter) {
                push(ejs->result);
            }
            FRAME = 0;
            goto done;

        /*
         *  Return from a function with a result
         *      ReturnValue
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_RETURN_VALUE):
            ejs->result = pop(ejs);
            mprAssert(ejs->exception || ejs->result);
            if (FRAME->caller == 0) {
                goto done;
            }
            state.stack = FRAME->stackReturn;
            if (FRAME->function.getter) {
                push(ejs->result);
            }
            state.bp = FRAME->function.block.prev;
            newFrame = FRAME->caller;
            FRAME = newFrame;
            BREAK;

        /*
         *  Return from a function without a result
         *      Return
         */
        CASE (EJS_OP_RETURN):
            ejs->result = ejs->undefinedValue;
            if (FRAME->caller == 0) {
                goto done;
            }
            state.stack = FRAME->stackReturn;
            state.bp = FRAME->function.block.prev;
            newFrame = FRAME->caller;
            FRAME = newFrame;
            BREAK;

        /*
         *  Load the catch argument
         *      PushCatchArg
         *      Stack before (top)  []
         *      Stack after         [catchArg]
         */
        CASE (EJS_OP_PUSH_CATCH_ARG):
            push(ejs->exceptionArg);
            ejs->exceptionArg = 0;
            BREAK;

        /*
         *  Push the function call result
         *      PushResult
         *      Stack before (top)  []
         *      Stack after         [result]
         */
        CASE (EJS_OP_PUSH_RESULT):
            push(ejs->result);
            BREAK;

        /*
         *  Save the top of stack and store in the interpreter result register
         *      SaveResult
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_SAVE_RESULT):
            ejs->result = pop(ejs);
            mprAssert(ejs->exception || ejs->result);
            BREAK;

        /*
         *  Load Constants -----------------------------------------------
         */

        /*
         *  Load a float constant
         *      LoadDouble          <double>
         *      Stack before (top)  []
         *      Stack after         [Double]
         */
        CASE (EJS_OP_LOAD_DOUBLE):
#if BLD_FEATURE_FLOATING_POINT
            push(ejsCreateNumber(ejs, GET_DOUBLE()));
#else
            ejsThrowReferenceError(ejs, "No floating point support");
#endif
            CHECK; BREAK;

        /*
         *  Load a signed integer constant (up to 55 bits worth of data)
         *      LoadInt.64          <int64>
         *      Stack before (top)  []
         *      Stack after         [Number]
         */
        CASE (EJS_OP_LOAD_INT):
            push(ejsCreateNumber(ejs, (MprNumber) GET_NUM()));
            CHECK; BREAK;

        /*
         *  Load integer constant between 0 and 9
         *      Load0, Load1, ... Load9
         *      Stack before (top)  []
         *      Stack after         [Number]
         */
        CASE (EJS_OP_LOAD_0):
        CASE (EJS_OP_LOAD_1):
        CASE (EJS_OP_LOAD_2):
        CASE (EJS_OP_LOAD_3):
        CASE (EJS_OP_LOAD_4):
        CASE (EJS_OP_LOAD_5):
        CASE (EJS_OP_LOAD_6):
        CASE (EJS_OP_LOAD_7):
        CASE (EJS_OP_LOAD_8):
        CASE (EJS_OP_LOAD_9):
            push(ejsCreateNumber(ejs, opcode - EJS_OP_LOAD_0));
            CHECK; BREAK;

        /*
         *  Load the -1 integer constant
         *      LoadMinusOne
         *      Stack before (top)  []
         *      Stack after         [Number]
         */
        CASE (EJS_OP_LOAD_M1):
            push(ejsCreateNumber(ejs, -1));
            BREAK;

        /*
         *  Load a string constant
         *      LoadString          <string>
         *      Stack before (top)  []
         *      Stack after         [String]
         */
        CASE (EJS_OP_LOAD_STRING):
            str = GET_STRING();
            push(ejsCreateString(ejs, str));
            CHECK; BREAK;

        /*
         *  Load a namespace constant
         *      LoadNamespace       <UriString>
         *      Stack before (top)  []
         *      Stack after         [Namespace]
         */
        CASE (EJS_OP_LOAD_NAMESPACE):
            str = GET_STRING();
            push(ejsCreateNamespace(ejs, str, str));
            CHECK; BREAK;


        /*
         *  Load an XML constant
         *      LoadXML             <xmlString>
         *      Stack before (top)  []
         *      Stack after         [XML]
         */
        CASE (EJS_OP_LOAD_XML):
#if BLD_FEATURE_EJS_E4X
            v1 = (EjsVar*) ejsCreateXML(ejs, 0, 0, 0, 0);
            str = GET_STRING();
            ejsLoadXMLString(ejs, (EjsXML*) v1, str);
            push(v1);
#endif
            CHECK; BREAK;

        /*
         *  Load a Regexp constant
         *      LoadRegExp
         *      Stack before (top)  []
         *      Stack after         [RegExp]
         */
        CASE (EJS_OP_LOAD_REGEXP):
            str = GET_STRING();
#if BLD_FEATURE_REGEXP
            v1 = (EjsVar*) ejsCreateRegExp(ejs, str);
            push(v1);
#else
            ejsThrowReferenceError(ejs, "No regular expression support");
#endif
            CHECK; BREAK;

        /*
         *  Load a null constant
         *      LoadNull
         *      Stack before (top)  []
         *      Stack after         [Null]
         */
        CASE (EJS_OP_LOAD_NULL):
            push(ejs->nullValue);
            BREAK;

        /*
         *  Load a void / undefined constant
         *      LoadUndefined
         *      Stack before (top)  []
         *      Stack after         [undefined]
         */
        CASE (EJS_OP_LOAD_UNDEFINED):
            push(ejs->undefinedValue);
            BREAK;

        /*
         *  Load the "this" value
         *      LoadThis
         *      Stack before (top)  []
         *      Stack after         [this]
         */
        CASE (EJS_OP_LOAD_THIS):
            push(THIS);
            BREAK;

        /*
         *  Load the "global" value
         *      LoadGlobal
         *      Stack before (top)  []
         *      Stack after         [global]
         */
        CASE (EJS_OP_LOAD_GLOBAL):
            push(ejs->global);
            BREAK;

        /*
         *  Load the "true" value
         *      LoadTrue
         *      Stack before (top)  []
         *      Stack after         [true]
         */
        CASE (EJS_OP_LOAD_TRUE):
            push(ejs->trueValue);
            BREAK;

        /*
         *  Load the "false" value
         *      LoadFalse
         *      Stack before (top)  []
         *      Stack after         [false]
         */
        CASE (EJS_OP_LOAD_FALSE):
            push(ejs->falseValue);
            BREAK;

        /*
         *  Load a global variable by slot number
         *      GetGlobalSlot       <slot>
         *      Stack before (top)  []
         *      Stack after         [PropRef]
         */
        CASE (EJS_OP_GET_GLOBAL_SLOT):
            push(GET_SLOT(global, GET_INT()));
            DO_GETTER(NULL); 
            BREAK;

        /*
         *  Load a local variable by slot number
         *      GetLocalSlot        <slot>
         *      Stack before (top)  []
         *      Stack after         [PropRef]
         */
        CASE (EJS_OP_GET_LOCAL_SLOT):
            push(GET_SLOT(FRAME, GET_INT()));
            DO_GETTER(NULL); 
            BREAK;

        /*
         *  Load a local variable in slot 0-9
         *      GetLocalSlot0, GetLocalSlot1, ... GetLocalSlot9
         *      Stack before (top)  []
         *      Stack after         [PropRef]
         */
        CASE (EJS_OP_GET_LOCAL_SLOT_0):
        CASE (EJS_OP_GET_LOCAL_SLOT_1):
        CASE (EJS_OP_GET_LOCAL_SLOT_2):
        CASE (EJS_OP_GET_LOCAL_SLOT_3):
        CASE (EJS_OP_GET_LOCAL_SLOT_4):
        CASE (EJS_OP_GET_LOCAL_SLOT_5):
        CASE (EJS_OP_GET_LOCAL_SLOT_6):
        CASE (EJS_OP_GET_LOCAL_SLOT_7):
        CASE (EJS_OP_GET_LOCAL_SLOT_8):
        CASE (EJS_OP_GET_LOCAL_SLOT_9):
            push(GET_SLOT(FRAME, opcode - EJS_OP_GET_LOCAL_SLOT_0));
            DO_GETTER(NULL); 
            BREAK;

        /*
         *  Load a block scoped variable by slot number
         *      GetBlockSlot        <slot> <nthBlock>
         *      Stack before (top)  []
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_BLOCK_SLOT):
            slotNum = GET_INT();
            obj = (EjsObject*) getNthBlock(ejs, GET_INT());
            push(GET_SLOT(obj, slotNum));
            DO_GETTER(NULL); 
            BREAK;

        /*
         *  Load a property in thisObj by slot number
         *      GetThisSlot         <slot>
         *      Stack before (top)  []
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_THIS_SLOT):
            push(GET_SLOT(THIS, GET_INT()));
            DO_GETTER(THIS); 
            BREAK;

        /*
         *  Load a property in slot 0-9
         *      GetThisSlot0, GetThisSlot1,  ... GetThisSlot9
         *      Stack before (top)  []
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_THIS_SLOT_0):
        CASE (EJS_OP_GET_THIS_SLOT_1):
        CASE (EJS_OP_GET_THIS_SLOT_2):
        CASE (EJS_OP_GET_THIS_SLOT_3):
        CASE (EJS_OP_GET_THIS_SLOT_4):
        CASE (EJS_OP_GET_THIS_SLOT_5):
        CASE (EJS_OP_GET_THIS_SLOT_6):
        CASE (EJS_OP_GET_THIS_SLOT_7):
        CASE (EJS_OP_GET_THIS_SLOT_8):
        CASE (EJS_OP_GET_THIS_SLOT_9):
            push(GET_SLOT(THIS, opcode - EJS_OP_GET_THIS_SLOT_0));
            DO_GETTER(THIS); 
            BREAK;

        /*
         *  Load a property in an object by slot number
         *      GetObjSlot          <slot>
         *      Stack before (top)  [obj]
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_OBJ_SLOT):
            vp = pop(ejs);
            push(GET_SLOT(vp, GET_INT()));
            DO_GETTER(vp); 
            BREAK;

        /*
         *  Load a property in an object from slot 0-9
         *      GetObjSlot0, GetObjSlot1, ... GetObjSlot9
         *      Stack before (top)  [obj]
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_OBJ_SLOT_0):
        CASE (EJS_OP_GET_OBJ_SLOT_1):
        CASE (EJS_OP_GET_OBJ_SLOT_2):
        CASE (EJS_OP_GET_OBJ_SLOT_3):
        CASE (EJS_OP_GET_OBJ_SLOT_4):
        CASE (EJS_OP_GET_OBJ_SLOT_5):
        CASE (EJS_OP_GET_OBJ_SLOT_6):
        CASE (EJS_OP_GET_OBJ_SLOT_7):
        CASE (EJS_OP_GET_OBJ_SLOT_8):
        CASE (EJS_OP_GET_OBJ_SLOT_9):
            vp = pop(ejs);
            push(GET_SLOT(vp, opcode - EJS_OP_GET_OBJ_SLOT_0));
            DO_GETTER(vp); 
            BREAK;

        /*
         *  Load a variable from a type by slot number
         *      GetTypeSlot         <slot> <nthBase>
         *      Stack before (top)  [objRef]
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_TYPE_SLOT):
            slotNum = GET_INT();
            obj = (EjsObject*) pop(ejs);
            vp = getNthBase(ejs, (EjsVar*) obj, GET_INT());
            push(GET_SLOT(vp, slotNum));
            DO_GETTER(obj); 
            BREAK;

        /*
         *  Load a type variable by slot number from this. NthBase counts from Object up rather than "this" down.
         *      GetThisTypeSlot     <slot> <nthBaseFromBottom>
         *      Stack before (top)  []
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_THIS_TYPE_SLOT):
            slotNum = GET_INT();
            type = (EjsType*) getNthBaseFromBottom(ejs, THIS, GET_INT());
            if (type == 0) {
                ejsThrowReferenceError(ejs, "Bad base class reference");
            } else {
                push(GET_SLOT(type, slotNum));
                DO_GETTER(THIS);
            }
            BREAK;

        /*
         *  Load a variable by an unqualified name
         *      GetScopedName       <qname>
         *      Stack before (top)  []
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_SCOPED_NAME):
#if DYNAMIC_BINDING
            mark = FRAME->pc - 1;
#endif
            qname = GET_NAME();
            vp = ejsGetVarByName(ejs, NULL, &qname, &lookup);
            if (unlikely(vp == 0)) {
                if (ejs->flags & EJS_FLAG_COMPILER) {
                    push(ejs->undefinedValue);
                } else {
                    ejsThrowReferenceError(ejs, "%s is not defined", qname.name);
                }
                CHECK;
            } else {
                push(vp);
                DO_GETTER(NULL);
            }
#if DYNAMIC_BINDING
            //  OPT FUNCTIONALIZE inline
            if (ejs->flags & EJS_FLAG_COMPILER || lookup.obj->type == ejs->objectType || lookup.slotNum >= 4096) {
                BREAK;
            }
            if (lookup.obj == ejs->global) {
                *mark++ = EJS_OP_GET_GLOBAL_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (lookup->obj == state->fp) {
                *mark++ = EJS_OP_GET_LOCAL_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (lookup->obj == state->fp->thisObj) {
                *mark++ = EJS_OP_GET_THIS_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (ejsIsA(ejs, THIS, (EjsType*) lookup.obj)) {
                *mark++ = EJS_OP_GET_BLOCK_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);
                mark += ejsEncodeUint(mark, lookup.nthBlock);

            } else {
                BREAK;
            }
            FILL(mark);
#endif
            BREAK;
                
        /*
         *  Load a variable by an unqualified name expression
         *      GetScopedNameExpr
         *      Stack before (top)  [name]
         *                          [space]
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_SCOPED_NAME_EXPR):
#if DYNAMIC_BINDING
            mark = FRAME->pc - 1;
#endif
            qname.name = ejsToString(ejs, pop(ejs))->value;
            v1 = pop(ejs);
            if (ejsIsNamespace(v1)) {
                qname.space = ((EjsNamespace*) v1)->uri;
            } else {
                qname.space = ejsToString(ejs, v1)->value;
            }
            vp = ejsGetVarByName(ejs, NULL, &qname, &lookup);
            if (unlikely(vp == 0)) {
                if (ejs->flags & EJS_FLAG_COMPILER) {
                    push(ejs->undefinedValue);
                } else {
                    ejsThrowReferenceError(ejs, "%s is not defined", qname.name);
                }
                CHECK;
            } else {
                push(vp);
                DO_GETTER(NULL);
            }
#if DYNAMIC_BINDING
            if (ejs->flags & EJS_FLAG_COMPILER || lookup.obj->type == ejs->objectType || lookup.slotNum >= 4096) {
                BREAK;
            }
            if (lookup.obj == ejs->global) {
                *mark++ = EJS_OP_GET_GLOBAL_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (lookup->obj == state->fp) {
                *mark++ = EJS_OP_GET_LOCAL_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (lookup->obj == state->fp->thisObj) {
                *mark++ = EJS_OP_GET_THIS_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (ejsIsA(ejs, THIS, (EjsType*) lookup.obj)) {
                *mark++ = EJS_OP_GET_BLOCK_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);
                mark += ejsEncodeUint(mark, lookup.nthBlock);

            } else {
                BREAK;
            }
            FILL(mark);
#endif
            BREAK;
                
        /*
         *  Load a property by property name
         *      GetObjName          <qname>
         *      Stack before (top)  [obj]
         *      Stack after         [result]
         */
        CASE (EJS_OP_GET_OBJ_NAME):
#if DYNAMIC_BINDING
            mark = FRAME->pc - 1;
#endif
            qname = GET_NAME();
            vp = pop(ejs);
            if (vp == 0 || vp == ejs->nullValue || vp == ejs->undefinedValue) {
                ejsThrowReferenceError(ejs, "Object reference is null");
                CHECK; BREAK;
            }
            v1 = ejsGetVarByName(ejs, vp, &qname, &lookup);
            push(v1 ? v1 : ejs->undefinedValue);
#if DYNAMIC_BINDING
            if (lookup.slotNum < 0 || lookup.slotNum > 4096 || ejs->flags & EJS_FLAG_COMPILER) {
                BREAK;
            }
            if (lookup.obj == ejs->global) {
                *mark++ = EJS_OP_GET_GLOBAL_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (lookup.obj == (EjsVar*) state.fp) {
                *mark++ = EJS_OP_GET_LOCAL_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else if (lookup.obj == state.fp->thisObj) {
                *mark++ = EJS_OP_GET_THIS_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);
                
            } else if (ejsIsType(lookup.obj) && ejsIsA(ejs, THIS, (EjsType*) lookup.obj)) {
                *mark++ = EJS_OP_GET_TYPE_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);
                mark += ejsEncodeUint(mark, lookup.nthBlock + 1);

            } else if ((EjsVar*) vp->type == lookup.obj) {
                *mark++  = EJS_OP_GET_TYPE_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);
                mark += ejsEncodeUint(mark, lookup.nthBase);

            } else if (ejsIsObject(lookup.obj)) {
                mprAssert(lookup.obj == vp);
                *mark++  = EJS_OP_GET_OBJ_SLOT;
                mark += ejsEncodeUint(mark, lookup.slotNum);

            } else {
                BREAK;
            }
            FILL(mark);
#endif
            DO_GETTER(vp);
            BREAK;

        /*
         *  Load a property by property a qualified name expression
         *      GetObjNameExpr
         *      Stack before (top)  [name]
         *                          [space]
         *                          [obj]
         *      Stack after         [value]
         */
        CASE (EJS_OP_GET_OBJ_NAME_EXPR):
            v1 = pop(ejs);
            v2 = pop(ejs);
            vp = pop(ejs);
            if (vp == 0 || vp == ejs->nullValue || vp == ejs->undefinedValue) {
                ejsThrowReferenceError(ejs, "Object reference is null");
                CHECK; BREAK;
            }
            if (vp->type->numericIndicies && ejsIsNumber(v1)) {
                vp = ejsGetProperty(ejs, vp, ejsGetInt(v1));
                push(vp == 0 ? ejs->nullValue : vp);
                CHECK; BREAK;
            } else {
                qname.name = ejsToString(ejs, v1)->value;
                if (ejsIsNamespace(v2)) {
                    qname.space = ((EjsNamespace*) v2)->uri;
                } else {
                    qname.space = ejsToString(ejs, v2)->value;
                }
                v2 = ejsGetVarByName(ejs, vp, &qname, &lookup);
                push(v2 ? v2 : ejs->undefinedValue);
                DO_GETTER(vp);
                CHECK; BREAK;
            }

        /*
         *  Store -------------------------------
         */

        /*
         *  Store a global variable by slot number
         *      Stack before (top)  [value]
         *      Stack after         []
         *      PutGlobalSlot       <slot>
         */
        CASE (EJS_OP_PUT_GLOBAL_SLOT):
            SET_SLOT(global, GET_INT(), pop(ejs), NULL);
            BREAK;

        /*
         *  Store a local variable by slot number
         *      Stack before (top)  [value]
         *      Stack after         []
         *      PutLocalSlot        <slot>
         */
        CASE (EJS_OP_PUT_LOCAL_SLOT):
            SET_SLOT(FRAME, GET_INT(), pop(ejs), NULL);
            BREAK;

        /*
         *  Store a local variable from slot 0-9
         *      PutLocalSlot0, PutLocalSlot1, ... PutLocalSlot9
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_LOCAL_SLOT_0):
        CASE (EJS_OP_PUT_LOCAL_SLOT_1):
        CASE (EJS_OP_PUT_LOCAL_SLOT_2):
        CASE (EJS_OP_PUT_LOCAL_SLOT_3):
        CASE (EJS_OP_PUT_LOCAL_SLOT_4):
        CASE (EJS_OP_PUT_LOCAL_SLOT_5):
        CASE (EJS_OP_PUT_LOCAL_SLOT_6):
        CASE (EJS_OP_PUT_LOCAL_SLOT_7):
        CASE (EJS_OP_PUT_LOCAL_SLOT_8):
        CASE (EJS_OP_PUT_LOCAL_SLOT_9):
            SET_SLOT(FRAME, opcode - EJS_OP_PUT_LOCAL_SLOT_0, pop(ejs), NULL);
            BREAK;

        /*
         *  Store a block variable by slot number
         *      PutBlockSlot        <slot> <nthBlock>
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_BLOCK_SLOT):
            slotNum = GET_INT();
            obj = (EjsObject*) getNthBlock(ejs, GET_INT());
            SET_SLOT(obj, slotNum, pop(ejs), NULL);
            BREAK;

        /*
         *  Store a property by slot number
         *      PutThisSlot         <slot>
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_THIS_SLOT):
            slotNum = GET_INT();
            SET_SLOT(THIS, slotNum, pop(ejs), THIS);
            BREAK;

        /*
         *  Store a property to slot 0-9
         *      PutThisSlot0, PutThisSlot1, ... PutThisSlot9,
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_THIS_SLOT_0):
        CASE (EJS_OP_PUT_THIS_SLOT_1):
        CASE (EJS_OP_PUT_THIS_SLOT_2):
        CASE (EJS_OP_PUT_THIS_SLOT_3):
        CASE (EJS_OP_PUT_THIS_SLOT_4):
        CASE (EJS_OP_PUT_THIS_SLOT_5):
        CASE (EJS_OP_PUT_THIS_SLOT_6):
        CASE (EJS_OP_PUT_THIS_SLOT_7):
        CASE (EJS_OP_PUT_THIS_SLOT_8):
        CASE (EJS_OP_PUT_THIS_SLOT_9):
            SET_SLOT(THIS, opcode - EJS_OP_PUT_THIS_SLOT_0, pop(ejs), THIS);
            BREAK;

        /* 
         *  Store a property by slot number
         *      PutObjSlot          <slot>
         *      Stack before (top)  [obj]
         *                          [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_OBJ_SLOT):
            vp = pop(ejs);
            SET_SLOT(vp, GET_INT(), pop(ejs), NULL);
            BREAK;

        /*
         *  Store a property to slot 0-9
         *      PutObjSlot0, PutObjSlot1, ... PutObjSlot9
         *      Stack before (top)  [obj]
         *                          [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_OBJ_SLOT_0):
        CASE (EJS_OP_PUT_OBJ_SLOT_1):
        CASE (EJS_OP_PUT_OBJ_SLOT_2):
        CASE (EJS_OP_PUT_OBJ_SLOT_3):
        CASE (EJS_OP_PUT_OBJ_SLOT_4):
        CASE (EJS_OP_PUT_OBJ_SLOT_5):
        CASE (EJS_OP_PUT_OBJ_SLOT_6):
        CASE (EJS_OP_PUT_OBJ_SLOT_7):
        CASE (EJS_OP_PUT_OBJ_SLOT_8):
        CASE (EJS_OP_PUT_OBJ_SLOT_9):
            vp = pop(ejs);
            SET_SLOT(vp, opcode - EJS_OP_PUT_OBJ_SLOT_0, pop(ejs), NULL);
            BREAK;

        /*
         *  Store a variable by an unqualified name
         *      PutScopedName       <qname>
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_SCOPED_NAME):
            qname = GET_NAME();
            storePropertyToScope(ejs, &qname, 0);
            CHECK; BREAK;

        /*
         *  Store a variable by an unqualified name expression
         *      PutScopedName 
         *      Stack before (top)  [name]
         *                          [space]
         *                          [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_SCOPED_NAME_EXPR):
            qname.name = ejsToString(ejs, pop(ejs))->value;
            v1 = pop(ejs);
            if (ejsIsNamespace(v1)) {
                qname.space = ((EjsNamespace*) v1)->uri;
            } else {
                qname.space = ejsToString(ejs, v1)->value;
            }
            storePropertyToScope(ejs, &qname, 1);
            CHECK; BREAK;

        /*
         *  Store a property by property name to an object
         *      PutObjName
         *      Stack before (top)  [value]
         *                          [objRef]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_OBJ_NAME):
            qname = GET_NAME();
            vp = pop(ejs);
            storeProperty(ejs, vp, &qname, 0);
            CHECK; BREAK;

        /*
         *  Store a property by a qualified property name expression to an object
         *      PutObjNameExpr
         *      Stack before (top)  [nameExpr]
         *                          [spaceExpr]
         *                          [objRef]
         *                          [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_OBJ_NAME_EXPR):
            v1 = pop(ejs);
            v2 = pop(ejs);
            vp = pop(ejs);
            if (vp->type->numericIndicies && ejsIsNumber(v1)) {
                ejsSetProperty(ejs, vp, ejsGetInt(v1), pop(ejs));
            } else {
                qname.name = ejsToString(ejs, v1)->value;
                if (ejsIsNamespace(v2)) {
                    qname.space = ((EjsNamespace*) v2)->uri;
                } else {
                    qname.space = ejsToString(ejs, v2)->value;
                }
                if (qname.name && qname.space) {
                    storeProperty(ejs, vp, &qname, 1);
                }
            }
            CHECK; BREAK;

        /*
         *  Store a type variable by slot number
         *      PutTypeSlot         <slot> <nthBase>
         *      Stack before (top)  [obj]
         *                          [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_TYPE_SLOT):
            slotNum = GET_INT();
            vobj = pop(ejs);
            vp = getNthBase(ejs, vobj, GET_INT());
            SET_SLOT(vp, slotNum, pop(ejs), vobj);
            BREAK;

        /*
         *  Store a variable to a slot in the nthBase class of the current "this" object
         *      PutThisTypeSlot     <slot> <nthBase>
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_PUT_THIS_TYPE_SLOT):
            slotNum = GET_INT();
            type = (EjsType*) getNthBaseFromBottom(ejs, THIS, GET_INT());
            if (type == 0) {
                ejsThrowReferenceError(ejs, "Bad base class reference");
            } else {
                SET_SLOT(type, slotNum, pop(ejs), THIS);
            }
            CHECK; BREAK;


        /*
         *  Function calling and return
         */

        /*
         *  Call a function by reference
         *      Stack before (top)  [args]
         *                          [function]
         *                          [thisObj]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL):
            argc = GET_INT();
            vp = state.stack[-argc - 1];
            fun = (EjsFunction*) state.stack[-argc];
            callProperty(ejs, fun, vp, argc, 2);
            CHECK; BREAK;

        /*
         *  Call a global function by slot on the given type
         *      CallGlobalSlot      <slot> <argc>
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_GLOBAL_SLOT):
            slotNum = GET_INT();
            argc = GET_INT();
            callProperty(ejs, (EjsFunction*) global->slots[slotNum], NULL, argc, 0);
            CHECK; BREAK;

        /*
         *  Call a function by slot number on the pushed object
         *      CallObjSlot         <slot> <argc>
         *      Stack before (top)  [args]
         *                          [obj]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_OBJ_SLOT):
            slotNum = GET_INT();
            argc = GET_INT();
            vp = state.stack[-argc];
            if (vp == 0 || vp == ejs->nullValue || vp == ejs->undefinedValue) {
                if (vp && (slotNum == ES_Object_get || slotNum == ES_Object_getValues)) {
                    callProperty(ejs, (EjsFunction*) vp->type->block.obj.slots[slotNum], vp, argc, 1);
                } else {
                    ejsThrowReferenceError(ejs, "Object reference is null or undefined");
                }
            } else {
                callProperty(ejs, (EjsFunction*) vp->type->block.obj.slots[slotNum], vp, argc, 1);
            }
            CHECK; BREAK;

        /*
         *  Call a function by slot number on the current this object.
         *      CallThisSlot        <slot> <argc>
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_THIS_SLOT):
            slotNum = GET_INT();
            argc = GET_INT();
            obj = (EjsObject*) THIS->type;
            if (ejsIsObject(obj) && slotNum < obj->numProp) {
                callProperty(ejs, (EjsFunction*) obj->slots[slotNum], NULL, argc, 0);
            } else {
                ejsThrowTypeError(ejs, "Property is not a function");
            }
            CHECK; BREAK;

        /*
         *  Call a function by slot number on the nth enclosing block
         *      CallBlockSlot        <slot> <nthBlock> <argc>
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_BLOCK_SLOT):
            slotNum = GET_INT();
            obj = (EjsObject*) getNthBlock(ejs, GET_INT());
            argc = GET_INT();
            callProperty(ejs, (EjsFunction*) obj->slots[slotNum], NULL, argc, 0);
            CHECK; BREAK;

        /*
         *  Call a function by slot number on an object.
         *      CallObjInstanceSlot <slot> <argc>
         *      Stack before (top)  [args]
         *                          [obj]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_OBJ_INSTANCE_SLOT):
            slotNum = GET_INT();
            argc = GET_INT();
            vp = state.stack[-argc];
            if (vp == 0 || vp == ejs->nullValue) {
                ejsThrowReferenceError(ejs, "Object reference is null");
            } else {
                fun = (EjsFunction*) ejsGetProperty(ejs, vp, slotNum);
                callProperty(ejs, fun, vp, argc, 1);
            }
            CHECK; BREAK;

        /*
         *  Call a static function by slot number on the pushed object
         *      CallObjStaticSlot   <slot> <nthBase> <argc>
         *      Stack before (top)  [args]
         *                          [obj]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_OBJ_STATIC_SLOT):
            slotNum = GET_INT();
            nthBase = GET_INT();
            argc = GET_INT();
            vp = state.stack[-argc];
            if (vp == ejs->nullValue) {
                throwNull(ejs);
            } else {
                type = (EjsType*) getNthBase(ejs, vp, nthBase);
                callProperty(ejs, (EjsFunction*) type->block.obj.slots[slotNum], (EjsVar*) type, argc, 1);
            }
            CHECK; BREAK;

        /*
         *  Call a static function by slot number on the nth base class of the current "this" object
         *      CallThisStaticSlot  <slot> <nthBase> <argc>
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_THIS_STATIC_SLOT):
            slotNum = GET_INT();
            nthBase = GET_INT();
            argc = GET_INT();
            type = (EjsType*) getNthBase(ejs, THIS, nthBase);
            if (type == ejs->objectType) {
                ejsThrowReferenceError(ejs, "Bad type reference");
                CHECK; BREAK;
            }
            callProperty(ejs, (EjsFunction*) type->block.obj.slots[slotNum], (EjsVar*) type, argc, 0);
            CHECK; BREAK;

        /*
         *  Call a function by name on the pushed object
         *      CallObjName         <qname> <argc>
         *      Stack before (top)  [args]
         *                          [obj]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_OBJ_NAME):
            qname = GET_NAME();
            argc = GET_INT();
            vp = state.stack[-argc];
            if (vp == 0) {
                ejsThrowReferenceError(ejs, "%s is not defined", qname.name);
                throwNull(ejs);
                CHECK; BREAK;
            }
            slotNum = ejsLookupVar(ejs, (EjsVar*) vp, &qname, &lookup);
            if (slotNum < 0) {
                ejsThrowReferenceError(ejs, "Can't find function \"%s\"", qname.name);
            } else {
                fun = (EjsFunction*) ejsGetProperty(ejs, lookup.obj, slotNum);
                if (fun->getter) {
                    fun = (EjsFunction*) ejsRunFunction(ejs, fun, vp, 0, NULL);
                }
                callProperty(ejs, fun, vp, argc, 1);
            }
            CHECK; BREAK;

        /*
         *  Call a function by name in the current scope. Use existing "this" object if defined.
         *      CallName            <qname> <argc>
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_SCOPED_NAME):
            qname = GET_NAME();
            argc = GET_INT();
            lookup.storing = 0;
            slotNum = ejsLookupScope(ejs, &qname, &lookup);
            if (slotNum < 0) {
                ejsThrowReferenceError(ejs, "Can't find method %s", qname.name);
                CHECK; BREAK;
            }
            fun = (EjsFunction*) ejsGetProperty(ejs, lookup.obj, slotNum);
            if (!ejsIsFunction(fun)) {
                callConstructor(ejs, fun, argc, 0);
            } else {
                /*
                 *  Calculate the "this" to use for the function. If required function is a method in the current 
                 *  "this" object use the current thisObj. If the lookup.obj is a type, then use it. Otherwise global.
                 */
                if ((vp = fun->thisObj) == 0) {
                    if (ejsIsA(ejs, THIS, (EjsType*) lookup.obj)) {
                        vp = THIS;
                    } else if (ejsIsType(lookup.obj)) {
                        vp = lookup.obj;
                    } else {
                        vp = ejs->global;
                    }
                }
                if (fun->getter) {
                    fun = (EjsFunction*) ejsRunFunction(ejs, fun, vp, 0, NULL);
                }
                callProperty(ejs, fun, vp, argc, 0);
            }
            CHECK; BREAK;

        /*
         *  Call a constructor
         *      CallConstructor     <argc>
         *      Stack before (top)  [args]
         *                          [obj]
         *      Stack after         [obj]
         */
        CASE (EJS_OP_CALL_CONSTRUCTOR):
            argc = GET_INT();
            vp = state.stack[-argc];
            if (vp == 0 || vp == ejs->nullValue) {
                throwNull(ejs);
                CHECK; BREAK;
            }
            type = vp->type;
            mprAssert(type);
            if (type && type->hasConstructor) {
                mprAssert(type->baseType);
                //  Constructor is always at slot 0 (offset by base properties)
                slotNum = type->block.numInherited;
                fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, slotNum);
                callFunction(ejs, fun, vp, argc, 0);
            }
            CHECK; BREAK;

        /*
         *  Call the next constructor
         *      CallNextConstructor <argc>
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_CALL_NEXT_CONSTRUCTOR):
            argc = GET_INT();
            type = (EjsType*) FRAME->function.owner;
            mprAssert(type);
            type = type->baseType;
            if (type) {
                mprAssert(type->hasConstructor);
                slotNum = type->block.numInherited;
                fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, slotNum);
                callFunction(ejs, fun, NULL, argc, 0);
            } else {
                mprAssert(0);
            }
            CHECK; BREAK;

        /*
         *  Add a literal namespace to the set of open namespaces for the current block
         *      AddNamespace <string>
         */
        CASE (EJS_OP_ADD_NAMESPACE):
            str = GET_STRING();
            ejsAddNamespaceToBlock(ejs, state.bp, ejsCreateNamespace(ejs, str, str));
            CHECK; BREAK;

        /*
         *  Add a namespace expression (reference) to the set of open namespaces for the current block. (use namespace).
         *      Stack before (top)  [namespace]
         *      Stack after         []
         *      AddNamespaceRef
         */
        CASE (EJS_OP_ADD_NAMESPACE_REF):
            ejsAddNamespaceToBlock(ejs, state.bp, (EjsNamespace*) pop(ejs));
            CHECK; BREAK;

        /*
         *  Push a new scope block on the scope chain
         *      OpenBlock <slotNum> <nthBlock>
         */
        CASE (EJS_OP_OPEN_BLOCK):
            slotNum = GET_INT();
            vp = getNthBlock(ejs, GET_INT());
            v1 = GET_SLOT(vp, slotNum);
            if (!ejsIsBlock(v1)) {
                ejsThrowReferenceError(ejs, "Reference is not a block");
                CHECK; BREAK;
            }
            //  OPT
            blk = ejsCopyBlock(ejs, (EjsBlock*) v1, 0);
            blk->prev = blk->scopeChain = state.bp;
            state.bp = blk;
            blk->stackBase = state.stack;
            ejsSetDebugName(state.bp, mprGetName(v1));
            CHECK; BREAK;

        /*
         *  Add a new scope block from the stack onto on the scope chain
         *      OpenWith
         */
        CASE (EJS_OP_OPEN_WITH):
            vp = pop(ejs);
            blk = ejsCreateBlock(ejs, 0);
            ejsSetDebugName(blk, "with");
            memcpy((void*) blk, vp, vp->type->instanceSize);
            blk->prev = blk->scopeChain = state.bp;
            state.bp->referenced = 1;
            state.bp = blk;
            CHECK; BREAK;

        /*
         *  Store the top scope block off the scope chain
         *      CloseBlock
         *      CloseWith
         */
        CASE (EJS_OP_CLOSE_BLOCK):
            state.bp = state.bp->prev;
            CHECK; BREAK;

        /*
         *  Define a class and initialize by calling any static initializer.
         *      DefineClass <type>
         */
        CASE (EJS_OP_DEFINE_CLASS):
            type = GET_TYPE();
            if (type == 0 || !ejsIsType(type)) {
                ejsThrowReferenceError(ejs, "Reference is not a class");
            } else {
                type->block.scopeChain = state.bp;
                if (type && type->hasStaticInitializer) {
                    //  Static initializer is always immediately after the constructor (if present)
                    slotNum = type->block.numInherited;
                    if (type->hasConstructor) {
                        slotNum++;
                    }
                    fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, slotNum);
                    callFunction(ejs, fun, (EjsVar*) type, 0, 0);
                    if (type->implements && !ejs->exception) {
                        callInterfaceInitializers(ejs, type);
                    }
                    state.bp = &FRAME->function.block;
                }
            }
            ejs->result = (EjsVar*) type;
            mprAssert(ejs->exception || ejs->result);
            CHECK; BREAK;

        /*
         *  Define a function. This is used only for non-method functions to capture the scope chain.
         *      DefineFunction <slot> <nthBlock>
         */
        CASE (EJS_OP_DEFINE_FUNCTION):
            slotNum = GET_INT();
            vp = getNthBlock(ejs, GET_INT());
            mprAssert(vp != ejs->global);
            fun = (EjsFunction*) GET_SLOT(vp, slotNum);
            if (!ejsIsFunction(fun)) {
                ejsThrowReferenceError(ejs, "Reference is not a function");
            } else if (fun->fullScope) {
                fun = ejsCopyFunction(ejs, fun);
                fun->block.scopeChain = state.bp;
                state.bp->referenced = 1;
                fun->thisObj = FRAME->function.thisObj;
                SET_SLOT(vp, slotNum, fun, NULL);
            }
            CHECK; BREAK;

        /*
         *  Exception Handling --------------------------------------------
         */

        /*
         *  Invoke finally blocks before acting on: return, returnValue and break (goto) opcodes.
         *  Injected by the compiler prior to break, continue and return statements. Also at the end of a try block
         *  if there is a finally block.
         *
         *      finally
         */
        CASE (EJS_OP_FINALLY):
            if ((ex = findExceptionHandler(ejs, EJS_EX_FINALLY)) != 0) {
                if (FRAME->function.inCatch) {
                    popExceptionBlock(ejs);
                    push(FRAME->pc);
                    createExceptionBlock(ejs, ex, EJS_EX_FINALLY);
                    BLOCK->breakCatch = 1;
                } else {
                    createExceptionBlock(ejs, ex, EJS_EX_FINALLY);
                }
            }
            BREAK;

        /*
         *  End of an exception block. Put at the end of the last catch/finally block
         *      EndException
         */
        CASE (EJS_OP_END_EXCEPTION):
            if (FRAME->function.inException) {
                FRAME->function.inCatch = 0;
                FRAME->function.inException = 0;
                if (BLOCK->breakCatch) {
                    /* Restart the original instruction (return, break, continue) */
                    popExceptionBlock(ejs);
                    FRAME->pc = (uchar*) pop(ejs);
                } else {
                    offset = findEndException(ejs);
                    FRAME->pc = &FRAME->function.body.code.byteCode[offset];
                    popExceptionBlock(ejs);
                }
            }
            CHECK; BREAK;

        /*
         *  Throw an exception
         *      Stack before (top)  [exceptionObj]
         *      Stack after         []
         *      Throw
         */
        CASE (EJS_OP_THROW):
            ejs->exception = pop(ejs);
            ejs->attention = 1;
            CHECK; BREAK;

        /*
         *  Stack management ----------------------------------------------
         */

        /*
         *  Pop one item off the stack
         *      Pop
         *      Stack before (top)  [value]
         *      Stack after         []
         */
        CASE (EJS_OP_POP):
            ejs->result = pop(ejs);
            mprAssert(ejs->exception || ejs->result);
            BREAK;

        /*
         *  Pop N items off the stack
         *      PopItems            <count>
         *      Stack before (top)  [value]
         *                          [...]
         *      Stack after         []
         */
        CASE (EJS_OP_POP_ITEMS):
            state.stack -= GET_BYTE();
            BREAK;

        /*
         *  Duplicate one item on the stack
         *      Stack before (top)  [value]
         *      Stack after         [value]
         *                          [value]
         */
        CASE (EJS_OP_DUP):
            vp = state.stack[0];
            push(vp);
            BREAK;

        /*
         *  Duplicate two items on the stack
         *      Dup2
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [value1]
         *                          [value2]
         *                          [value1]
         *                          [value2]
         */
        CASE (EJS_OP_DUP2):
            v1 = state.stack[-1];
            push(v1);
            v1 = state.stack[0];
            push(v1);
            BREAK;

        /*
         *  Swap the top two items on the stack
         *      Swap
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [value2]
         *                          [value1]
         */
        CASE (EJS_OP_SWAP):
            SWAP; BREAK;

        /*
         *  Branching
         */

        /*
         *  Default function argument initialization. Computed goto for functions with more than 256 parameters.
         *      InitDefault         <tableSize>
         */
        CASE (EJS_OP_INIT_DEFAULT_ARGS): {
            int tableSize, numNonDefault;
            /*
             *  Use the argc value for the current function. Compare with the number of default args.
             */
            tableSize = (schar) GET_BYTE();
            numNonDefault = FRAME->function.numArgs - FRAME->function.numDefault;
            mprAssert(numNonDefault >= 0);
            offset = FRAME->argc - numNonDefault;
            if (offset < 0 || offset > tableSize) {
                offset = tableSize;
            }
            FRAME->pc += ((uint*) FRAME->pc)[offset];
            CHECK; BREAK;
        }

        /*
         *  Default function argument initialization. Computed goto for functions with less than 256 parameters.
         *      InitDefault.8       <tableSize.8>
         */
        CASE (EJS_OP_INIT_DEFAULT_ARGS_8): {
            int tableSize, numNonDefault;
            tableSize = (schar) GET_BYTE();
            numNonDefault = FRAME->function.numArgs - FRAME->function.numDefault;
            mprAssert(numNonDefault >= 0);
            offset = FRAME->argc - numNonDefault;
            if (offset < 0 || offset > tableSize) {
                offset = tableSize;
            }
            FRAME->pc += FRAME->pc[offset];
            CHECK; BREAK;
        }

        /*
         *  Unconditional branch to a new location
         *      Goto                <offset.32>
         */
        CASE (EJS_OP_GOTO):
            offset = GET_WORD();
            FRAME->pc = &FRAME->pc[offset];
            BREAK;

        /*
         *  Unconditional branch to a new location (8 bit)
         *      Goto.8              <offset.8>
         */
        CASE (EJS_OP_GOTO_8):
            offset = (schar) GET_BYTE();
            FRAME->pc = &FRAME->pc[offset];
            BREAK;

        /*
         *  Branch to offset if false
         *      BranchFalse
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_FALSE):
            offset = GET_WORD();
            goto commonBoolBranchCode;

        /*
         *  Branch to offset if true
         *      BranchTrue
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_TRUE):
            offset = GET_WORD();
            goto commonBoolBranchCode;

        /*
         *  Branch to offset if false (8 bit)
         *      BranchFalse.8
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_FALSE_8):
            opcode = (EjsOpCode) (opcode - EJS_OP_BRANCH_TRUE_8 + EJS_OP_BRANCH_TRUE);
            offset = (schar) GET_BYTE();
            goto commonBoolBranchCode;

        /*
         *  Branch to offset if true (8 bit)
         *      BranchTrue.8
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_TRUE_8):
            /* We want sign extension here */
            opcode = (EjsOpCode) (opcode - EJS_OP_BRANCH_TRUE_8 + EJS_OP_BRANCH_TRUE);
            offset = (schar) GET_BYTE();

        /*
         *  Common boolean branch code
         */
        commonBoolBranchCode:
            v1 = pop(ejs);
            if (v1 == 0 || !ejsIsBoolean(v1)) {
                v1 = ejsCastVar(ejs, v1, ejs->booleanType);
                if (ejs->exception) {
                    CHECK; BREAK;
                }
            }
            if (!ejsIsBoolean(v1)) {
                ejsThrowTypeError(ejs, "Result of a comparision must be boolean");
                CHECK; BREAK;
            }
            if (opcode == EJS_OP_BRANCH_TRUE) {
                if (((EjsBoolean*) v1)->value) {
                    FRAME->pc = &FRAME->pc[offset];
                }
            } else {
                if (((EjsBoolean*) v1)->value == 0) {
                    FRAME->pc = &FRAME->pc[offset];
                }
            }
            BREAK;

        /*
         *  Branch to offset if [value1] == null
         *      BranchNull
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_NULL):
            push(ejs->nullValue);
            offset = GET_WORD();
            goto commonBranchCode;

        /*
         *  Branch to offset if [value1] == undefined
         *      BranchUndefined
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_UNDEFINED):
            push(ejs->undefinedValue);
            offset = GET_WORD();
            goto commonBranchCode;

        /*
         *  Branch to offset if [tos] value is zero
         *      BranchZero
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_ZERO):
            /* Fall through */

        /*
         *  Branch to offset if [tos] value is not zero
         *      BranchNotZero
         *      Stack before (top)  [boolValue]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_NOT_ZERO):
            push(ejs->zeroValue);
            offset = GET_WORD();
            goto commonBranchCode;

        /*
         *  Branch to offset if [value1] == [value2]
         *      BranchEQ
         *      Stack before (top)  [value1]
         *      Stack before (top)  [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_EQ):

        /*
         *  Branch to offset if [value1] === [value2]
         *      BranchStrictlyEQ
         *      Stack before (top)  [value1]
         *      Stack after         [value2]
         */
        CASE (EJS_OP_BRANCH_STRICTLY_EQ):

        /*
         *  Branch to offset if [value1] != [value2]
         *      BranchNotEqual
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_NE):

        /*
         *  Branch to offset if [value1] !== [value2]
         *      BranchStrictlyNE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_STRICTLY_NE):

        /*
         *  Branch to offset if [value1] <= [value2]
         *      BranchLE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_LE):

        /*
         *  Branch to offset if [value1] < [value2]
         *      BranchLT
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_LT):

        /*
         *  Branch to offset if [value1] >= [value2]
         *      BranchGE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_GE):

        /*
         *  Branch to offset if [value1] > [value2]
         *      BranchGT
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         []
         */
        CASE (EJS_OP_BRANCH_GT):
            offset = GET_WORD();
            goto commonBranchCode;

        /*
         *  Handle all branches here. We convert to a compare opcode and pass to the type to handle.
         */
        commonBranchCode:
            opcode = (EjsOpCode) (opcode - EJS_OP_BRANCH_EQ + EJS_OP_COMPARE_EQ);
            v2 = pop(ejs);
            v1 = pop(ejs);
            result = evalBinaryExpr(ejs, v1, opcode, v2);
            if (!ejsIsBoolean(result)) {
                ejsThrowTypeError(ejs, "Result of a comparision must be boolean");
                CHECK;
            } else {
                if (((EjsBoolean*) result)->value) {
                    FRAME->pc = &FRAME->pc[offset];
                }
            }
            BREAK;

        /*
         *  Compare if [value1] == true
         *      CompareTrue
         *      Stack before (top)  [value]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_TRUE):

        /*
         *  Compare if ![value1]
         *      CompareNotTrue
         *      Stack before (top)  [value]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_FALSE):
            v1 = pop(ejs);
            result = evalUnaryExpr(ejs, v1, opcode);
            mprAssert(ejs->exception || ejs->result);
            push(result);
            CHECK; BREAK;

        /*
         *  Compare if [value1] == NULL
         *      CompareNull
         *      Stack before (top)  [value]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_NULL):
            push(ejs->nullValue);
            goto binaryExpression;

        /*
         *  Compare if [item] == undefined
         *      CompareUndefined
         *      Stack before (top)  [value]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_UNDEFINED):
            push(ejs->undefinedValue);
            goto binaryExpression;

        /*
         *  Compare if [item] value is zero
         *      CompareZero
         *      Stack before (top)  [value]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_ZERO):
            push(ejsCreateNumber(ejs, 0));
            goto binaryExpression;

        /*
         *  Compare if [tos] value is not zero
         *      CompareZero
         *      Stack before (top)  [value]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_NOT_ZERO):
            push(ejsCreateNumber(ejs, 0));
            goto binaryExpression;

        /*
         *  Compare if [value1] == [item2]
         *      CompareEQ
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_EQ):

        /*
         *  Compare if [value1] === [item2]
         *      CompareStrictlyEQ
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_STRICTLY_EQ):

        /*
         *  Compare if [value1] != [item2]
         *      CompareNE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_NE):

        /*
         *  Compare if [value1] !== [item2]
         *      CompareStrictlyNE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_STRICTLY_NE):

        /*
         *  Compare if [value1] <= [item2]
         *      CompareLE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_LE):

        /*
         *  Compare if [value1] < [item2]
         *      CompareStrictlyLT
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_LT):

        /*
         *  Compare if [value1] >= [item2]
         *      CompareGE
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_GE):

        /*
         *  Compare if [value1] > [item2]
         *      CompareGT
         *      Stack before (top)  [value1]
         *                          [value2]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_COMPARE_GT):

        /*
         *  Binary expressions
         *      Stack before (top)  [right]
         *                          [left]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_ADD):
        CASE (EJS_OP_SUB):
        CASE (EJS_OP_MUL):
        CASE (EJS_OP_DIV):
        CASE (EJS_OP_REM):
        CASE (EJS_OP_SHL):
        CASE (EJS_OP_SHR):
        CASE (EJS_OP_USHR):
        CASE (EJS_OP_AND):
        CASE (EJS_OP_OR):
        CASE (EJS_OP_XOR):
        binaryExpression:
            v2 = pop(ejs);
            v1 = pop(ejs);
            ejs->result = evalBinaryExpr(ejs, v1, opcode, v2);
            push(ejs->result);
            CHECK; BREAK;

        /*
         *  Unary operators
         */

        /*
         *  Negate a value
         *      Neg
         *      Stack before (top)  [value]
         *      Stack after         [result]
         */
        CASE (EJS_OP_NEG):
            v1 = pop(ejs);
            result = evalUnaryExpr(ejs, v1, opcode);
            push(result);
            CHECK; BREAK;

        /*
         *  Logical not (~value)
         *      LogicalNot
         *      Stack before (top)  [value]
         *      Stack after         [result]
         */
        CASE (EJS_OP_LOGICAL_NOT):
            v1 = pop(ejs);
            v1 = ejsCastVar(ejs, v1, ejs->booleanType);
            result = evalUnaryExpr(ejs, v1, opcode);
            push(result);
            CHECK; BREAK;

        /*
         *  Bitwise not (!value)
         *      BitwiseNot
         *      Stack before (top)  [value]
         *      Stack after         [result]
         */
        CASE (EJS_OP_NOT):
            v1 = pop(ejs);
            result = evalUnaryExpr(ejs, v1, opcode);
            push(result);
            CHECK; BREAK;

        /*
         *  Increment a stack variable
         *      Inc                 <increment>
         *      Stack before (top)  [value]
         *      Stack after         [result]
         */
        CASE (EJS_OP_INC):
            v1 = pop(ejs);
            count = (schar) GET_BYTE();
            result = evalBinaryExpr(ejs, v1, EJS_OP_ADD, (EjsVar*) ejsCreateNumber(ejs, count));
            push(result);
            CHECK; BREAK;

        /*
         *  Object creation
         */

        /*
         *  Create a new object
         *      New
         *      Stack before (top)  [type]
         *      Stack after         [obj]
         */
        CASE (EJS_OP_NEW):
            type = (EjsType*) pop(ejs);
            if (ejsIsType(type)) {
                v1 = ejsCreateVar(ejs, type, 0);
                push(v1);
                ejs->result = v1;
                mprAssert(ejs->exception || ejs->result);
            } else {
                if (ejsIsFunction(type)) {
                    fun = (EjsFunction*) type;
                    if ((type = fun->prototype) == 0) {
                        type = ejsCreatePrototype(ejs, fun, &slotNum);
                    }
                }
                if (ejsIsType(type)) {
                    v1 = ejsCreateVar(ejs, type, 0);
                    push(v1);
                    ejs->result = v1;
                    mprAssert(ejs->exception || ejs->result);
                } else {
                    ejsThrowReferenceError(ejs, "Can't locate type");
                }
            }
            CHECK; BREAK;

        /*
         *  Create a new object literal
         *      NewObject           <type> <argc.
         *      Stack before (top)  [args]
         *      Stack after         []
         */
        CASE (EJS_OP_NEW_OBJECT):
            type = GET_TYPE();
            argc = GET_INT();
            vp = (EjsVar*) ejsCreateObject(ejs, type, 0);
            for (i = 1 - (argc * 3); i <= 0; ) {
                spaceVar = ejsToString(ejs, state.stack[i++]);
                nameVar = ejsToString(ejs, state.stack[i++]);
                v1 = state.stack[i++];
                if (v1 && nameVar && spaceVar) {
                    if (ejsIsFunction(v1)) {
                        fun = (EjsFunction*) v1;
                        if (fun->literalGetter) {
                            fun->getter = 1;
                        }
                        if (fun->getter || fun->setter) {
                             vp->hasGetterSetter = 1;
                        }
                    }
                    ejsName(&qname, mprStrdup(vp, spaceVar->value), mprStrdup(vp, nameVar->value));
                    vp->noPool = 1;
                    ejsSetPropertyByName(ejs, vp, &qname, v1);
                }
            }
            state.stack -= (argc * 3);
            push(vp);
            CHECK; BREAK;

        /*
         *  Reference the super class
         *      Super
         *      Stack before (top)  [obj]
         *      Stack after         [type]
         */
        CASE (EJS_OP_SUPER):
            push(FRAME->function.thisObj->type);
            BREAK;

        /*
         *  Delete an object property by name expression
         *      DeleteNameExpr
         *      Stack before (top)  [name]
         *                          [space]
         *                          [obj]
         *      Stack after         []
         */
        CASE (EJS_OP_DELETE_NAME_EXPR):
            qname.name = ejsToString(ejs, pop(ejs))->value;
            v1 = pop(ejs);
            if (ejsIsNamespace(v1)) {
                qname.space = ((EjsNamespace*) v1)->uri;
            } else {
                qname.space = ejsToString(ejs, v1)->value;
            }
            vp = pop(ejs);
            ejsDeletePropertyByName(ejs, vp, &qname);
            CHECK; BREAK;

        /*
         *  Delete an object property from the current scope
         *      DeleteScopedNameExpr
         *      Stack before (top)  [name]
         *                          [space]
         *      Stack after         []
         */
        CASE (EJS_OP_DELETE_SCOPED_NAME_EXPR):
            qname.name = ejsToString(ejs, pop(ejs))->value;
            v1 = pop(ejs);
            if (ejsIsNamespace(v1)) {
                qname.space = ((EjsNamespace*) v1)->uri;
            } else {
                qname.space = ejsToString(ejs, v1)->value;
            }
            lookup.storing = 0;
            slotNum = ejsLookupScope(ejs, &qname, &lookup);
            if (slotNum < 0) {
                ejsThrowReferenceError(ejs, "Property \"%s\" does not exist", qname.name);
            } else {
                ejsDeleteProperty(ejs, lookup.obj, slotNum);
            }
            CHECK; BREAK;

        /*
         *  No operation. Does nothing.
         *      Nop
         */
        CASE (EJS_OP_NOP):
            BREAK;

        /*
         *  Check if object is a given type
         *      IsA, Like, InstanceOf
         *      Stack before (top)  [type]
         *                          [obj]
         *      Stack after         [boolean]
         */
        CASE (EJS_OP_INST_OF):
        CASE (EJS_OP_IS_A):
        CASE (EJS_OP_LIKE):
            type = (EjsType*) pop(ejs);
            v1 = pop(ejs);
            push(ejsCreateBoolean(ejs, ejsIsA(ejs, v1, type)));
            BREAK;

        /*
         *  Get the type of an object.
         *      TypeOf              <obj>
         *      Stack before (top)  [obj]
         *      Stack after         [string]
         */
        CASE (EJS_OP_TYPE_OF):
            v1 = pop(ejs);
            push(ejsGetTypeOf(ejs, v1));
            BREAK;

        /*
         *  Cast an object to the given the type. Throw if not castable.
         *      Cast
         *      Stack before (top)  [type]
         *                          [obj]
         *      Stack after         [result]
         */
        CASE (EJS_OP_CAST):
            type = (EjsType*) pop(ejs);
            if (!ejsIsType(type)) {
                ejsThrowTypeError(ejs, "Not a type");
            } else {
                v1 = pop(ejs);
                push(ejsCastVar(ejs, v1, type));
            }
            CHECK; BREAK;

        /*
         *  Cast to a boolean type
         *      CastBoolean
         *      Stack before (top)  [value]
         *      Stack after         [result]
         */
        CASE (EJS_OP_CAST_BOOLEAN):
            v1 = ejsCastVar(ejs, pop(ejs), ejs->booleanType);
            push(v1);
            CHECK; BREAK;

        /*
         *  Test if a given name is the name of a property "in" an object
         *      Cast
         *      Stack before (top)  [obj]
         *                          [name]
         *      Stack after         [result]
         */
        CASE (EJS_OP_IN):
            v1 = pop(ejs);
            nameVar = ejsToString(ejs, pop(ejs));
            if (nameVar == 0) {
                ejsThrowTypeError(ejs, "Can't convert to a name");
            } else {
                ejsName(&qname, "", nameVar->value);                        //  Don't consult namespaces
                slotNum = ejsLookupProperty(ejs, v1, &qname);
                if (slotNum < 0) {
                    slotNum = ejsLookupVar(ejs, v1, &qname, &lookup);
                    if (slotNum < 0 && ejsIsType(v1)) {
                        slotNum = ejsLookupVar(ejs, (EjsVar*) ((EjsType*) v1)->instanceBlock, &qname, &lookup);
                    }
                }
                push(ejsCreateBoolean(ejs, slotNum >= 0));
            }
            CHECK; BREAK;

        /*
         *  Unimplemented op codes
         */
        CASE (EJS_OP_BREAKPOINT):
            mprAssert(0);
            BREAK;

#if !BLD_UNIX_LIKE && !(VXWORKS && !BLD_CC_DIAB)
        }
        if (ejs->attention && !payAttention(ejs)) {
            goto done;
        }
    }
#endif
    
done:
    ejs->state = priorState;
}


static MPR_INLINE int storePropertyToSlot(Ejs *ejs, EjsObject *obj, int slotNum, EjsVar *value, EjsVar *thisObj)
{
    EjsFunction     *fun;

    if (ejsIsObject(obj) && slotNum < obj->numProp) {
        fun = (EjsFunction*) obj->slots[slotNum];
    } else {
        fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) obj, slotNum);
    }
    if (unlikely(fun && ((EjsVar*) fun)->isFunction && fun->getter && fun->nextSlot >= 0)) {
        handleSetters(ejs, fun, thisObj, value);
        if (unlikely(ejs->attention) && !payAttention(ejs)) {
            return 0;
        }
        return 1;
    }
    if (ejsIsObject(obj) && slotNum < obj->numProp) {
        obj->slots[slotNum] = value;
    } else {
        ejsSetProperty(ejs, (EjsVar*) obj, slotNum, (EjsVar*) value);
        if (unlikely(ejs->attention) && !payAttention(ejs)) {
            return 0;
        }
    }
    return 1;
}


/*
 *  Store a property by name in the given object. Will create if the property does not already exist.
 */
static void storeProperty(Ejs *ejs, EjsVar *obj, EjsName *qname, bool dup)
{
    EjsFunction     *fun;
    EjsLookup       lookup;
    EjsVar          *value, *origObj;
    int             slotNum;

    mprAssert(qname);
    mprAssert(qname->name);
    mprAssert(obj);

    slotNum = -1;
    origObj = obj;

    if (obj->type->helpers->setPropertyByName) {
        /*
         *  If a setPropertyByName helper is defined, defer to it. Allows types like XML to not offer slot based APIs.
         */
        ejs->result = value = popOutside(ejs);
        mprAssert(ejs->exception || ejs->result);
        slotNum = (*obj->type->helpers->setPropertyByName)(ejs, obj, qname, value);
        if (slotNum >= 0) {
            return;
        }
        pushOutside(ejs, value);
    }

    slotNum = ejsLookupVar(ejs, obj, qname, &lookup);
    if (slotNum >= 0) {
        obj = lookup.obj;
        /*
         *  Handle setters. Setters, if present, are chained off the getter.
         */
        if (obj->hasGetterSetter) {
            fun = (EjsFunction*) ejsGetProperty(ejs, obj, slotNum);
            if (ejsIsFunction(fun) && fun->getter && fun->nextSlot) {
                fun = (EjsFunction*) ejsGetProperty(ejs, fun->owner, fun->nextSlot);
                callFunction(ejs, fun, origObj, 1, 0);
                return;
            }
        }
        ejs->result = value = popOutside(ejs);
        mprAssert(ejs->exception || ejs->result);
        slotNum = ejsSetProperty(ejs, obj, slotNum, value);
        
    } else {
        ejs->result = value = popOutside(ejs);
        mprAssert(ejs->exception || ejs->result);
        slotNum = ejsSetProperty(ejs, obj, slotNum, value);
        if (slotNum >= 0) {
            if (dup) {
                qname->name = mprStrdup(obj, qname->name);
                qname->space = mprStrdup(obj, qname->space);
                obj->noPool = 1;
            }
            ejsSetPropertyName(ejs, obj, slotNum, qname);
        }
    }
}


/*
 *  Store a property by name in the scope chain. Will create properties if the given name does not already exist.
 */
static void storePropertyToScope(Ejs *ejs, EjsName *qname, bool dup)
{
    EjsFrame        *fp;
    EjsFunction     *fun;
    EjsVar          *value, *obj;
    EjsLookup       lookup;
    int             slotNum;

    mprAssert(qname);

    fp = ejs->state->fp;
    lookup.storing = 1;
    slotNum = ejsLookupScope(ejs, qname, &lookup);

    if (slotNum >= 0) {
        obj = lookup.obj;
        fun = (EjsFunction*) ejsGetProperty(ejs, obj, slotNum);
        if (fun && ejsIsFunction(fun) && fun->getter && fun->nextSlot) {
            fun = (EjsFunction*) ejsGetProperty(ejs, fun->owner, fun->nextSlot);
            callFunction(ejs, fun, obj, 1, 0);
            return;
        }
    } else if (fp->function.lang & EJS_SPEC_FIXED && fp->caller) {
        obj = (EjsVar*) fp;
    } else {
        obj = ejs->global;
    }
    value = popOutside(ejs);
    ejs->result = value;
    mprAssert(ejs->exception || ejs->result);
    slotNum = ejsSetProperty(ejs, obj, slotNum, value);
    if (slotNum >= 0) {
        if (dup) {
            qname->name = mprStrdup(obj, qname->name);
            qname->space = mprStrdup(obj, qname->space);
            obj->noPool = 1;
        }
        ejsSetPropertyName(ejs, obj, slotNum, qname);
    }
}


/*
 *  Attend to unusual circumstances. Memory allocation errors, exceptions and forced exits.
 */
static bool payAttention(Ejs *ejs)
{
    ejs->attention = 0;

    if (ejs->gcRequired) {
        ejsCollectGarbage(ejs, EJS_GEN_NEW);
    }
    if (mprHasAllocError(ejs)) {
        mprResetAllocError(ejs);
        ejsThrowMemoryError(ejs);
        ejs->attention = 1;
        return 0;
    }
    if (ejs->exception && !manageExceptions(ejs)) {
        ejs->attention = 1;
        return 0;
    }
    if (ejs->exiting || mprIsExiting(ejs)) {
        return 0;
    }
    return 1;
}


/*
 *  Run the module initializer
 */
EjsVar *ejsRunInitializer(Ejs *ejs, EjsModule *mp)
{
    EjsModule   *dp;
    EjsVar      *result;
    int         next;

    if (mp->initialized || !mp->hasInitializer) {
        mp->initialized = 1;
        return ejs->nullValue;
    }
    mp->initialized = 1;

    if (mp->dependencies) {
        for (next = 0; (dp = (EjsModule*) mprGetNextItem(mp->dependencies, &next)) != 0;) {
            if (dp->hasInitializer && !dp->initialized) {
                if (ejsRunInitializer(ejs, dp) == 0) {
                    mprAssert(ejs->exception);
                    return 0;
                }
            }
        }
    }
    mprLog(ejs, 6, "Running initializer for module %s", mp->name);
    result = ejsRunFunction(ejs, mp->initializer, ejs->global, 0, NULL);

    ejsMakeTransient(ejs, (EjsVar*) mp->initializer);
    mprAssert(ejs->exception || ejs->result);
    return result;
}


/*
 *  Run all initializers for all modules
 */
int ejsRun(Ejs *ejs)
{
    EjsModule   *mp;
    int         next;

    /*
     *  This is used by ejs to interpret scripts. OPT. Should not run through old modules every time
     */
    for (next = 0; (mp = (EjsModule*) mprGetNextItem(ejs->modules, &next)) != 0;) {
        if (mp->initialized) {
            continue;
        }
        if (ejsRunInitializer(ejs, mp) == 0) {
            mprAssert(ejs->exception);
            return EJS_ERR;
        }
    }
    return 0;
}


/*
 *  Run a function with the given parameters
 */
EjsVar *ejsRunFunction(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, EjsVar **argv)
{
    int         i;
    
    mprAssert(ejs);
    mprAssert(fun);
    mprAssert(ejsIsFunction(fun));

    if (thisObj == 0) {
        if ((thisObj = fun->thisObj) == 0) {
            thisObj = ejs->state->fp->function.thisObj;
        }
    }    

    if (ejsIsNativeFunction(fun)) {
        mprAssert(fun->body.proc);
        if (fun->body.proc == 0) {
            ejsThrowArgError(ejs, "Native function is not defined");
            return 0;
        }
        ejs->result = (fun->body.proc)(ejs, thisObj, argc, argv);
        if (ejs->result == 0) {
            ejs->result = ejs->nullValue;
        }
        mprAssert(ejs->exception || ejs->result);

    } else {
        mprAssert(fun->body.code.byteCode);
        mprAssert(fun->body.code.codeLen > 0);
        
        for (i = 0; i < argc; i++) {
            pushOutside(ejs, argv[i]);
        }
        VM(ejs, fun, thisObj, argc, 0);
        ejs->state->stack -= argc;
        if (ejs->exiting || mprIsExiting(ejs)) {
            ejs->attention = 1;
        }
    }
    return (ejs->exception) ? 0 : ejs->result;
}


/*
 *  Run a function by slot.
 */
EjsVar *ejsRunFunctionBySlot(Ejs *ejs, EjsVar *obj, int slotNum, int argc, EjsVar **argv)
{
    EjsFunction     *fun;

    if (obj == 0) {
        mprAssert(0);
        return 0;
    }

    if (obj == ejs->global) {
        fun = (EjsFunction*) ejsGetProperty(ejs, obj, slotNum);
    } else {
        fun = (EjsFunction*) ejsGetProperty(ejs, ejsIsType(obj) ? obj : (EjsVar*) obj->type, slotNum);
    }
    if (fun == 0) {
        return 0;
    }
    return ejsRunFunction(ejs, fun, obj, argc, argv);
}


/*
 *  Validate the args. This routine handles ...rest args and parameter type checking and casts. Returns the new argc 
 *  or < 0 on errors.
 */
static int validateArgs(Ejs *ejs, EjsFunction *fun, int argc, EjsVar **argv)
{
    EjsTrait        *trait;
    EjsVar          *newArg;
    EjsArray        *rest;
    int             nonDefault, i, limit, numRest;

    nonDefault = fun->numArgs - fun->numDefault;

    if (argc < nonDefault) {
        if (!fun->rest || argc != (fun->numArgs - 1)) {
            if (fun->lang < EJS_SPEC_FIXED) {
                /*
                 *  Create undefined values for missing args
                 */
                for (i = argc; i < nonDefault; i++) {
                    pushOutside(ejs, ejs->undefinedValue);
                }
                argc = nonDefault;

            } else {
                ejsThrowArgError(ejs, "Insufficient actual parameters. Call requires %d parameter(s).", nonDefault);
                return EJS_ERR;
            }
        }
    }

    if ((uint) argc > fun->numArgs && !fun->rest) {
        /*
         *  Discard excess arguments for scripted functions. No need to discard for native procs. This allows
         *  ejsDefineGlobalFunction to not have to bother with specifying the number of args for native procs.
         */
        if (!ejsIsNativeFunction(fun)) {
            ejs->state->stack -=  (argc - fun->numArgs);
            argc = fun->numArgs;
        }
    }

    /*
     *  Handle rest "..." args
     */
    if (fun->rest) {
        numRest = argc - fun->numArgs + 1;
        rest = ejsCreateArray(ejs, numRest);
        if (rest == 0) {
            return EJS_ERR;
        }
        for (i = numRest - 1; i >= 0; i--) {
            ejsSetProperty(ejs, (EjsVar*) rest, i, popOutside(ejs));
        }
        argc = argc - numRest + 1;
        pushOutside(ejs, rest);
    }

    if (fun->block.numTraits == 0) {
        return argc;
    }

    mprAssert((uint) fun->block.numTraits >= fun->numArgs);

    /*
     *  Cast args to the right types
     */
    limit = min((uint) argc, fun->numArgs);
    for (i = 0; i < limit; i++) {
        if ((trait = ejsGetTrait((EjsBlock*) fun, i)) == 0 || trait->type == 0) {
            continue;
        }
        if (argv[i] == ejs->nullValue && (trait->attributes & EJS_ATTR_NOT_NULLABLE)) {
            continue;
        }
        if (!ejsIsA(ejs, argv[i], trait->type)) {
            if (ejsIsNull(argv[i]) || ejsIsUndefined(argv[i])) {
                newArg = argv[i];
            } else {
                if (trait->type == ejs->stringType) {
                    /*
                     *  Do this to pickup toString overrides.
                     */
                    newArg = (EjsVar*) ejsToString(ejs, argv[i]);
                } else {
                    newArg = ejsCastVar(ejs, argv[i], trait->type);
                }
            }
            if (newArg == 0) {
                mprAssert(ejs->exception);
                return EJS_ERR;
            }
            argv[i] = newArg;
        }
    }

    return argc;
}


/*
 *  Call a function aka pushFunctionFrame. Supports both native and scripted functions. If native, the function is fully 
 *  invoked here. If scripted, a new frame is created and the pc adjusted to point to the new function.
 */
static void callConstructor(Ejs *ejs, EjsFunction *vp, int argc, int stackAdjust)
{
    EjsFunction     *fun;
    EjsType         *type;
    EjsVar          *obj;
    int             slotNum;

    mprAssert(!ejsIsFunction(vp));

    if ((EjsVar*) vp == (EjsVar*) ejs->undefinedValue) {
        ejsThrowReferenceError(ejs, "Function is undefined");
        return;

    } else if (ejsIsType(vp)) {
        /* 
         *  Handle calling a constructor to create a new instance 
         */
        type = (EjsType*) vp;
        obj = ejsCreateVar(ejs, type, 0);

        if (type->hasConstructor) {
            /*
             *  Constructor is always at slot 0 (offset by base properties)
             */
            slotNum = type->block.numInherited;
            fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) type, slotNum);

            if (ejsIsNativeFunction(fun)) {
                callFunction(ejs, fun, obj, argc, stackAdjust);
                mprAssert(ejs->result || ejs->exception);

            } else {
                VM(ejs, fun, obj, argc, stackAdjust);
                ejs->state->stack -= (argc + stackAdjust);
                if (ejs->exiting || mprIsExiting(ejs)) {
                    ejs->attention = 1;
                }
            }
        }
        ejs->result = obj;
        mprAssert(ejs->exception || ejs->result);

    } else {
        ejsThrowReferenceError(ejs, "Reference is not a function");
    }
}


/*
 *  Find the right base class to use as "this" for a static method
 */
static EjsVar *getStaticThis(Ejs *ejs, EjsType *type, int slotNum)
{
    while (type) {
        /*
         *  Use baseType->numTraits rather than numInherited because implemented traits are not accessed via the base type.
         */
        if (slotNum >= type->baseType->block.numTraits) {
            break;
        }
        type = type->baseType;
    }
    return (EjsVar*) type;
}


static void callInterfaceInitializers(Ejs *ejs, EjsType *type)
{
    EjsType     *iface;
    EjsFunction *fun;
    int         next, slotNum;

    for (next = 0; ((iface = mprGetNextItem(type->implements, &next)) != 0); ) {
        if (iface->hasStaticInitializer) {
            slotNum = iface->block.numInherited;
            if (iface->hasConstructor) {
                slotNum++;
            }
            fun = (EjsFunction*) ejsGetProperty(ejs, (EjsVar*) iface, slotNum);
            callFunction(ejs, fun, (EjsVar*) type, 0, 0);
        }
    }
}


/*
 *  Push a block. Used by the compiler.
 */
EjsBlock *ejsPushBlock(Ejs *ejs, EjsBlock *original)
{
    EjsBlock    *block;

    mprAssert(!ejsIsFunction(original));

    //  OPT. Need a faster way to do this. Should this copy slots? old code did. 
    block = ejsCopyBlock(ejs, original, 0);
    block->scopeChain = ejs->state->bp;
    block->prev = ejs->state->bp;
    block->stackBase = ejs->state->stack;
    ejs->state->bp = block;
    return block;
}


/*
 *  Pop a block frame and return to the previous frame.  This pops functions and/or lexical blocks.
 */
EjsBlock *ejsPopBlock(Ejs *ejs)
{
    EjsBlock    *bp;

    bp = ejs->state->bp;
    ejs->state->stack = bp->stackBase;
    return ejs->state->bp = bp->prev;
}


/*
 *  Pop an exception block.
 */
static EjsBlock *popExceptionBlock(Ejs *ejs)
{
    EjsBlock     *prev;

    if ((prev = ejs->state->bp->prev) != 0) {
        if (ejs->exception == 0) {
            ejs->exception = prev->prevException;
            prev->prevException = 0;
            if (ejs->exception) {
                ejs->attention = 1;
                /* Advance by one as checkExceptionHandlers expects the PC to be advanced after parsing the opcode */
                ejs->state->fp->pc++;
            }
        }
    }
    ejs->state->bp = prev;
    return prev;
}


/*
 *  Manage exceptions. Bubble up the exception until we find an exception handler for it.
 */
static bool manageExceptions(Ejs *ejs)
{
    EjsState        *state;

    state = ejs->state;

    /*
     *  Check at each function level for a handler to process the exception.
     */
    while (state->fp) {
        checkExceptionHandlers(ejs);
        if (ejs->exception == 0) {
            return 1;
        }
        state->stack = state->fp->stackReturn;
        state->bp = state->fp->function.block.prev;
        state->fp = state->fp->caller;
    }
    return 0;
}


static inline EjsEx *findExceptionHandler(Ejs *ejs, int kind)
{
    EjsEx       *ex;
    EjsFrame    *fp;
    EjsCode     *code;
    uint        pc;
    int         i;

    ex = 0;
    fp = ejs->state->fp;
    code = &fp->function.body.code;
    pc = (uint) (fp->pc - code->byteCode - 1);

    /*
     *  Exception handlers are sorted with the inner most handlers first.
     */
    for (i = 0; i < code->numHandlers; i++) {
        ex = code->handlers[i];
        if (ex->tryStart <= pc && pc < ex->handlerEnd && (ex->flags & kind)) {
            if (kind == EJS_EX_FINALLY || ex->catchType == ejs->voidType || 
                    ejsIsA(ejs, (EjsVar*) ejs->exception, ex->catchType)) {
                return ex;
            }
        }
    }
    return 0;
}

static inline EjsEx *inHandler(Ejs *ejs, int kind)
{
    EjsEx       *ex;
    EjsFrame    *fp;
    EjsCode     *code;
    uint        pc;
    int         i;
    
    ex = 0;
    fp = ejs->state->fp;
    code = &fp->function.body.code;
    pc = (uint) (fp->pc - code->byteCode - 1);
    
    /*
     *  Exception handlers are sorted with the inner most handlers first.
     */
    for (i = 0; i < code->numHandlers; i++) {
        ex = code->handlers[i];
        if (ex->handlerStart <= pc && pc < ex->handlerEnd && (ex->flags & kind)) {
            return ex;
        }
    }
    return 0;
}


/*
 *  Find the end of the last catch/finally handler.
 */
static inline uint findEndException(Ejs *ejs)
{
    EjsFrame    *fp;
    EjsEx       *ex;
    EjsCode     *code;
    uint        offset, pc;
    int         i;

    ex = 0;
    fp = ejs->state->fp;
    code = &fp->function.body.code;
    pc = (uint) (fp->pc - code->byteCode - 1);
    offset = 0;

    for (i = 0; i < code->numHandlers; i++) {
        ex = code->handlers[i];
        /*
         *  Comparison must include try and all catch handlers, incase there are multiple catch handlers
         */
        if (ex->tryStart <= pc && pc < ex->handlerEnd) {
            offset = ex->handlerEnd;
            for (++i; i < code->numHandlers; i++) {
                /* Find the last handler of this try block. Use tryEnd as nested try blocks can start at the same location */
                if (ex->tryEnd == code->handlers[i]->tryEnd) {
                    offset = code->handlers[i]->handlerEnd;
                }
            }
        }
    }
    mprAssert(offset);
    return offset;
}


/*
 *  Search for an exception handler at this level to process the exception. Return true if the exception is handled.
 */
static void checkExceptionHandlers(Ejs *ejs)
{
    EjsFrame        *fp;
    EjsCode         *code;
    EjsEx           *ex;
    uint            pc;

    ex = 0;
    fp = ejs->state->fp;
    code = &fp->function.body.code;

    if (code->numHandlers == 0) {
        return;
    }

    /*
     *  The PC is always one advanced from the throwing instruction. ie. the PC has advanced past the offending 
     *  instruction so reverse by one.
     */
    pc = (uint) (fp->pc - code->byteCode - 1);
    mprAssert(pc >= 0);

rescan:
    if (!fp->function.inException || (ejs->exception == (EjsVar*) ejs->stopIterationType)) {
        /*
         *  Normal exception in a try block. NOTE: the catch will jump or fall through to the finally block code.
         *  ie. We won't come here again for the finally code unless there is an exception in the catch block.
         *  Otherwise, No catch handler at this level and need to bubble up.
         */
        if ((ex = findExceptionHandler(ejs, EJS_EX_CATCH)) != 0) {
            createExceptionBlock(ejs, ex, ex->flags);
            return;
        }

    } else {
        /*
         *  Exception in a catch or finally block. If in a catch block, must first run the finally
         *  block before bubbling up. If in a finally block, we are done and upper levels will handle. We can be
         *  in a finally block and inException == 0. This happens because try blocks jump through directly
         *  into finally blocks (fast). But we need to check here if we are in the finally block explicitly.
         */
        if ((ex = inHandler(ejs, EJS_EX_FINALLY)) != 0) {
            /*
             *  If in a finally block, must advance the outer blocks's pc to be outside [tryStart .. finallyStart]
             *  This prevents this try block from handling this exception again.
             */
            fp->pc = &fp->function.body.code.byteCode[ex->handlerEnd + 1];
            fp->function.inCatch = fp->function.inException = 0;
            goto rescan;            
        }
    }

    /*
     *  Exception without a catch block or exception in a catch block. 
     */
    if ((ex = findExceptionHandler(ejs, EJS_EX_FINALLY)) != 0) {
        if (fp->function.inCatch) {
            popExceptionBlock(ejs);
        }
        createExceptionBlock(ejs, ex, EJS_EX_FINALLY);
    } else {
        fp->function.inCatch = fp->function.inException = 0;
    }
}


/*
 *  Called for catch and finally blocks
 */
static void createExceptionBlock(Ejs *ejs, EjsEx *ex, int flags)
{
    EjsBlock        *block;
    EjsFrame        *fp;
    EjsState        *state;
    int             i, count;

    state = ejs->state;
    fp = state->fp;
    mprAssert(ex);

    if (flags & EJS_EX_ITERATION) {
        /*
         *  Empty handler is a special case for iteration. We simply do a break to the handler location
         *  which targets the end of the for/in loop.
         */
        fp->pc = &fp->function.body.code.byteCode[ex->handlerStart];
        ejs->exception = 0;
        return;
    }

    /*
     *  Discard all lexical blocks defined inside the try block.
     *  Count the lexical blocks. We know how many there should be before the try block (ex->numBlocks). 
     */
    if (!fp->function.inCatch) {
        for (count = 0, block = state->bp; block != (EjsBlock*) state->fp; block = block->prev) {
            count++;
        }
        count -= ex->numBlocks;
        mprAssert(count >= 0);
        for (i = 0; i < count; i++) {
            ejsPopBlock(ejs);
        }
        count = (int) (state->stack - fp->stackBase);
        state->stack -= (count - ex->numStack);
    }
    
    /*
     *  Allocate a new frame in which to execute the handler
     */
    block = ejsCreateBlock(ejs, 0);
    ejsSetDebugName(block, "exception");
    if (block == 0) {
        /*  Exception will continue to bubble up */
        return;
    }
    block->prev = block->scopeChain = state->bp;
    block->stackBase = state->stack;
    state->bp = block;
    state->fp->function.block.referenced = 1;

    /*
     *  Move the PC outside of the try region. If this is a catch block, this allows the finally block to still
     *  be found. But if this is processing a finally block, the scanning for a matching handler will be forced
     *  to bubble up.
     */
    fp->pc = &fp->function.body.code.byteCode[ex->handlerStart];

    if (flags & EJS_EX_CATCH) {
        ejs->exceptionArg = ejs->exception;
        fp->function.inCatch = 1;

    } else {
        mprAssert(flags & EJS_EX_FINALLY);
        /*
         *  Mask the exception while processing the finally block
         */
        block->prev->prevException = ejs->exception;
        ejs->attention = 1;
        fp->function.inCatch = 0;
    }
    ejs->exception = 0;
    fp->function.inException = 1;
}


typedef struct OperMap {
    int         opcode;
    cchar       *name;
} OperMap;

static OperMap operMap[] = {
        { EJS_OP_MUL,           "*"     },
        { EJS_OP_DIV,           "/"     },
        { EJS_OP_REM,           "%"     },
        { EJS_OP_COMPARE_LT,    "<"     },
        { EJS_OP_COMPARE_GT,    ">"     },
        { EJS_OP_COMPARE_LE,    "<="    },
        { EJS_OP_COMPARE_GE,    ">="    },
        { 0,                    0       },
};


static int lookupOverloadedOperator(Ejs *ejs, EjsOpCode opcode, EjsVar *lhs)
{
    EjsName     qname;
    int         i;

    for (i = 0; operMap[i].opcode; i++) {
        if (operMap[i].opcode == opcode) {
            ejsName(&qname, "", operMap[i].name);
            break;
        }
    }
    return ejsLookupProperty(ejs, (EjsVar*) lhs->type, &qname);
}


/*
 *  Evaluate a binary expression.
 *  OPT -- simplify and move back inline into eval loop.
 */
static EjsVar *evalBinaryExpr(Ejs *ejs, EjsVar *lhs, EjsOpCode opcode, EjsVar *rhs)
{
    EjsVar      *result;
    int         slotNum;

    if (lhs == 0) {
        lhs = ejs->undefinedValue;
    }
    if (rhs == 0) {
        rhs = ejs->undefinedValue;
    }

    result = ejsInvokeOperator(ejs, lhs, opcode, rhs);

    if (result == 0 && ejs->exception == 0) {
        slotNum = lookupOverloadedOperator(ejs, opcode, lhs);
        if (slotNum >= 0) {
            result = ejsRunFunctionBySlot(ejs, lhs, slotNum, 1, &rhs);
        }
    }
    mprAssert(ejs->exception || result);
    return result;
}


/*
 *  Evaluate a unary expression.
 *  OPT -- once simplified, move back inline into eval loop.
 */
static EjsVar *evalUnaryExpr(Ejs *ejs, EjsVar *lhs, EjsOpCode opcode)
{
    EjsVar  *result;
    result = ejsInvokeOperator(ejs, lhs, opcode, 0);
    mprAssert(ejs->exception || result);
    return result;
}


int ejsInitStack(Ejs *ejs)
{
    EjsState    *state;

    state = ejs->state = ejs->masterState = mprAllocObjZeroed(ejs, EjsState);

    /*
     *  Allocate the stack
     */
    state->stackSize = MPR_PAGE_ALIGN(EJS_STACK_MAX, mprGetPageSize(ejs));

    /*
     *  This will allocate memory virtually for systems with virutal memory. Otherwise, it will just use malloc.
     */
    state->stackBase = mprMapAlloc(ejs, state->stackSize, MPR_MAP_READ | MPR_MAP_WRITE);
    if (state->stackBase == 0) {
        mprSetAllocError(ejs);
        return EJS_ERR;
    }
    state->stack = &state->stackBase[-1];
    return 0;
}


/*
 *  Exit the script
 */
void ejsExit(Ejs *ejs, int status)
{
    ejs->flags |= EJS_FLAG_EXIT;
}


static EjsName getNameArg(EjsFrame *fp)
{
    EjsName     qname;

    qname.name = getStringArg(fp);
    qname.space = getStringArg(fp);
    return qname;
}


/*
 *  Get an interned string. String constants are stored as token offsets into the constant pool. The pool
 *  contains null terminated UTF-8 strings.
 */
static char *getStringArg(EjsFrame *fp)
{
    int     number;

    number = (int) ejsDecodeNum(&fp->pc);

    mprAssert(fp->function.body.code.constants);
    return &fp->function.body.code.constants->pool[number];
}


static EjsVar *getGlobalArg(Ejs *ejs, EjsFrame *fp)
{
    EjsVar      *vp;
    EjsName     qname;
    int         t, slotNum;

    t = (int) ejsDecodeNum(&fp->pc);
    if (t < 0) {
        return 0;
    }

    slotNum = -1;
    qname.name = 0;
    qname.space = 0;
    vp = 0;

    /*
     *  OPT. Could this encoding be optimized?
     */
    switch (t & EJS_ENCODE_GLOBAL_MASK) {
    default:
        mprAssert(0);
        return 0;

    case EJS_ENCODE_GLOBAL_NOREF:
        return 0;

    case EJS_ENCODE_GLOBAL_SLOT:
        slotNum = t >> 2;
        if (0 <= slotNum && slotNum < ejsGetPropertyCount(ejs, ejs->global)) {
            vp = ejsGetProperty(ejs, ejs->global, slotNum);
        }
        break;

    case EJS_ENCODE_GLOBAL_NAME:
        qname.name = &fp->function.body.code.constants->pool[t >> 2];
        if (qname.name == 0) {
            mprAssert(0);
            return 0;
        }
        qname.space = getStringArg(fp);
        if (qname.space == 0) {
            return 0;
        }
        if (qname.name) {
            vp = ejsGetPropertyByName(ejs, ejs->global, &qname);
        }
        break;
    }
    return vp;
}


/*
 *  Get a function reference. This handles getters and binds the "this" value for method extraction.
 */
static void handleGetters(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj)
{
    if (fun->getter) {
        ejs->state->stack--;
        callFunction(ejs, fun, thisObj, 0, 0);
        if (ejsIsNativeFunction(fun)) {
            pushOutside(ejs, ejs->result);
        }

    } else if (!fun->thisObj && thisObj) {
        /*
         *  Function extraction. Bind the "thisObj" into a clone of the function
         */
        fun = ejsCopyFunction(ejs, fun);
        fun->thisObj = thisObj;
        ejs->state->stack--;
        pushOutside(ejs, fun);
    }
}


/*
 *  Handle setters. Setters, if present, are chained off the getter.
 */
static MPR_INLINE void handleSetters(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, EjsVar *value)
{
    /*
     *  Step from the getter to retrieve the setter
     */
    fun = (EjsFunction*) ejsGetProperty(ejs, fun->owner, fun->nextSlot);
    pushOutside(ejs, value);
    callFunction(ejs, fun, thisObj, 1, 0);
}


static void callProperty(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, int stackAdjust)
{
    if (ejsIsFunction(fun) && fun->getter) {
        fun = (EjsFunction*) ejsRunFunction(ejs, fun, thisObj, 0, NULL);
    }
    callFunction(ejs, fun, thisObj, argc, stackAdjust);
}


/*
 *  Call a function aka pushFunctionFrame. Supports both native and scripted functions. If native, the function is fully 
 *  invoked here. If scripted, a new frame is created and the pc adjusted to point to the new function.
 */
static void callFunction(Ejs *ejs, EjsFunction *fun, EjsVar *thisObj, int argc, int stackAdjust)
{
    EjsState        *state;
    EjsFrame        *fp;
    EjsName         qname;
    EjsVar          **argv;

    mprAssert(fun);

    state = ejs->state;

    if (unlikely(!ejsIsFunction(fun))) {
        callConstructor(ejs, fun, argc, stackAdjust);
        return;
    }
    if (thisObj == 0) {
        if ((thisObj = fun->thisObj) == 0) {
            thisObj = state->fp->function.thisObj;
        } 
    } 
    if (fun->staticMethod && !ejsIsType(thisObj)) {
        /*
         *  Calling a static method via an instance object
         */
        thisObj = getStaticThis(ejs, thisObj->type, fun->slotNum);
    }

    /*
     *  Validate the args. Cast to the right type, handle rest args and return with argc adjusted.
     */
    argv = NULL;
    if (argc > 0 || fun->numArgs || fun->rest) {
        argv = &(state->stack[1 - argc]);
        if ((argc = validateArgs(ejs, fun, argc, argv)) < 0) {
            return;
        }
    }

    if (ejsIsNativeFunction(fun)) {
        if (fun->body.proc == 0) {
            qname = ejsGetPropertyName(ejs, fun->owner, fun->slotNum);
            ejsThrowInternalError(ejs, "Native function \"%s\" is not defined", qname.name);
            return;
        }
        ejs->result = (fun->body.proc)(ejs, thisObj, argc, argv);
        if (ejs->result == 0) {
            ejs->result = ejs->nullValue;
        }
        mprAssert(ejs->exception || ejs->result);
        state->stack -= (argc + stackAdjust);

    } else {
        fp = ejsCreateFrame(ejs, fun);
        fp->function.block.prev = state->bp;
        fp->function.thisObj = thisObj;
        fp->caller = state->fp;
        fp->stackBase = state->stack;
        fp->stackReturn = state->stack - argc - stackAdjust;
        if (argc > 0) {
            fp->argc = argc;
            if ((uint) argc < (fun->numArgs - fun->numDefault) || (uint) argc > fun->numArgs) {
                ejsThrowArgError(ejs, "Incorrect number of arguments");
                return;
            }
            memcpy(fp->function.block.obj.slots, argv, argc * sizeof(EjsVar*));
        }
        state->fp = fp;
        state->bp = (EjsBlock*) fp;
    }
}


static void throwNull(Ejs *ejs)
{
    ejsThrowReferenceError(ejs, "Object reference is null");
}


/*
 *  Object can be an instance or a type. If an instance, then step to the immediate base type to begin the count.
 */
static EjsVar *getNthBase(Ejs *ejs, EjsVar *obj, int nthBase)
{
    EjsType     *type;

    if (obj) {
        if (ejsIsType(obj)) {
            type = (EjsType*) obj;
        } else {
            type = obj->type;
            nthBase--;
        }
        for (; type && nthBase > 0; type = type->baseType) {
            nthBase--;
        }
        if (nthBase > 0) {
            ejsThrowReferenceError(ejs, "Can't find correct base class");
            return 0;
        }
        obj = (EjsVar*) type;
    }
    return obj;
}


static EjsVar *getNthBaseFromBottom(Ejs *ejs, EjsVar *obj, int nthBase)
{
    EjsType     *type, *tp;
    int         count;

    if (obj) {
        if (ejsIsType(obj) || obj == ejs->global) {
            type = (EjsType*) obj;
        } else {
            type = obj->type;
        }
        for (count = 0, tp = type->baseType; tp; tp = tp->baseType) {
            count++;
        }
        nthBase = count - nthBase;
        for (; type && nthBase > 0; type = type->baseType) {
            nthBase--;
        }
        obj = (EjsVar*) type;
    }
    return obj;
}


static MPR_INLINE EjsVar *getNthBlock(Ejs *ejs, int nth)
{
    EjsBlock    *block;

    mprAssert(ejs);
    mprAssert(nth >= 0);

    for (block = ejs->state->bp; block && --nth >= 0; ) {
        if (block->obj.var.hidden) nth++;
        block = block->scopeChain;
    }
    return (EjsVar*) block;
}


/*
 *  Enter a mesage into the log file
 */
void ejsLog(Ejs *ejs, const char *fmt, ...)
{
    va_list     args;
    char        buf[MPR_MAX_LOG_STRING];

    va_start(args, fmt);
    mprVsprintf(buf, sizeof(buf) - 1, fmt, args);
    va_end(args);

    mprLog(ejs, 0, "%s", buf);
}


void ejsShowStack(Ejs *ejs, EjsFunction *fp)
{
    char    *stack;
    
    stack = ejsFormatStack(ejs, NULL);
    mprLog(ejs, 7, "Stack\n%s", stack);
    mprFree(stack);
}


#if BLD_DEBUG

int ejsOpCount = 0;
static EjsOpCode traceCode(Ejs *ejs, EjsOpCode opcode)
{
    EjsFrame        *fp;
    EjsState        *state;
    EjsOptable      *optable;
    int             len;
    static int      once = 0;
    static int      stop = 1;

    state = ejs->state;
    fp = state->fp;
    opcount[opcode]++;

    if (ejs->initialized && opcode != EJS_OP_DEBUG) {
        //  OPT - should strip '\n' in the compiler
        if (fp->currentLine) {
            len = (int) strlen(fp->currentLine) - 1;
            if (fp->currentLine[len] == '\n') {
                ((char*) fp->currentLine)[len] = '\0';
            }
        }
        optable = ejsGetOptable(ejs);
        mprLog(ejs, 7, "%04d: [%d] %02x: %-35s # %s:%d %s",
            (uint) (fp->pc - fp->function.body.code.byteCode) - 1, (int) (state->stack - fp->stackReturn),
            (uchar) opcode, optable[opcode].name, fp->filename, fp->lineNumber, fp->currentLine);
        if (stop && once++ == 0) {
            mprSleep(ejs, 0);
        }
    }
    ejsOpCount++;
    return opcode;
}
#endif

#undef top

/*
 *  @copy   default
 *
 *  Copyright (c) Embedthis Software LLC, 2003-2011. All Rights Reserved.
 *  Copyright (c) Michael O'Brien, 1993-2011. All Rights Reserved.
 *
 *  This software is distributed under commercial and open source licenses.
 *  You may use the GPL open source license described below or you may acquire
 *  a commercial license from Embedthis Software. You agree to be fully bound
 *  by the terms of either license. Consult the LICENSE.TXT distributed with
 *  this software for full details.
 *
 *  This software is open source; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the
 *  Free Software Foundation; either version 2 of the License, or (at your
 *  option) any later version. See the GNU General Public License for more
 *  details at: http://www.embedthis.com/downloads/gplLicense.html
 *
 *  This program is distributed WITHOUT ANY WARRANTY; without even the
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  This GPL license does NOT permit incorporating this software into
 *  proprietary programs. If you are unable to comply with the GPL, you must
 *  acquire a commercial license to use this software. Commercial licenses
 *  for this software and support services are available from Embedthis
 *  Software at http://www.embedthis.com
 *
 *  Local variables:
    tab-width: 4
    c-basic-offset: 4
    End:
    vim: sw=4 ts=4 expandtab

    @end
 */
