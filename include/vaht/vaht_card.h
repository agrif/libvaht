#ifndef __INCLUDE_VAHT_CARD_H__
#define __INCLUDE_VAHT_CARD_H__

#include <stdint.h>
#include "vaht_resource.h"

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
 * @}
 */

#endif /* __INCLUDE_VAHT_CARD_H__ */
