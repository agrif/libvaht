#ifndef __INCLUDE_VAHT_SCRIPT_H__
#define __INCLUDE_VAHT_SCRIPT_H__

#include <stdint.h>
#include "vaht_resource.h"

/**
 * \defgroup vaht_script_group Scripts
 * Functions for scripts. These let you parse and inspect scripts
 * attached to CARDS and HSPT resources (and possibly others).  @{
 */

struct vaht_script_s;

/**
 * \brief the script data type
 *
 * This data type refers to a set of script handlers, with associated
 * command lists. Use it as a pointer only, because it is always
 * dynamically allocated.
 *
 * \sa vaht_script_read
 */
typedef struct vaht_script_s vaht_script;

/**
 * \brief a list of event types for handlers
 *
 * Every script is a list of handlers, where each handler has an
 * associated event. These are the possible event types.
 */
enum vaht_script_event_t
{
    EVENT_MOUSE_DOWN = 0,
    EVENT_MOUSE_STILL_DOWN = 1,
    EVENT_MOUSE_UP = 2,
    EVENT_MOUSE_ENTER = 3,
    EVENT_MOUSE_WITHIN = 4,
    EVENT_MOUSE_LEAVE = 5,
    EVENT_LOAD_CARD = 6,
    EVENT_CLOSE_CARD = 7,
    EVENT_OPEN_CARD = 9,
    EVENT_DISPLAY_UPDATE = 10,
    
    EVENT_COUNT
};

struct vaht_command_s;

/**
 * \brief an object representing a single command
 *
 * This object represents a single command inside a script
 * handler. You should always use this as a pointer, since it is
 * always dynamically allocated. Usually, you will see these in
 * NULL-terminated arrays of pointers.
 */
typedef struct vaht_command_s vaht_command;

/**
 * \brief read a script block from a resource
 *
 * This function will read the script block from the current seek
 * position of the given resource. If the read script is invalid, this
 * returns NULL. This reads in the entire script at once; it will not
 * hold the resource open. You must free the script when you are done
 * with it.
 *
 * \param resource the resource to read the script block from
 * \return the script object, or NULL
 * \sa vaht_script_free
 */
vaht_script* vaht_script_read(vaht_resource* resource);

/**
 * \brief free the script object
 *
 * This function clears any memory used by a script object. Use it
 * when you are done with one.
 *
 * \param script the script object to close
 * \sa vaht_script_read
 */
void vaht_script_free(vaht_script* script);

/**
 * \brief get the commands for a given event handler
 *
 * Returns a list of commands in the handler for the given event. If
 * there are no commands for that event, returns NULL. The returned
 * list is NULL-terminated, and is owned by the script object (so it
 * must not be freed).
 *
 * \param script the script object
 * \param event the event to get a handler for
 * \return a NULL-terminated list of commands, or NULL
 */
vaht_command** vaht_script_handler(vaht_script* script, enum vaht_script_event_t event);

/**
 * \brief returns whether the given command is a conditional branch
 *
 * If the given command is a branch, this returns 1. Otherwise, it
 * returns 0. Branch commands can be inspected with
 * vaht_command_branch_* functions.
 *
 * \param cmd the command to inspect
 * \return 1 if the command is a branch, 0 otherwise.
 * \sa vaht_command_branch_variable vaht_command_branch_count
 * \sa vaht_command_branch_value vaht_command_branch_body
 */
uint8_t vaht_command_branch(vaht_command* cmd);

/**
 * \brief get the command opcode
 *
 * This returns the opcode for the given command.
 *
 * \param cmd the command to inspect
 * \return the opcode for the command
 */
uint16_t vaht_command_code(vaht_command* cmd);

/**
 * \brief get the number of arguments for a command
 *
 * This returns the number of arguments present for this command.
 *
 * \param cmd the command to inspect
 * \return the number of arguments present in this command
 * \sa vaht_command_argument
 */
uint16_t vaht_command_argument_count(vaht_command *cmd);

/**
 * \brief get an argument for a given command
 *
 * This returns the argument in position i for a given command.
 *
 * \param cmd the command to inspect
 * \param i the index of the argument to get
 * \return the argument value
 * \sa vaht_command_argument_count
 */
uint16_t vaht_command_argument(vaht_command* cmd, uint16_t i);

/**
 * \brief the variable id to branch on
 *
 * This returns the index in NAME record 4 of the variable this
 * conditional depends on. If this is not a conditional command,
 * behaviour is undefined.
 *
 * \param cmd the command to inspect
 * \return an index into NAME record 4
 * \sa vaht_command_branch
 */
uint16_t vaht_command_branch_variable(vaht_command *cmd);

/**
 * \brief the number of cases in this conditional
 *
 * Returns the number of different cases in this conditional
 * command. If this is not a conditional command, behaviour is
 * undefined.
 *
 * \param cmd the command to inspect
 * \return the number of cases in this conditional
 * \sa vaht_command_branch vaht_command_branch_value vaht_command_branch_body
 */
uint16_t vaht_command_branch_count(vaht_command* cmd);

/**
 * \brief get the value for a conditional case
 *
 * This returns the value to check for in a given conditional case. If
 * this value is 0xffff, this acts as a default case. If this command
 * is not a conditional, behaviour is undefined.
 *
 * \param cmd the command to inspect
 * \param i the index of the case
 * \return the value to check for, or 0xffff for default
 * \sa vaht_command_branch vaht_command_branch_count
 */
uint16_t vaht_command_branch_value(vaht_command* cmd, uint16_t i);

/**
 * \brief get the commands for a given conditional case
 *
 * This returns a NULL-terminated list of commands for a given case,
 * or NULL if there are no commands. If this command is not a
 * conditional, behaviour is undefined.
 *
 * \param cmd the command to inspect
 * \param i the index of the case
 * \return a NULL-terminated list of commands, or NULL
 * \sa vaht_command_branch vaht_command_branch_count
 */
vaht_command** vaht_command_branch_body(vaht_command* cmd, uint16_t i);

/**
 * @}
 */

#endif /* __INCLUDE_VAHT_SCRIPT_H__ */
