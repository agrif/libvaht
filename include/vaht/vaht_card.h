#ifndef __INCLUDE_VAHT_CARD_H__
#define __INCLUDE_VAHT_CARD_H__

#include <stdint.h>
#include "vaht_resource.h"
#include "vaht_script.h"
#include "vaht_plst.h"
#include "vaht_blst.h"
#include "vaht_hspt.h"

/**
 * \defgroup vaht_card_group Cards
 * Functions for Cards. Use these to access CARD resource metadata,
 * like zip mode eligibility and scripts.
 * @{
 */

struct vaht_card_s;

/**
 * \brief the card data type
 *
 * This data type refers to an individual card. Use it as a pointer
 * only, because it is always dynamically allocated.
 *
 * \sa vaht_card_open
 */
typedef struct vaht_card_s vaht_card;

/**
 * \brief create a card object
 *
 * This function will create an object that lets you inspect a CARD
 * resource. If the resource is not a CARD, this returns NULL. It will
 * keep the resource open for as long as it exists. You must close it
 * when you are done.
 *
 * \param resource the CARD resource to inspect
 * \return the CARD object, or NULL
 * \sa vaht_card_close
 */
vaht_card* vaht_card_open(vaht_resource* resource);

/**
 * \brief close the card object
 *
 * This function clears any memory used by a CARD object. Use it when
 * you are done with one.
 *
 * \param card the CARD object to close
 * \sa vaht_card_open
 */
void vaht_card_close(vaht_card* card);

/**
 * \brief get the name record of this card
 *
 * This returns the record index in NAME resource 1 that corresponds
 * to this card, or a negative number signalling this card has no
 * special name. There is also the convenience function
 * vaht_card_name() to get this name for you, and cache it.
 *
 * \param card the CARD to get the name record for
 * \return the record in NAME resource 1
 * \sa vaht_card_name
 */
int16_t vaht_card_name_record(vaht_card* card);

/**
 * \brief get the name of this card
 *
 * This returns the name associated with this card, which is distinct
 * from the resource name. It performs the appropriate NAME resource
 * lookup for you, and caches the result (subsequent calls are less
 * expensive).
 *
 * \param card the CARD to get the name of
 * \return the name, as a string (or NULL for no name)
 * \sa vaht_card_name_record
 */
const char* vaht_card_name(vaht_card* card);

/**
 * \brief get whether this card is zip-enabled
 *
 * This will return non-zero if this card can be used as a zip-mode
 * destination. Engines should keep track of visits to this card, so
 * they know when zip hotspots are usable (if their card has been
 * visited).
 *
 * \param card the CARD to check for zip status
 * \return 0 if the card is normal, 1 (usually) if it's zip-enabled
 */
uint16_t vaht_card_zip_mode(vaht_card* card);

/**
 * \brief get the script for this card
 *
 * This will return the script object for this card. This object is
 * owned by the card object, so you do not need to free it (it's freed
 * when the card object is closed).
 *
 * \param card the CARD to get the script for
 * \return the script object
 */
vaht_script* vaht_card_script(vaht_card* card);

/**
 * \brief get the associated Picture List for this card
 *
 * This will get the PLST resource associated with this card -- it's
 * the one with the same id as the card. If for some reason it does
 * not exist, this function returns NULL. You are responsible for
 * closing this when you're done with it!
 *
 * \param card the CARD to get the list for
 * \return the PLST resource, or NULL
 */
vaht_plst* vaht_card_plst_open(vaht_card* card);

/**
 * \brief get the associated Button List for this card
 *
 * This will get the BLST resource associated with this card -- it's
 * the one with the same id as the card. If for some reason it does
 * not exist, this function returns NULL. You are responsible for
 * closing this when you're done with it!
 *
 * \param card the CARD to get the list for
 * \return the BLST resource, or NULL
 */
vaht_blst* vaht_card_blst_open(vaht_card* card);

/**
 * \brief get the associated Hotspot resource for this card
 *
 * This will get the HSPT resource associated with this card -- it's
 * the one with the same id as the card. If for some reason it does
 * not exist, this function returns NULL. You are responsible for
 * closing this when you're done with it!
 *
 * \param card the CARD to get the list for
 * \return the HSPT resource, or NULL
 */
vaht_hspt* vaht_card_hspt_open(vaht_card* card);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_CARD_H__ */
