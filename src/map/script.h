/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/map/script.h                                                     *
* Descri��o Prim�ria.                                                        *
* Descri��o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#ifndef _SCRIPT_H_
#define _SCRIPT_H_

#include "map.h" //EVENT_NAME_LENGTH

#define NUM_WHISPER_VAR 10

struct map_session_data;
struct eri;

extern int potion_flag; //For use on Alchemist improved potions/Potion Pitcher. [Skotlex]
extern int potion_hp, potion_per_hp, potion_sp, potion_per_sp;
extern int potion_target;

extern struct Script_Config {
	unsigned warn_func_mismatch_argtypes : 1;
	unsigned warn_func_mismatch_paramnum : 1;
	int check_cmdcount;
	int check_gotocount;
	int input_min_value;
	int input_max_value;

	const char *die_event_name;
	const char *kill_pc_event_name;
	const char *kill_mob_event_name;
	const char *login_event_name;
	const char *logout_event_name;
	const char *loadmap_event_name;
	const char *baselvup_event_name;
	const char *joblvup_event_name;

	const char *ontouch_name;
	const char *ontouch2_name;
} script_config;

typedef enum c_op {
    C_NOP, // end of script/no value (nil)
    C_POS,
    C_INT, // number
    C_PARAM, // parameter variable (see pc_readparam/pc_setparam)
    C_FUNC, // buildin function call
    C_STR, // string (free'd automatically)
    C_CONSTSTR, // string (not free'd)
    C_ARG, // start of argument list
    C_NAME,
    C_EOL, // end of line (extra stack values are cleared)
    C_RETINFO,
    C_USERFUNC, // internal script function
    C_USERFUNC_POS, // internal script function label
    C_REF, // the next call to c_op2 should push back a ref to the left operand

    // operators
    C_OP3, // a ? b : c
    C_LOR, // a || b
    C_LAND, // a && b
    C_LE, // a <= b
    C_LT, // a < b
    C_GE, // a >= b
    C_GT, // a > b
    C_EQ, // a == b
    C_NE, // a != b
    C_XOR, // a ^ b
    C_OR, // a | b
    C_AND, // a & b
    C_ADD, // a + b
    C_SUB, // a - b
    C_MUL, // a * b
    C_DIV, // a / b
    C_MOD, // a % b
    C_NEG, // - a
    C_LNOT, // ! a
    C_NOT, // ~ a
    C_R_SHIFT, // a >> b
    C_L_SHIFT, // a << b
    C_ADD_PP, // ++a
    C_SUB_PP, // --a
} c_op;

struct script_retinfo {
	struct DBMap *var_function;// scope variables
	struct script_code *script;// script code
	int pos;// script location
	int nargs;// argument count
	int defsp;// default stack pointer
};

struct script_data {
	enum c_op type;
	union script_data_val {
		int num;
		char *str;
		struct script_retinfo *ri;
	} u;
	struct DBMap **ref;
};

// Moved defsp from script_state to script_stack since
// it must be saved when script state is RERUNLINE. [Eoe / jA 1094]
struct script_code {
	int script_size;
	unsigned char *script_buf;
	struct DBMap *script_vars;
};

struct script_stack {
	int sp;// number of entries in the stack
	int sp_max;// capacity of the stack
	int defsp;
	struct script_data *stack_data;// stack
	struct DBMap *var_function;// scope variables
};

enum hQueueOpt {
	HQO_NONE,
	HQO_onLogOut,
	HQO_OnDeath,
	HQO_OnMapChange,
	HQO_MAX,
};

/* [Ind/Hercules] */
struct hQueue {
	int id;
	int *item;
	int items;/* how many actual items are in the array */
	int size;/* size of the *item array, not the current amount of items in it since it can have empty slots */
	/* events */
	char onLogOut[EVENT_NAME_LENGTH];
	char onDeath[EVENT_NAME_LENGTH];
	char onMapChange[EVENT_NAME_LENGTH];
};

struct hQueueIterator {
	int *item;
	int items;
	int pos;
};

//
// Script state
//
enum e_script_state { RUN,STOP,END,RERUNLINE,GOTO,RETFUNC,CLOSE };

struct script_state {
	struct script_stack *stack;
	int start,end;
	int pos;
	enum e_script_state state;
	int rid,oid;
	struct script_code *script, *scriptroot;
	struct sleep_data {
		int tick,timer,charid;
	} sleep;
	int instance_id;
	//For backing up purposes
	struct script_state *bk_st;
	unsigned char hIterator;
	int bk_npcid;
	unsigned freeloop : 1;// used by buildin_freeloop
	unsigned op2ref : 1;// used by op_2
	unsigned npc_item_flag : 1;
	unsigned mes_active : 1;  // Store if invoking character has a NPC dialog box open.
	unsigned int id;
};

struct script_reg {
	int index;
	int data;
};

struct script_regstr {
	int index;
	char *data;
};

enum script_parse_options {
    SCRIPT_USE_LABEL_DB = 0x1,// records labels in scriptlabel_db
    SCRIPT_IGNORE_EXTERNAL_BRACKETS = 0x2,// ignores the check for {} brackets around the script
    SCRIPT_RETURN_EMPTY_SCRIPT = 0x4// returns the script object instead of NULL for empty scripts
};

const char *skip_space(const char *p);
void script_error(const char *src, const char *file, int start_line, const char *error_msg, const char *error_pos);

struct script_code *parse_script(const char *src,const char *file,int line,int options);
void run_script_sub(struct script_code *rootscript,int pos,int rid,int oid, char *file, int lineno);
void run_script(struct script_code *,int,int,int);

int set_var(struct map_session_data *sd, char *name, void *val);
int conv_num(struct script_state *st,struct script_data *data);
const char *conv_str(struct script_state *st,struct script_data *data);
int run_script_timer(int tid, unsigned int tick, int id, intptr_t data);
void run_script_main(struct script_state *st);

void script_stop_instances(struct script_code *code);
struct linkdb_node *script_erase_sleepdb(struct linkdb_node *n);
void script_free_code(struct script_code *code);
void script_free_vars(struct DBMap *storage);
struct script_state *script_alloc_state(struct script_code *rootscript, int pos, int rid, int oid);
void script_free_state(struct script_state *st);

struct DBMap *script_get_userfunc_db(void);
void script_run_autobonus(const char *autobonus,int id, int pos);

bool script_get_constant(const char *name, int *value);
void script_set_constant(const char *name, int value, bool isparameter);

void script_detach_rid(struct script_state *st);

void script_cleararray_pc(struct map_session_data *sd, const char *varname, void *value);
void script_setarray_pc(struct map_session_data *sd, const char *varname, uint8 idx, void *value, int *refcache);

int script_config_read(char *cfgName);
void do_init_script(void);
int do_final_script(void);
int add_str(const char *p);
const char *get_str(int id);
int script_reload(void);

// @commands (script based)
void setd_sub(struct script_state *st, TBL_PC *sd, const char *varname, int elem, void *value, struct DBMap **ref);

TBL_PC *script_rid2sd(struct script_state *st);

#ifdef BETA_THREAD_TEST
void queryThread_log(char *entry, int length);
#endif
struct script_label_entry {
	int key,pos;
};

/* script.c interface (incomplete) */
struct script_interface {
	/* */
	DBMap *st_db;
	unsigned int active_scripts;
	unsigned int next_id;
	struct eri *st_ers;
	struct eri *stack_ers;
	char **buildin;
	unsigned int buildin_count;
	/* */
	struct hQueue *hq;
	struct hQueueIterator *hqi;
	int hqs, hqis;
	int hqe[HQO_MAX];
	/* */
	char *word_buf;
	int word_size;
	/* */
	unsigned short current_item_id;
	/* */
	struct script_label_entry *labels;
	int label_count;
	int labels_size;
	/*  */
	void (*set_constant2) (const char *name, int value, bool isparameter);
	void (*set_constant_force) (const char *name, int value, bool isparameter);
	void (*label_add)(int key, int pos);
	/* */
	struct hQueue *(*queue) (int idx);
	bool (*queue_add) (int idx, int var);
	bool (*queue_del) (int idx);
	bool (*queue_remove) (int idx, int var);
	int (*queue_create) (void);
	void (*queue_clear) (int idx);
};

struct script_interface *script;

void script_defaults(void);

#endif /* _SCRIPT_H_ */
