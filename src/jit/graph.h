/* The MVMJitGraph is - for now - really a linked list of instructions.
 * It's likely I'll add complexity when it's needed */
struct MVMJitGraph {
    MVMSpeshGraph      *sg;
    MVMJitNode *first_node;
    MVMJitNode  *last_node;

    MVMint32    num_labels;
    MVMJitLabel    *labels;
    MVMuint8        in_osr;
};

/* A label */
struct MVMJitLabel {
    MVMint32 name;
    MVMSpeshBB *bb;
};

struct MVMJitPrimitive {
    MVMSpeshIns * ins;
};

struct MVMJitGuard {
    MVMSpeshIns * ins;
    MVMint32      deopt_target;
    MVMint32      deopt_offset;
};

typedef enum {
    MVM_JIT_CONTROL_INVOKISH,
    MVM_JIT_CONTROL_OSRLABEL,
    MVM_JIT_CONTROL_BREAKPOINT,
} MVMJitControlType;

struct MVMJitControl {
    MVMSpeshIns       *ins;
    MVMJitControlType type;
};

/* Special branch target for the exit */
#define MVM_JIT_BRANCH_EXIT -1
#define MVM_JIT_BRANCH_OUT  -2

/* What does a branch need? a label to go to, an instruction to read */
struct MVMJitBranch {
    MVMJitLabel dest;
    MVMSpeshIns *ins;
};

typedef enum {
    MVM_JIT_INTERP_TC,
    MVM_JIT_INTERP_FRAME,
    MVM_JIT_INTERP_CU,
    MVM_JIT_INTERP_PARAMS,
} MVMJitInterpVar;

typedef enum {
    MVM_JIT_INTERP_VAR,
    MVM_JIT_REG_VAL,
    MVM_JIT_REG_VAL_F,
    MVM_JIT_REG_ADDR,
    MVM_JIT_STR_IDX,
    MVM_JIT_LITERAL,
    MVM_JIT_LITERAL_F,
    MVM_JIT_LITERAL_64,
    MVM_JIT_LITERAL_PTR,
} MVMJitArgType;

struct MVMJitCallArg {
    MVMJitArgType type;
    union {
        MVMint64      lit_i64;
        MVMnum64      lit_n64;
        MVMJitInterpVar  ivar;
        MVMint16          reg;
        void             *ptr;
    } v;
};


typedef enum {
    MVM_JIT_RV_VOID,
    /* ptr and int are mostly the same, but they might not be on all
       platforms */
    MVM_JIT_RV_INT, 
    MVM_JIT_RV_PTR,
    /* floats aren't */
    MVM_JIT_RV_NUM,
    /* dereference and store */
    MVM_JIT_RV_DEREF,
    /* store local at address */
    MVM_JIT_RV_ADDR
} MVMJitRVMode;


struct MVMJitCallC {
    void       *func_ptr; 
    MVMJitCallArg  *args;
    MVMuint16   num_args;
    MVMuint16  has_vargs;
    MVMJitRVMode rv_mode;
    MVMint16      rv_idx;
};

struct MVMJitInvoke {
    MVMint16      callsite_idx;
    MVMint16      arg_count;
    MVMSpeshIns **arg_ins;
    MVMReturnType return_type;
    MVMint16      return_register;
    MVMint16      code_register;
    MVMint16      spesh_cand;
    MVMint8       is_fast;
    MVMint32      reentry_label;
};

/* Node types */
typedef enum {
    MVM_JIT_NODE_PRIMITIVE,
    MVM_JIT_NODE_CALL_C,
    MVM_JIT_NODE_BRANCH,
    MVM_JIT_NODE_LABEL,
    MVM_JIT_NODE_GUARD,
    MVM_JIT_NODE_INVOKE,
    MVM_JIT_NODE_CONTROL,
} MVMJitNodeType;

struct MVMJitNode {
    MVMJitNode   * next; // linked list
    MVMJitNodeType type; // tag
    union {
        MVMJitPrimitive prim;
        MVMJitCallC     call;
        MVMJitBranch    branch;
        MVMJitLabel     label;
        MVMJitGuard     guard;
        MVMJitInvoke    invoke;
        MVMJitControl   control;
    } u;
};

MVMJitGraph* MVM_jit_try_make_graph(MVMThreadContext *tc, MVMSpeshGraph *sg);
