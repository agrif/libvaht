#ifndef __INCLUDE_VAHT_NAME_H__
#define __INCLUDE_VAHT_NAME_H__

#include <stdint.h>
#include "vaht_resource.h"

//! \defgroup vaht_name_group Name Indexes
//! Functions for Name Index Resources.  These functions can be used
//! to inspect NAME resources in a Mohawk archive. There are special
//! NAME resources that contain lists of names for other resource
//! types.
//! @{

struct vaht_name_s;

//! the name index data type

//! This data type refers to an individual name index. Use it as a
//! pointer only, because it is always dynamically allocated.
//! \sa vaht_name_open
typedef struct vaht_name_s vaht_name;

vaht_name* vaht_name_open(vaht_resource* resource);
void vaht_name_close(vaht_name* name);

//! @}

#endif // __INCLUDE_VAHT_NAME_H__
