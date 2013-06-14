#ifndef __INCLUDE_VAHT_BLST_H__
#define __INCLUDE_VAHT_BLST_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_blst_group Button Lists
 * Functions for Button List resources.  These functions can be used
 * to inspect BLST resources in a Mohawk archive. These hold lists of
 * hotspots associated with a CARD of the same id.
 * @{
 */

struct vaht_blst_s;

/**
 * \brief the button list data type
 *
 * This data type refers to an individual button list. Use it as a
 * pointer only, because it is always dynamically allocated.
 *
 * \sa vaht_blst_open
 */
typedef struct vaht_blst_s vaht_blst;

/**
 * \brief create a button list object
 *
 * This function will create an object that lets you inspect a BLST
 * resource. If the resource is not a BLST, this returns NULL. It
 * will keep the resource open for as long as it exists. You must
 * close it when you are done.
 *
 * \param resource the BLST resource to inspect
 * \return the BLST object, or NULL
 * \sa vaht_blst_close vaht_card_blst_open
 */
vaht_blst* vaht_blst_open(vaht_resource* resource);

/**
 * \brief close the button list object
 *
 * This function clears any memory used by a BLST object. Use it when
 * you are done with one.
 *
 * \param blst the BLST object to close
 * \sa vaht_blst_open
 */
void vaht_blst_close(vaht_blst* blst);

/**
 * \brief get the number of records in this button list
 *
 * This will tell you how many total records there are in the BLST
 * resource. Note that button lists are indexed from 1, so the number
 * returned by this function is also the highest valid index.
 *
 * \param blst the BLST to get a record count from
 * \return the number of records in the list
 */
uint16_t vaht_blst_records(vaht_blst* blst);

/**
 * \brief get the hotspot state associated with a record
 *
 * This will return whether the hotspots associated with this record
 * should be enabled (1) or disabled (0) when the record is
 * activated. Note that records are indexed from 1, so the number of
 * indexes is also the highest valid index. If this record does not
 * exist, 0 is returned, which is indistinguishable from a record that
 * does exist that disables hotspots, so be warned.
 *
 * \param blst the BLST to read the record from
 * \param i the index of the record (starting with 1)
 * \return the hotspot state, or 0
 */
uint16_t vaht_blst_enabled(vaht_blst* blst, uint16_t i);

/**
 * \brief get the hotspot id associated with a record
 *
 * This will return the hotspot_id of this record, which matches the
 * blst_id of the corresponding HSPT record. Note that records are
 * indexed from 1, so the number of indexes is also the highest valid
 * index. If this record does not exist, -1 (an invalid hotspot_id) is
 * returned.
 *
 * \param blst the BLST to read the record from
 * \param i the index of the record (starting with 1)
 * \return the hotspot_id, or -1
 * \sa vaht_hspt_blst_id
 */
int32_t vaht_blst_hotspot_id(vaht_blst* blst, uint16_t i);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_BLST_H__ */
