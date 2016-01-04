#ifndef __INCLUDE_VAHT_SLST_H__
#define __INCLUDE_VAHT_SLST_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_slst_group Sound Lists
 
 * Functions for Sound List Resources. These resources are used to
 * describe ambient background sounds. There is one for every CARD
 * with the same id.
 * @{
 */

struct vaht_slst_s;

/**
 * \brief the sound list data type
 *
 * This data type refers to an individual sound list. Use it as a
 * pointer only, because it is always dynamically allocated.
 * \sa vaht_slst_open
 */
typedef struct vaht_slst_s vaht_slst;

/**
 * \brief create a sound list object
 *
 * This function will create an object that lets you inspect a sound
 * list resource. If the resource is not a sound list, this returns
 * NULL. It will keep the resource open for as long as it exists. You
 * must close it when you are done.
 * \param resource the sound list resource to inspect
 * \return the sound list object, or NULL
 * \sa vaht_slst_close
 */
vaht_slst* vaht_slst_open(vaht_resource* resource);

/**
 * \brief close the sound list object
 *
 * This function clears any memory used by a sound list object. Use
 * it when you are done with one.
 * \param slst the sound list object to close
 * \sa vaht_slst_open
 */
void vaht_slst_close(vaht_slst* slst);

/**
 * \brief get the number of records in this sound list
 *
 * This will tell you how many total records there are in the SLST
 * resource. Note that sound lists are indexed from 1, so the number
 * returned by this function is also the highest valid index.
 *
 * \param slst the SLST to get a record count from
 * \return the number of records in the list
 */
uint16_t vaht_slst_records(vaht_slst* slst);

/**
 * \brief get the number of sounds in a record
 *
 * This will tell you how many sounds there are in a particular
 * record, or sound set. Not that records are indexed from 1.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \return the number of sounds in the record
 * \sa vaht_slst_records
 */
uint16_t vaht_slst_count(vaht_slst* slst, uint16_t i);

/**
 * \brief get the sound resource ID
 *
 * This will tell you the tWAV resource ID for each sound in a sound
 * set. Note that records are indexed from 1, while sounds are indexed
 * from 0.
 *
 * Returns 0 for invalid sound indexes.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \param j the index of the sound (starting with 0)
 * \return the resource ID of the tWAV resource
 * \sa vaht_slst_records vaht_slst_count
 */
uint16_t vaht_slst_id(vaht_slst* slst, uint16_t i, uint16_t j);

/**
 * \brief fade settings
 *
 * This enumeration describes the fade settings for a sound set:
 * whether to fade in, out, or both.
 *
 * \sa vaht_slst_fade
 */
enum vaht_slst_fade
{
    /** no fading */
    SLST_NO_FADE = 0,
    /** fade out sounds not in this set */
    SLST_FADE_OUT = 1,
    /** fade in sounds in this set */
    SLST_FADE_IN = 2,
    /** fade in and out sounds (SLST_FADE_IN | SLST_FADE_OUT) */
    SLST_FADE_IN_OUT = 3
};


/**
 * \brief get fade settings
 *
 * This will tell you what fades to apply to change to this sound
 * set. Note that records are indexed from 1.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \return a description of how to fade this sound set
 * \sa vaht_slst_records
 */
enum vaht_slst_fade vaht_slst_fade(vaht_slst* slst, uint16_t i);

/**
 * \brief get loop settings
 *
 * This will tell you whether to loop a particular sound set. Note
 * that records are indexed from 1.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \return 1 to loop forever, 0 otherwise
 * \sa vaht_slst_records
 */
uint16_t vaht_slst_loop(vaht_slst* slst, uint16_t i);

/**
 * \brief get global volume of sound set
 *
 * This will tell you the global volume of this sound set. The ranges
 * are not known exactly, but are around 0-400. Note that records are
 * indexed from 1.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \return global volume of the sound set
 * \sa vaht_slst_records
 */
uint16_t vaht_slst_global_volume(vaht_slst* slst, uint16_t i);

/**
 * \brief get the volume for a sound
 *
 * Get the volume for a particular sound. The ranges are not known
 * exactly, but are around 0-400. Note that records are indexed from
 * 1, and sounds are indexed from 0.
 *
 * Returns 0 for invalid sound indexes.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \param j the index of the sound (starting with 0)
 * \return volume for this sound
 * \sa vaht_slst_records, vaht_slst_count
 */
uint16_t vaht_slst_volume(vaht_slst* slst, uint16_t i, uint16_t j);

/**
 * \brief get the balance of a sound
 *
 * Get the balance for a particular sound. Values less than 0 are to
 * the left, and values greater than 0 are to the right. Note that
 * records are indexed from 1, and sounds are indexed from 0.
 *
 * Returns 0 for invalid sound indexes.
 *
 * \param slst the SLST to read from
 * \param i the index of the record (starting with 1)
 * \param j the index of the sound (starting with 0)
 * \return balance for this sound (<0 left, =0 middle, >0 right)
 * \sa vaht_slst_records, vaht_slst_count
 */
int16_t vaht_slst_balance(vaht_slst* slst, uint16_t i, uint16_t j);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_SLST_H__ */
