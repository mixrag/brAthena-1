/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/map/itemdb.h                                                     *
* Descri��o Prim�ria.                                                        *
* Descri��o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena, eAthena                                          *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#ifndef _ITEMDB_H_
#define _ITEMDB_H_

#include "../common/db.h"
#include "../common/mmo.h" // ITEM_NAME_LENGTH
#include "map.h"

/**
 * Declarations
 **/
struct item_package;

/**
 * Defines
 **/
#define MAX_ITEMDB 0x8000 // 32k array entries in array (the rest goes to the db)
#define MAX_ITEMDELAYS 10 // The maximum number of item delays
#define MAX_SEARCH 5 //Designed for search functions, species max number of matches to display.
#define MAX_ITEMS_PER_COMBO 6 /* maximum amount of items a combo may require */
#define IG_FINDINGORE 6
#define IG_POTION 37
//The max. item group count (increase this when needed).
#define MAX_ITEMGROUP 140
#define MAX_RANDITEM    20000

#define CARD0_FORGE 0x00FF
#define CARD0_CREATE 0x00FE
#define CARD0_PET ((short)0xFF00)

//Marks if the card0 given is "special" (non-item id used to mark pets/created items. [Skotlex]
#define itemdb_isspecial(i) (i == CARD0_FORGE || i == CARD0_CREATE || i == CARD0_PET)

//Use apple for unknown items.
#define UNKNOWN_ITEM_ID 512

enum item_itemid {
    ITEMID_HOLY_WATER = 523,
    ITEMID_EMPERIUM = 714,
    ITEMID_YELLOW_GEMSTONE = 715,
    ITEMID_RED_GEMSTONE = 716,
    ITEMID_BLUE_GEMSTONE = 717,
    ITEMID_TRAP = 1065,
    ITEMID_STRANGE_EMBRYO = 6415,
    ITEMID_FACE_PAINT = 6120,
    ITEMID_STONE = 7049,
    ITEMID_SKULL_ = 7420,
    ITEMID_TOKEN_OF_SIEGFRIED = 7621,
    ITEMID_TRAP_ALLOY = 7940,
    ITEMID_ANCILLA = 12333,
    ITEMID_REINS_OF_MOUNT = 12622,
    ITEMID_LOVE_ANGEL = 12287,
    ITEMID_SQUIRREL = 12288,
    ITEMID_GOGO = 12289,
    ITEMID_PICTURE_DIARY = 12304,
    ITEMID_MINI_HEART = 12305,
    ITEMID_NEWCOMER = 12306,
    ITEMID_KID = 12307,
    ITEMID_MAGIC_CASTLE = 12308,
    ITEMID_BULGING_HEAD = 12309,
};

/**
 * Rune Knight
 **/

enum {
    ITEMID_NAUTHIZ = 12725,
    ITEMID_RAIDO,
    ITEMID_BERKANA,
    ITEMID_ISA,
    ITEMID_OTHILA,
    ITEMID_URUZ,
    ITEMID_THURISAZ,
    ITEMID_WYRD,
    ITEMID_HAGALAZ,
    ITEMID_LUX_ANIMA = 22540,
} rune_list;

/**
 * Mechanic
 **/
enum {
    ITEMID_ACCELERATOR = 2800,
    ITEMID_HOVERING_BOOSTER,
    ITEMID_SUICIDAL_DEVICE,
    ITEMID_SHAPE_SHIFTER,
    ITEMID_COOLING_DEVICE,
    ITEMID_MAGNETIC_FIELD_GENERATOR,
    ITEMID_BARRIER_BUILDER,
    ITEMID_REPAIR_KIT,
    ITEMID_CAMOUFLAGE_GENERATOR,
    ITEMID_HIGH_QUALITY_COOLER,
    ITEMID_SPECIAL_COOLER,
    ITEMID_MONKEY_SPANNER = 6186,
} mecha_item_list;

enum {
	NOUSE_SITTING = 0x01,
} item_nouse_list;

//
struct item_data {
	uint16 nameid;
	char name[ITEM_NAME_LENGTH],jname[ITEM_NAME_LENGTH];

	//Do not add stuff between value_buy and view_id (see how getiteminfo works)
	int value_buy;
	int value_sell;
	int type;
	int maxchance; //For logs, for external game info, for scripts: Max drop chance of this item (e.g. 0.01% , etc.. if it = 0, then monsters don't drop it, -1 denotes items sold in shops only) [Lupus]
	int sex;
	int equip;
	int weight;
	int atk;
	int def;
	int range;
	int slot;
	int look;
	int elv;
	int wlv;
	int view_id;
#if VERSION == 1
	int matk;
	int elvmax;/* maximum level for this item */
#endif

	int delay;
//Lupus: I rearranged order of these fields due to compatibility with ITEMINFO script command
//		some script commands should be revised as well...
	unsigned int class_base[3]; //Specifies if the base can wear this item (split in 3 indexes per type: 1-1, 2-1, 2-2)
	unsigned class_upper : 6; //Specifies if the upper-type can equip it (bitfield, 1: normal, 2: upper, 3: baby,4:third)
	struct {
		unsigned short chance;
		int id;
	} mob[MAX_SEARCH]; //Holds the mobs that have the highest drop rate for this item. [Skotlex]
	struct script_code *script; //Default script for everything.
	struct script_code *equip_script;   //Script executed once when equipping.
	struct script_code *unequip_script;//Script executed once when unequipping.
	struct {
		unsigned available : 1;
		unsigned no_refine : 1; // [celest]
		unsigned delay_consume : 1; // Signifies items that are not consumed immediately upon double-click [Skotlex]
		unsigned trade_restriction : 9; //Item restrictions mask [Skotlex]
		unsigned autoequip: 1;
		unsigned buyingstore : 1;
	} flag;
	struct {// item stacking limitation
		unsigned short amount;
		unsigned int inventory:1;
		unsigned int cart:1;
		unsigned int storage:1;
		unsigned int guildstorage:1;
	} stack;
	struct {// used by item_nouse.txt
		unsigned int flag;
		unsigned short override;
	} item_usage;
	short gm_lv_trade_override; //GM-level to override trade_restriction
	/* bugreport:309 */
	struct item_combo **combos;
	unsigned char combos_count;
	/* TODO add a pointer to some sort of (struct extra) and gather all the not-common vals into it to save memory */
	struct item_package *package;
};

struct item_group {
	int nameid[MAX_RANDITEM];
	int qty; //Counts amount of items in the group.
};

struct item_combo {
	struct script_code *script;
	unsigned short *nameid;/* nameid array */
	unsigned char count;
	unsigned short id;/* id of this combo */
	bool isRef;/* whether this struct is a reference or the master */
};

struct item_group itemgroup_db[MAX_ITEMGROUP];

struct item_package_rand_entry {
	unsigned short id;
	unsigned short qty;
	unsigned short probability;
	unsigned short hour;
	unsigned int onair : 1;
	unsigned int guid : 1;
	struct item_package_rand_entry *next;
};

struct item_package_must_entry {
	unsigned short id;
	unsigned short qty;
	unsigned short hour;
	unsigned int onair : 1;
	unsigned int guid : 1;
};

struct item_package_rand_group {
	struct item_package_rand_entry *random_list;
	unsigned short random_qty;
};

struct item_package {
	unsigned short id;
	struct item_package_rand_group *random_groups;
	struct item_package_must_entry *must_items;
	unsigned short random_qty;
	unsigned short must_qty;
};

struct item_data *itemdb_searchname(const char *name);
int itemdb_searchname_array(struct item_data **data, int size, const char *str, int flag);
struct item_data *itemdb_load(int nameid);
struct item_data *itemdb_search(int nameid);
struct item_data *itemdb_exists(int nameid);
#define itemdb_name(n) itemdb_search(n)->name
#define itemdb_jname(n) itemdb_search(n)->jname
#define itemdb_type(n) itemdb_search(n)->type
#define itemdb_atk(n) itemdb_search(n)->atk
#define itemdb_def(n) itemdb_search(n)->def
#define itemdb_look(n) itemdb_search(n)->look
#define itemdb_weight(n) itemdb_search(n)->weight
#define itemdb_equip(n) itemdb_search(n)->equip
#define itemdb_usescript(n) itemdb_search(n)->script
#define itemdb_equipscript(n) itemdb_search(n)->script
#define itemdb_wlv(n) itemdb_search(n)->wlv
#define itemdb_range(n) itemdb_search(n)->range
#define itemdb_slot(n) itemdb_search(n)->slot
#define itemdb_available(n) (itemdb_search(n)->flag.available)
#define itemdb_traderight(n) (itemdb_search(n)->flag.trade_restriction)
#define itemdb_viewid(n) (itemdb_search(n)->view_id)
#define itemdb_autoequip(n) (itemdb_search(n)->flag.autoequip)
#define itemdb_is_rune(n) ((n >= ITEMID_NAUTHIZ && n <= ITEMID_HAGALAZ) || n == ITEMID_LUX_ANIMA)
#define itemdb_is_element(n) (n >= 990 && n <= 993)
#define itemdb_is_spellbook(n) (n >= 6188 && n <= 6205)
#define itemdb_is_poison(n) (n >= 12717 && n <= 12724)
#define itemid_isgemstone(id) ( (id) >= ITEMID_YELLOW_GEMSTONE && (id) <= ITEMID_BLUE_GEMSTONE )
#define itemdb_iscashfood(id) ( (id) >= 12202 && (id) <= 12207 )
#define itemdb_is_GNbomb(n) (n >= 13260 && n <= 13267)
#define itemdb_is_GNthrowable(n) (n >= 13268 && n <= 13290)
const char *itemdb_typename(int type);

int itemdb_group_bonus(struct map_session_data *sd, int itemid);
int itemdb_searchrandomid(int flags);

#define itemdb_value_buy(n) itemdb_search(n)->value_buy
#define itemdb_value_sell(n) itemdb_search(n)->value_sell
#define itemdb_canrefine(n) (!itemdb_search(n)->flag.no_refine)
//Item trade restrictions [Skotlex]
int itemdb_isdropable_sub(struct item_data *, int, int);
int itemdb_cantrade_sub(struct item_data *, int, int);
int itemdb_canpartnertrade_sub(struct item_data *, int, int);
int itemdb_cansell_sub(struct item_data *,int, int);
int itemdb_cancartstore_sub(struct item_data *, int, int);
int itemdb_canstore_sub(struct item_data *, int, int);
int itemdb_canguildstore_sub(struct item_data *, int, int);
int itemdb_canmail_sub(struct item_data *, int, int);
int itemdb_canauction_sub(struct item_data *, int, int);
int itemdb_isrestricted(struct item *item, int gmlv, int gmlv2, int (*func)(struct item_data *, int, int));
#define itemdb_isdropable(item, gmlv) itemdb_isrestricted(item, gmlv, 0, itemdb_isdropable_sub)
#define itemdb_cantrade(item, gmlv, gmlv2) itemdb_isrestricted(item, gmlv, gmlv2, itemdb_cantrade_sub)
#define itemdb_canpartnertrade(item, gmlv, gmlv2) itemdb_isrestricted(item, gmlv, gmlv2, itemdb_canpartnertrade_sub)
#define itemdb_cansell(item, gmlv) itemdb_isrestricted(item, gmlv, 0, itemdb_cansell_sub)
#define itemdb_cancartstore(item, gmlv)  itemdb_isrestricted(item, gmlv, 0, itemdb_cancartstore_sub)
#define itemdb_canstore(item, gmlv) itemdb_isrestricted(item, gmlv, 0, itemdb_canstore_sub)
#define itemdb_canguildstore(item, gmlv) itemdb_isrestricted(item , gmlv, 0, itemdb_canguildstore_sub)
#define itemdb_canmail(item, gmlv) itemdb_isrestricted(item , gmlv, 0, itemdb_canmail_sub)
#define itemdb_canauction(item, gmlv) itemdb_isrestricted(item , gmlv, 0, itemdb_canauction_sub)

int itemdb_isequip(int);
int itemdb_isequip2(struct item_data *);
int itemdb_isidentified(int);
int itemdb_isidentified2(struct item_data *data);
int itemdb_isstackable(int);
int itemdb_isstackable2(struct item_data *);
uint64 itemdb_unique_id(int8 flag, int64 value); // Unique Item ID

void itemdb_reload(void);
void do_final_itemdb(void);
void do_init_itemdb(void);

/* incomplete */
struct itemdb_interface {
	void (*name_constants) (void);
	void (*force_name_constants) (void);
	/* */
	struct item_package *packages;
	unsigned short package_count;
	/* */
	DBMap *names;
	/* */
#if VERSION != -1
	void (*read_packages) (void);
	/* */
	void (*write_cached_packages) (const char *config_filename);
	bool (*read_cached_packages) (const char *config_filename);
#endif
	/* */
	struct item_data* (*name2id) (const char *str);
	void (*package_item) (struct map_session_data *sd, struct item_package *package);
};

struct itemdb_interface *itemdb;

void itemdb_defaults(void);

#endif /* _ITEMDB_H_ */
