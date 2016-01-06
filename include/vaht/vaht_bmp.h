#ifndef __INCLUDE_VAHT_BMP_H__
#define __INCLUDE_VAHT_BMP_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_bmp_group Bitmaps
 * Functions for Bitmap Resources.
 * These functions can be used to inspect tBMP (bitmap) resources in the
 * archive. You can use these also to get decompressed image data.
 * @{
 */

struct vaht_bmp_s;

/**
 * \brief the bitmap data type
 *
 * This data type refers to an individual bitmap. Use it as a pointer only,
 * because it is always dynamically allocated.
 * \sa vaht_bmp_open
 */
typedef struct vaht_bmp_s vaht_bmp;

/**
 * \brief create a bitmap object
 *
 * This function will create a bitmap object for the given resource. If the
 * resource given is not a bitmap, or in a format that libvaht does not
 * understand, it will return null. You must close these after you are done
 * with them. This object will keep the resource open while it exists.
 * \param resource the resource you want a bitmap object of
 * \return a bitmap object, or NULL
 * \sa vaht_bmp_close
 */
vaht_bmp* vaht_bmp_open(vaht_resource* resource);

/**
 * \brief close a bitmap object
 *
 * This function will free the memory associated with a bitmap object,
 * including any decompressed data. Use it when you are done with a bitmap.
 * \param bmp the bitmap to close
 * \sa vaht_bmp_open
 */
void vaht_bmp_close(vaht_bmp* bmp);

/**
 * \brief get the width of a bitmap
 *
 * This function will return the width of the image represented by this
 * bitmap, in pixels.
 * \param bmp the bitmap object to get the width for
 * \return the width of the image
 * \sa vaht_bmp_height vaht_bmp_data
 */
uint16_t vaht_bmp_width(vaht_bmp* bmp);

/**
 * \brief get the height of a bitmap
 *
 * This function will return the height of the image represented by this
 * bitmap, in pixels.
 * \param bmp the bitmap object to get the height for
 * \return the height of the image
 * \sa vaht_bmp_width vaht_bmp_height
 */
uint16_t vaht_bmp_height(vaht_bmp* bmp);

/**
 * \brief get the decompressed data for a bitmap
 *
 * This function will read in the data for a bitmap, and decompress it if
 * needed. Note that if there is decompression to be done, it is done now,
 * and not when the bmp object is created.
 *
 * The data is a list of pixels, each pixel containing a byte each of red,
 * green, and blue components. Each component varies from 0 to 255, with 0
 * meaning black. The pixels come ordered from left to right, then top down.
 * (The first row comes first, then the second, etc.)
 *
 * The pointer returned does not have to be freed, as the data is cached
 * until the bitmap object is closed. Subsequent calls to this function will
 * not reread or decompress the data, but simply returned the cached data.
 *
 * \param bmp the bitmap to get the data for
 * \return a pointer to the bitmap data
 * \sa vaht_bmp_width vaht_bmp_height
 */
uint8_t* vaht_bmp_data(vaht_bmp* bmp);

/**
 * \brief get the palette for a bitmap
 *
 * This function will read in the palette for a bitmap, and decompress it if
 * needed. Note that if there is decompression to be done, it is done now,
 * and not when the bmp object is created.
 *
 * The palette is a list of 256 pixels, each pixel containing a byte
 * each of red, green, and blue components. Each component varies from
 * 0 to 255, with 0 meaning black.
 *
 * The pointer returned does not have to be freed, as the data is cached
 * until the bitmap object is closed. Subsequent calls to this function will
 * not reread or decompress the data, but simply returned the cached data.
 *
 * If the image is truecolor (not indexed), this function returns NULL.
 *
 * \param bmp the bitmap to get the palette for
 * \return a pointer to the palette data (or NULL)
 * \sa vaht_bmp_width vaht_bmp_height vaht_bmp_indexed_data vaht_bmp_truecolor
 */
uint8_t* vaht_bmp_palette(vaht_bmp* bmp);

/**
 * \brief get the decompressed indexed data for a bitmap
 *
 * This function will read in the data for a bitmap, and decompress it if
 * needed. Note that if there is decompression to be done, it is done now,
 * and not when the bmp object is created.
 *
 * The data is a list of pixels, each pixel containing a byte index
 * into the palette table. The pixels come ordered from left to right,
 * then top down. (The first row comes first, then the second, etc.)
 *
 * The pointer returned does not have to be freed, as the data is cached
 * until the bitmap object is closed. Subsequent calls to this function will
 * not reread or decompress the data, but simply returned the cached data.
 *
 * If the image is truecolor (not indexed), this function returns NULL.
 *
 * \param bmp the bitmap to get the indexed data for
 * \return a pointer to the indexed bitmap data (or NULL)
 * \sa vaht_bmp_width vaht_bmp_height vaht_bmp_palette vaht_bmp_truecolor
 */
uint8_t* vaht_bmp_indexed_data(vaht_bmp* bmp);

/**
 * \brief was the image compressed?
 *
 * This function will return 1 if the image was compressed in the archive,
 * and 0 otherwise.
 * \param bmp the bitmap object to inspect
 * \return 1 or 0
 * \sa vaht_bmp_truecolor
 */
uint8_t vaht_bmp_compressed(vaht_bmp* bmp);

/**
 * \brief was this image truecolor?
 *
 * This function will return 1 if the image was stored as truecolor in the
 * archive, and 0 if it was indexed.
 * \param bmp the bitmap object to inspect
 * \return 1 or 0
 * \sa vaht_bmp_compressed
 */
uint8_t vaht_bmp_truecolor(vaht_bmp* bmp);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_BMP_H__ */
