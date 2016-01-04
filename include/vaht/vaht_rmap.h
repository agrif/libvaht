#ifndef __INCLUDE_VAHT_RMAP_H__
#define __INCLUDE_VAHT_RMAP_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_rmap_group Reverse Maps
 
 * Functions for Reverse Map Resources. These resources are used to
 * look up a local card when jumping to it from a remote stack. There
 * is only one such resource per stack.
 * @{
 */

struct vaht_rmap_s;

/**
 * \brief the reverse map data type
 *
 * This data type refers to an individual reverse map. Use it as a
 * pointer only, because it is always dynamically allocated.
 * \sa vaht_rmap_open
 */
typedef struct vaht_rmap_s vaht_rmap;

/**
 * \brief create a reverse map object
 *
 * This function will create an object that lets you inspect a reverse
 * map resource. If the resource is not a reverse map, this returns
 * NULL. It will keep the resource open for as long as it exists. You
 * must close it when you are done.
 * \param resource the reverse map resource to inspect
 * \return the reverse map object, or NULL
 * \sa vaht_rmap_close
 */
vaht_rmap* vaht_rmap_open(vaht_resource* resource);

/**
 * \brief close the reverse map object
 *
 * This function clears any memory used by a reverse map object. Use
 * it when you are done with one.
 * \param rmap the reverse map object to close
 * \sa vaht_rmap_open
 */
void vaht_rmap_close(vaht_rmap* rmap);

/**
 * \brief get how many entries in the reverse map
 *
 * This function returns how many codes there are in this reverse map.
 * \param rmap the reverse map object to get the size of
 * \return the number of entries
 * \sa vaht_rmap_get
 */
uint16_t vaht_rmap_count(vaht_rmap* rmap);

/**
 * \brief get a string from the reverse map
 *
 * This function will return a code from the reverse map. Note that
 * this (unlike CARD resources) is indexed starting at 0, so there is
 * an extra code at 0 that does not correspond to any card.
 * \param rmap the reverse map object to index into
 * \param i the index, up to (and including) count - 1
 * \return the code
 * \sa vaht_rmap_count
 */
uint32_t vaht_rmap_get(vaht_rmap* rmap, uint16_t i);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_RMAP_H__ */
