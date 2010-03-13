#ifndef __INCLUDE_VAHT_ARCHIVE_H__
#define __INCLUDE_VAHT_ARCHIVE_H__

#include <stdint.h>

//! \defgroup vaht_archive_group Archives
//! Archives are the objects that represent individual mohawk files.
//! From these, you can open individual Resources, and from those you can read
//! out all the data you want.
//! @{

struct vaht_archive_s;

//! the archive data type

//! This is a typedef to avoid using structs everywhere in sources.
//! This will always be dynamically allocated, so only refer to these
//! as pointers.
//! \sa vaht_archive_open
typedef struct vaht_archive_s vaht_archive;

//! open a mohawk archive

//! This function will open a mohawk archive, given the filename. It will return
//! NULL if the file couldn't be opened, or if the file is not a valid mohawk
//! archive.
//!
//! These have to be closed.
//!
//! \param filename the file to open
//! \return a \ref vaht_archive or NULL
//! \sa vaht_archive_close
vaht_archive* vaht_archive_open(const char* filename);

//! close a mohawk archive

//! Use this to close open mohawk archives, or release grabbed archives.
//! Basically, use this to signal you are done with the archive.
//! \param archive the archive to close
//! \return the number of remaining owners of this archive
//! \sa vaht_archive_open vaht_archive_grab
uint16_t vaht_archive_close(vaht_archive* archive);

//! claim ownership of an archive

//! This function will prevent the archive from being closed by anything,
//! until you release it by calling \ref vaht_archive_close . This is
//! a simple memory management technique.
//! \param archive the archive to grab
//! \return the number of owners of this archive
//! \sa vaht_archive_close
uint16_t vaht_archive_grab(vaht_archive* archive);

//! get the number of resource types

//! This function will tell you how many different types of resources are
//! stored in this archive. You will need to use 
//! \ref vaht_archive_get_resource_type to get the resource type strings from
//! this archive.
//! \param archive the archive to get resource types from
//! \return the number of resource types in this file
//! \sa vaht_archive_get_resource_type
uint16_t vaht_archive_get_resource_types(vaht_archive* archive);

//! get the resource type string for a resource type

//! This is how you get the string version of types (ex. "tBMP") for this
//! archive. The index is less than the number of types in this archive.
//! \param archive the archive to get resource type string from
//! \param i the index of the resource type to get
//! \return a string type name (ex. "tBMP")
//! \sa vaht_archive_get_resource_types
char* vaht_archive_get_resource_type(vaht_archive* archive, uint16_t i);

//! @}

#endif // __INCLUDE_VAHT_ARCHIVE_H__
