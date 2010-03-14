#ifndef __INCLUDE_VAHT_MOV_H__
#define __INCLUDE_VAHT_MOV_H__

#include <stdint.h>
#include "vaht_resource.h"

//! \defgroup vaht_mov_group Movies
//! Functions for Movie Resources.
//! These functions can be used to read modified tMOV resources from the
//! archive. Right now, they assist in correcting the data so that it can
//! be played in Quicktime on its own, but eventually it will also include
//! functions for decompressing the movie frames.
//! @{

struct vaht_mov_s;

//! the movie data type

//! This data type refers to an individual movie. Use it as a pointer only,
//! because it is always dynamically allocated.
//! \sa vaht_mov_open
typedef struct vaht_mov_s vaht_mov;

//! create a movie object

//! This function will create a movie object for the given resource. If the
//! resource is not a movie, or it is in a format that libvaht does not
//! understand, it will return NULL. You must close these after you are done
//! with them. This object will keep the resource open while it exists.
//! \param resource the resource you want a movie object of
//! \return a movie object, or NULL
//! \sa vaht_mov_close
vaht_mov* vaht_mov_open(vaht_resource* resource);

//! close a bitmap object

//! This function frees the memory associated with a bitmap object. Use it when
//! you are done with a movie.
//! \param mov the movie to close
//! \sa vaht_mov_open
void vaht_mov_close(vaht_mov* mov);

//! read from a movie object

//! This function is analogous to \ref vaht_resource_read . Use it to read the
//! data for the Quicktime file. It will modify STCO chunks in-stream so that
//! the Quicktime file will play correctly on its own, outside of the Mohawk
//! archive.
//! \param mov the movie object to read
//! \param size the amount of data to read
//! \param buffer the buffer to read data into
//! \return the number of bytes read
//! \sa vaht_mov_seek
uint32_t vaht_mov_read(vaht_mov* mov, uint32_t size, void* buffer);

//! seek into a movie object

//! This function is analogous to \ref vaht_resource_seek . It will move the
//! read cursor to a specific position in the file, given in bytes since the
//! beginning of the file.
//! \param mov the movie object to seek
//! \param seek the position to seek to
//! \sa vaht_mov_read vaht_mov_tell
void vaht_mov_seek(vaht_mov* mov, uint32_t seek);

//! get the read position

//! This function is analogous to \ref vaht_resource_tell . It will tell you
//! where the read cursor is in the file, in bytes since the start of the file.
//! \param mov the movie to get the position for
//! \return the current read position
//! \sa vaht_mov_seek
uint32_t vaht_mov_tell(vaht_mov* mov);

//! @}

#endif // __INCLUDE_VAHT_MOV_H__
