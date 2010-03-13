#ifndef __INCLUDE_VAHT_RESOURCE_H__
#define __INCLUDE_VAHT_RESOURCE_H__

#include <stdint.h>
#include "vaht_archive.h"

//! \defgroup vaht_resource_group Resources
//! Resources represent each data block in an archive.
//! You can think of resources as files, each one represented by a type name
//! and an ID.
//! @{

struct vaht_resource_s;

//! the resource data type

//! This data type refers to an individual resource. Use it as a pointer only,
//! because it is always dynamically allocated.
//! \sa vaht_resource_open
typedef struct vaht_resource_s vaht_resource;

//! open a resource

//! This function will let you open a resource from an opened archive. It will
//! return null if the resource does not exist. You must close these resources
//! after you are done with them.
//! \param archive the archive to open a resource from
//! \param type a string identifying the type of resource
//! \param id the id of the resource to open
//! \return an opened resource, or NULL
//! \sa vaht_resource_close vaht_archive_type
vaht_resource* vaht_resource_open(vaht_archive* archive, const char* type, uint16_t id);

//! close an opened resource

//! This function will release the given resource, and close it if everything
//! is done using it. Think of it as a signal that you are done with it. Every
//! call to \ref vaht_resource_open or \ref vaht_resource_grab must be matched
//! with a call to this.
//! \param resource the resource to close
//! \return the number of remaining owners of this resource
//! \sa vaht_resource_open vaht_resource_grab
uint16_t vaht_resource_close(vaht_resource* resource);

//! claim ownership of an archive

//! This function will prevent the given resource from being destroyed until
//! you call \ref vaht_resource_close . Use it whenever you didn't open the
//! resource, but you want to use it for any length of time. It's a simple
//! memory management technique, but you must always remember to close it!
//! \param resource the resource to retain
//! \return the number of owners of this resource
//! \sa vaht_resource_close
uint16_t vaht_resource_grab(vaht_resource* resource);

//! open multiple resources with one type

//! Use this function to open all the resources of a single type in an archive.
//! The return is a NULL terminated array of resources, or NULL itself on
//! error. These can all be closed individually, or with the convenience
//! function \ref vaht_resources_close .
//! \param archive the archive to open resources from
//! \param type the type of resources to open
//! \return A NULL terminated array of resources, or NULL
//! \sa vaht_resources_close vaht_resource_open
vaht_resource** vaht_resources_open(vaht_archive* archive, const char* type);

//! close multiple resources

//! This function will close a list of resources opened by
//! \ref vaht_resources_open , or any NULL terminated array of resources.
//! Always remember to close your resources!
//! \param resources the NULL terminated array of resources to close
//! \sa vaht_resources_open vaht_resource_close
void vaht_resources_close(vaht_resource** resources);

//! get the name of a resource

//! This function will retreive a resource's name. Not all resources usually
//! have meaningful names. This function will return a 0 length string for
//! most resources, so be prepared for that.
//! \param resource the resource to get a name for
//! \return the resource's name
//! \sa vaht_resource_type vaht_resource_id
char* vaht_resource_name(vaht_resource* resource);

//! get the resource type

//! This function will get a particular resource's type. This type is a NULL
//! terminated C string with 4 characters.
//! \param resource the resource who's type you want
//! \return the resource's type
//! \sa vaht_resource_name vaht_resource_id
char* vaht_resource_type(vaht_resource* resource);

//! get the resource ID

//! This will get the resource ID for the given resource. It's unique only
//! to within the same resource type.
//! \param resource the resource who's ID you want
//! \return the resource's ID
//! \sa vaht_resource_name vaht_resource_type
uint16_t vaht_resource_id(vaht_resource* resource);

//! get the resource's size

//! This function will tell you how many bytes of data are in the resource.
//! This can be useful in conjunction with the other read functions.
//! \param resource the resource to get the size of
//! \return the resource's size
//! \sa vaht_resource_read vaht_resource_seek
uint32_t vaht_resource_size(vaht_resource* resource);

//! read data from a resource

//! This function reads data from a resource into the given buffer, and returns
//! how many bytes are read. Bytes that are put in the buffer are not modified
//! at all; if you need to adjust for endianness, you need to do so yourself.
//! If a NULL is given as the buffer, the read will act like a seek.
//! \param resource the resource to read
//! \param size the number of bytes to read
//! \param buffer a buffer big enouch to hold the bytes read
//! \return the total number of bytes read (can be less than size!)
//! \sa vaht_resource_size vaht_resource_seek
uint32_t vaht_resource_read(vaht_resource* resource, uint32_t size, void* buffer);

//! seek to a new position

//! This function will move the read cursor to somewhere specific in the
//! resource, given in bytes from the start of the file. Use
//! \ref vaht_resource_tell to get the current position.
//! \param resource the resource to seek through
//! \param seek the new position to set
//! \sa vaht_resource_tell vaht_resource_size
void vaht_resource_seek(vaht_resource* resource, uint32_t seek);

//! return the current read position

//! This function is the opposite of \ref vaht_resource_seek . It will tell
//! you where you are reading from, in bytes since the start of the file.
//! \param resource the resource to get the read position from
//! \return the read position, in bytes since the start of the file
//! \sa vaht_resource_seek vaht_resource_size
uint32_t vaht_resource_tell(vaht_resource* resource);

//! @}

#endif // __INCLUDE_VAHT_RESOURCE_H__
