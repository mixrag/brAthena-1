/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/common/mapindex.c                                                *
* Descri��o Prim�ria.                                                        *
* Descri��o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena                                                   *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#include "../common/mmo.h"
#include "../common/showmsg.h"
#include "../common/malloc.h"
#include "../common/strlib.h"
#include "../common/db.h"
#include "../config/configs.h"
#include "mapindex.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

struct _indexes {
	char name[MAP_NAME_LENGTH]; //Stores map name
} indexes[MAX_MAPINDEX];

int max_index = 0;

char mapindex_cfgfile[80] = "db/map_index.txt";

#define mapindex_exists_sub(id) (indexes[id].name[0] != '\0')

bool mapindex_exists(int id) {
	return mapindex_exists_sub(id);
}

/// Retrieves the map name from 'string' (removing .gat extension if present).
/// Result gets placed either into 'buf' or in a static local buffer.
const char *mapindex_getmapname(const char *string, char *output)
{
	static char buf[MAP_NAME_LENGTH];
	char *dest = (output != NULL) ? output : buf;

	size_t len = strnlen(string, MAP_NAME_LENGTH_EXT);
	if(len == MAP_NAME_LENGTH_EXT) {
		ShowWarning(read_message("Source.common.mapindex_getmapname"), 2*MAP_NAME_LENGTH_EXT, string);
		len--;
	}
	if(len >= 4 && stricmp(&string[len-4], ".gat") == 0)
		len -= 4; // strip .gat extension

	len = min(len, MAP_NAME_LENGTH-1);
	safestrncpy(dest, string, len+1);
	memset(&dest[len], '\0', MAP_NAME_LENGTH-len);

	return dest;
}

/// Retrieves the map name from 'string' (adding .gat extension if not already present).
/// Result gets placed either into 'buf' or in a static local buffer.
const char *mapindex_getmapname_ext(const char *string, char *output)
{
	static char buf[MAP_NAME_LENGTH_EXT];
	char *dest = (output != NULL) ? output : buf;

	size_t len;

	strcpy(buf,string);
	sscanf(string,"%*[^#]%*[#]%s",buf);

	len = safestrnlen(buf, MAP_NAME_LENGTH);

	if(len == MAP_NAME_LENGTH) {
		ShowWarning(read_message("Source.common.mapindex_getmapname"), 2*MAP_NAME_LENGTH, buf);
		len--;
	}
	safestrncpy(dest, buf, len+1);

	if(len < 4 || stricmp(&dest[len-4], ".gat") != 0) {
		strcpy(&dest[len], ".gat");
		len += 4; // add .gat extension
	}

	memset(&dest[len], '\0', MAP_NAME_LENGTH_EXT-len);

	return dest;
}

/// Adds a map to the specified index
/// Returns 1 if successful, 0 oherwise
int mapindex_addmap(int index, const char *name)
{
	char map_name[MAP_NAME_LENGTH];

	if(index == -1) {
		for(index = 1; index < max_index; index++) {
			//if (strcmp(indexes[index].name,"#CLEARED#")==0)
			if(indexes[index].name[0] == '\0')
				break;
		}
	}

	if(index < 0 || index >= MAX_MAPINDEX) {
		ShowError(read_message("Source.common.mapindex_add"), index, name, MAX_MAPINDEX);
		return 0;
	}

	mapindex_getmapname(name, map_name);

	if(map_name[0] == '\0') {
		ShowError(read_message("Source.common.mapindex_add2"));
		return 0;
	}

	if(strlen(map_name) >= MAP_NAME_LENGTH) {
		ShowError(read_message("Source.common.mapindex_add3"), map_name, MAP_NAME_LENGTH);
		return 0;
	}

	if(mapindex_exists_sub(index)) {
		ShowWarning(read_message("Source.common.mapindex_add4"), index, indexes[index].name, map_name);
		strdb_remove(mapindex_db, indexes[index].name);
	}

	safestrncpy(indexes[index].name, map_name, MAP_NAME_LENGTH);
	strdb_iput(mapindex_db, map_name, index);
	if(max_index <= index)
		max_index = index+1;

	return index;
}

unsigned short mapindex_name2id(const char *name)
{
	//TODO: Perhaps use a db to speed this up? [Skotlex]
	int i;

	char map_name[MAP_NAME_LENGTH];
	mapindex_getmapname(name, map_name);

		if((i = strdb_iget(mapindex_db, map_name)))
			return i;
	#if VERSION == 1
 	ShowDebug(read_message("Source.common.mapindex_name2id"), map_name);
	#endif
	return 0;
}

const char* mapindex_id2name_sub(unsigned short id,const char *file, int line, const char *func) {
	if(id > MAX_MAPINDEX || !mapindex_exists_sub(id)) {
		ShowDebug(read_message("Source.common.mapindex_id2name"), id,file,func,line);
		return indexes[0].name; // dummy empty string so that the callee doesn't crash
	}
	return indexes[id].name;
}

int mapindex_init(void)
{
	FILE *fp;
	char line[1024];
	int last_index = -1;
	int index, total = 0;
	char map_name[12];

	if((fp = fopen(mapindex_cfgfile,"r")) == NULL) {
		ShowFatalError(read_message("Source.common.mapindex_init"), mapindex_cfgfile);
		exit(EXIT_FAILURE); //Server can't really run without this file.
	}
	memset (&indexes, 0, sizeof (indexes));
	mapindex_db = strdb_alloc(DB_OPT_DUP_KEY, MAP_NAME_LENGTH);
	while(fgets(line, sizeof(line), fp)) {
	
	#if VERSION == -1
	if(total >= 970)
		total = 0;
	#endif

		if(line[0] == '/' && line[1] == '/')
			continue;

		switch(sscanf(line, "%12s\t%d", map_name, &index)) {
			case 1: //Map with no ID given, auto-assign
				index = last_index+1;
			case 2: //Map with ID given
	#if VERSION == -1
				if(total < 446)
	#endif
				mapindex_addmap(index,map_name);
				total++;
				break;
			default:
				continue;
		}
		last_index = index;
	}
	fclose(fp);

	if(!strdb_iget(mapindex_db, MAP_DEFAULT)) {
		ShowError("mapindex_init: MAP_DEFAULT '%s' not found in cache! update mapindex.h MAP_DEFAULT var!!!\n",MAP_DEFAULT);
	}
	return total;
}

int mapindex_removemap(int index)
{
	indexes[index].name[0] = '\0';
	return 0;
}

void mapindex_final(void) {
	db_destroy(mapindex_db);
}
