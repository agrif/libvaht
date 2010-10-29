#ifndef __INCLUDE_VAHT_PLST_H__
#define __INCLUDE_VAHT_PLST_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_plst_group Picture Lists
 * Functions for Picture List resources.  These functions can be used
 * to inspect PLST resources in a Mohawk archive. These hold lists of
 * bitmaps associated with a CARD of the same id, and the drawing
 * positions of each.
 * @{
 */

struct vaht_plst_s;

/**
 * \brief the picture list data type
 *
 * This data type refers to an individual picture list. Use it as a
 * pointer only, because it is always dynamically allocated.
 *
 * \sa vaht_plst_open
 */
typedef struct vaht_plst_s vaht_plst;

/**
 * \brief create a picture list object
 *
 * This function will create an object that lets you inspect a PLST
 * resource. If the resource is not a PLST, this returns NULL. It
 * will keep the resource open for as long as it exists. You must
 * close it when you are done.
 *
 * \param resource the PLST resource to inspect
 * \return the PLST object, or NULL
 * \sa vaht_plst_close vaht_card_plst_open
 */
vaht_plst* vaht_plst_open(vaht_resource* resource);

/**
 * \brief close the picture list object
 *
 * This function clears any memory used by a PLST object. Use it when
 * you are done with one.
 *
 * \param plst the PLST object to close
 * \sa vaht_plst_open
 */
void vaht_plst_close(vaht_plst* plst);

/**
 * \brief get the number of records in this picture list
 *
 * This will tell you how many total records there are in the PLST
 * resource. Note that picture lists are indexed from 1, so the number
 * returned by this function is also the highest valid index.
 *
 * \param plst the PLST to get a record count from
 * \return the number of records in the list
 */
uint16_t vaht_plst_records(vaht_plst* plst);

/**
 * \brief get the bitmap id associated with a record
 *
 * This will return the resource ID of the tBMP this record is linked
 * to. Note that records are indexed from 1, so the number of indexes
 * is also the highest valid index. If the record does not exist, -1
 * (an invalid resource id) is returned.
 *
 * \param plst the PLST to read the record from
 * \param i the index of the record (starting with 1)
 * \return the resource ID of the tBMP, or 0
 * \sa vaht_plst_bitmap_open
 */
int32_t vaht_plst_bitmap_id(vaht_plst* plst, uint16_t i);

/**
 * \brief get the bitmap object associated with a record
 *
 * This will return the vaht_bmp object associated with a record in
 * the list. Note that records are indexed from 1, so the number of
 * records is also the highest valid index. If the record does not
 * exist, NULL is returned. You are responsible for closing this bmp
 * object.
 *
 * \param plst the PLST to read the record from
 * \param i the index of the record (starting with 1)
 * \return the vaht_bmp object, or NULL
 * \sa vaht_plst_bitmap_id
 */
vaht_bmp* vaht_plst_bitmap_open(vaht_plst* plst, uint16_t i);

/**
 * \brief get the on-screen draw rectangle of the image
 *
 * This will store the draw rectangle of an image record in the
 * pointers given. Note that records are indexed from 1, so the number
 * of records is also the highest valid index. There is no good way to
 * see if this call fails, so compare the result with
 * vaht_plst_bitmap_id() or similar. If any of the pointers are NULL,
 * that value will be ignored and not returned.
 *
 * For Riven, the screen is 608x392 pixels. X is left to right, and Y
 * is top to bottom.
 *
 * \param plst the PLST to read the record from
 * \param i the index of the record (starting with 1)
 * \param left the pointer to store the left boundary in
 * \param right the pointer to store the right boundary in
 * \param top the pointer to store the top boundary in
 * \param bottom the pointer to store the bottom boundary in
 */
void vaht_plst_rect(vaht_plst* plst, uint16_t i, uint16_t* left, uint16_t* right, uint16_t* top, uint16_t* bottom);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_PLST_H__ */
