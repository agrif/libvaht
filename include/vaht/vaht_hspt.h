#ifndef __INCLUDE_VAHT_HSPT_H__
#define __INCLUDE_VAHT_HSPT_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_hspt_group Hotspots
 * Functions for Hotspot resources.  These functions can be used
 * to inspect HSPT resources in a Mohawk archive. These hold lists of
 * hotspots associated with a CARD of the same id.
 * @{
 */

struct vaht_hspt_s;

/**
 * \brief the hotspot data type
 *
 * This data type refers to an individual hotspot resource. Use it as
 * a pointer only, because it is always dynamically allocated.
 *
 * \sa vaht_hspt_open
 */
typedef struct vaht_hspt_s vaht_hspt;

/**
 * \brief create a hotspot object
 *
 * This function will create an object that lets you inspect a HSPT
 * resource. If the resource is not a HSPT, this returns NULL. It
 * will keep the resource open for as long as it exists. You must
 * close it when you are done.
 *
 * \param resource the HSPT resource to inspect
 * \return the HSPT object, or NULL
 * \sa vaht_hspt_close vaht_card_hspt_open
 */
vaht_hspt* vaht_hspt_open(vaht_resource* resource);

/**
 * \brief close the hotspot object
 *
 * This function clears any memory used by a HSPT object. Use it when
 * you are done with one.
 *
 * \param hspt the HSPT object to close
 * \sa vaht_hspt_open
 */
void vaht_hspt_close(vaht_hspt* hspt);

/**
 * \brief get the number of records in this hotspot resource
 *
 * This will tell you how many total records there are in the HSPT
 * resource. Note that button lists are indexed from 1, so the number
 * returned by this function is also the highest valid index.
 *
 * \param hspt the HSPT to get a record count from
 * \return the number of records in the list
 */
uint16_t vaht_hspt_records(vaht_hspt* hspt);

/**
 * \brief get the blst id associated with a record
 *
 * This will return the blst_id of this record, which matches the
 * hotspot_id of the corresponding BLST record. Note that records are
 * indexed from 1, so the number of indexes is also the highest valid
 * index.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \return the blst_id
 * \sa vaht_blst_hotspot_id
 */
uint16_t vaht_hspt_blst_id(vaht_hspt* hspt, uint16_t i);

/**
 * \brief get the name record associated with a record
 *
 * This will return the index into NAME resource 2 which holds the
 * name of this hotspot. If this is negative, the hotspot has no name.
 * Note that records are indexed from 1, so the number of indexes is
 * also the highest valid index.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \return the name record index
 * \sa vaht_hspt_name_record
 */
int16_t vaht_hspt_name_record(vaht_hspt* hspt, uint16_t i);

/**
 * \brief get the name associated with a record
 *
 * This will return the name of this hotspot. If this is NULL, the
 * hotspot has no name.  This performs a NAME resource lookup, but the
 * result is cached, so future calls with the same arguments will be
 * less expensive. You must not free the result of this call.  Note
 * that records are indexed from 1, so the number of indexes is also
 * the highest valid index.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \return the name of this hotspot
 * \sa vaht_hspt_name
 */
const char* vaht_hspt_name(vaht_hspt* hspt, uint16_t i);

/**
 * \brief get the on-screen rectangle of the hotspot
 *
 * This will store the rectangle of a hotspot record in the pointers
 * given. For some hotspots, it is possible for left > right, or top >
 * bottom. Note that records are indexed from 1, so the number of
 * records is also the highest valid index. If any of the pointers are
 * NULL, that value will be ignored and not returned.
 *
 * For Riven, the screen is 608x392 pixels. X is left to right, and Y
 * is top to bottom.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \param left the pointer to store the left boundary in
 * \param right the pointer to store the right boundary in
 * \param top the pointer to store the top boundary in
 * \param bottom the pointer to store the bottom boundary in
 */
void vaht_hspt_rect(vaht_hspt* hspt, uint16_t i, int16_t* left, int16_t* right, int16_t* top, int16_t* bottom);

/**
 * \brief get the mouse-over cursor id associated with a record
 *
 * This will return the id of the cursor image to use when the mouse
 * is over this hotspot. Note that records are indexed from 1, so the
 * number of indexes is also the highest valid index.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \return the cursor image id to use
 */
uint16_t vaht_hspt_cursor(vaht_hspt* hspt, uint16_t i);

/**
 * \brief get whether a hotspot record is zip mode or not
 *
 * This will return 1 when the given hotspot is a zip mode hotspot,
 * and 0 when it isn't. Note that records are indexed from 1, so the
 * number of indexes is also the highest valid index.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \return whether the hotspot is zip mode or not
 */
uint16_t vaht_hspt_zip_mode(vaht_hspt* hspt, uint16_t i);

/**
 * \brief get the script associated with a record
 *
 * This will return the set of scripts associated with this
 * record. Note that these scripts are owned by the HSPT object, you
 * should not free them. Note that records are indexed from 1, so the
 * number of indexes is also the highest valid index.
 *
 * \param hspt the HSPT to read the record from
 * \param i the index of the record (starting with 1)
 * \return the scripts for this hotspot
 */
vaht_script* vaht_hspt_script(vaht_hspt* hspt, uint16_t i);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_HSPT_H__ */
