#ifndef __INCLUDE_VAHT_NAME_H__
#define __INCLUDE_VAHT_NAME_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_name_group Name Indexes
 * Functions for Name Index Resources.  These functions can be used
 * to inspect NAME resources in a Mohawk archive. There are special
 * NAME resources that contain lists of names for other resource
 * types.
 * @{
 */

struct vaht_name_s;

/**
 * \brief the name index data type
 *
 * This data type refers to an individual name index. Use it as a
 * pointer only, because it is always dynamically allocated.
 * \sa vaht_name_open
 */
typedef struct vaht_name_s vaht_name;

/**
 * \brief create a name index object
 *
 * This function will create an object that lets you inspect a NAME
 * resource. If the resource is not a NAME, this returns NULL. It
 * will keep the resource open for as long as it exists. You must
 * close it when you are done.
 * \param resource the NAME resource to inspect
 * \return the NAME object, or NULL
 * \sa vaht_name_close
 */
vaht_name* vaht_name_open(vaht_resource* resource);

/**
 * \brief close the name index object
 *
 * This function clears any memory used by a NAME object. Use it when
 * you are done with one.
 * \param name the NAME object to close
 * \sa vaht_name_open
 */
void vaht_name_close(vaht_name* name);

/**
 * \brief get how many entries in the name index
 *
 * This function returns how many strings there are in this name
 * index.
 * \param name the NAME object to get the size of
 * \return the number of entries
 * \sa vaht_name_get
 */
uint16_t vaht_name_count(vaht_name* name);

/**
 * \brief get a string from the name index
 *
 * This function will return a name from the name index.
 * \param name the NAME object to index into
 * \param the index, up to (and including) count - 1
 * \return the string (which MUST be free'd when you are done)
 * \sa vaht_name_count
 */
char* vaht_name_get(vaht_name* name, uint16_t i);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_NAME_H__ */
