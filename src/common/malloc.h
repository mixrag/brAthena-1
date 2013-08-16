/****************************************************************************!
*                _           _   _   _                                       *    
*               | |__  _ __ / \ | |_| |__   ___ _ __   __ _                  *  
*               | '_ \| '__/ _ \| __| '_ \ / _ \ '_ \ / _` |                 *   
*               | |_) | | / ___ \ |_| | | |  __/ | | | (_| |                 *    
*               |_.__/|_|/_/   \_\__|_| |_|\___|_| |_|\__,_|                 *    
*                                                                            *
*                                                                            *
* \file src/common/malloc.h                                                  *
* Descri��o Prim�ria.                                                        *
* Descri��o mais elaborada sobre o arquivo.                                  *
* \author brAthena, Athena                                                   *
* \date ?                                                                    *
* \todo ?                                                                    *  
*****************************************************************************/

#ifndef _MALLOC_H_
#define _MALLOC_H_

#include "../common/cbasetypes.h"

#define ALC_MARK __FILE__, __LINE__, __func__


// default use of the built-in memory manager
#if !defined(NO_MEMMGR) && !defined(USE_MEMMGR)
#if defined(MEMWATCH) || defined(DMALLOC) || defined(GCOLLECT)
// disable built-in memory manager when using another memory library
#define NO_MEMMGR
#else
// use built-in memory manager by default
#define USE_MEMMGR
#endif
#endif


//////////////////////////////////////////////////////////////////////
// Enable memory manager logging by default
#define LOG_MEMMGR

// no logging for minicore
#if defined(MINICORE) && defined(LOG_MEMMGR)
#undef LOG_MEMMGR
#endif

#	define aMalloc(n)		iMalloc->malloc (n,ALC_MARK)
#	define aCalloc(m,n)		iMalloc->calloc (m,n,ALC_MARK)
#	define aRealloc(p,n)	iMalloc->realloc	(p,n,ALC_MARK)
#	define aStrdup(p)		iMalloc->astrdup (p,ALC_MARK)
#	define aFree(p)			iMalloc->free   (p,ALC_MARK)

/////////////// Buffer Creation /////////////////
// Full credit for this goes to Shinomori [Ajarn]

#ifdef __GNUC__ // GCC has variable length arrays

#define CREATE_BUFFER(name, type, size) type name[size]
#define DELETE_BUFFER(name)

#else // others don't, so we emulate them

#define CREATE_BUFFER(name, type, size) type *name = (type *) aCalloc (size, sizeof(type))
#define DELETE_BUFFER(name) aFree(name)

#endif

////////////// Others //////////////////////////
// should be merged with any of above later
#define CREATE(result, type, number) (result) = (type *) aCalloc ((number), sizeof(type))
#define RECREATE(result, type, number) (result) = (type *) aRealloc ((result), sizeof(type) * (number))

////////////////////////////////////////////////

//void malloc_memory_check(void);
//bool malloc_verify_ptr(void* ptr);
//size_t malloc_usage (void);
//void malloc_init (void);
//void malloc_final (void);

void malloc_defaults(void);

struct malloc_interface {
	void* (*malloc	)(size_t size, const char *file, int line, const char *func);
	void* (*calloc	)(size_t num, size_t size, const char *file, int line, const char *func);
	void* (*realloc	)(void *p, size_t size, const char *file, int line, const char *func);
	char* (*astrdup	)(const char *p, const char *file, int line, const char *func);
	void  (*free	)(void *p, const char *file, int line, const char *func);

	void	(*memory_check)(void);
	bool	(*verify_ptr)(void* ptr);
	size_t	(*usage) (void);
	void	(*init) (void);
	void	(*final) (void);
} iMalloc_s;

void memmgr_report (int extra);

struct malloc_interface *iMalloc;
#endif /* _MALLOC_H_ */
