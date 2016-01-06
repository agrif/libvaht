#include "vaht_intern.h"
#include <stdlib.h>

static inline void vaht_commands_free(vaht_command** commands)
{
    unsigned int i;
    for (i = 0; commands[i] != NULL; i++)
    {
        vaht_command* cmd = commands[i];
        if (cmd->is_branch)
        {
            unsigned int j;
            for (j = 0; j < cmd->branch.value_count; j++)
            {
                if (cmd->branch.commands[j])
                    vaht_commands_free(cmd->branch.commands[j]);
            }
            if (cmd->branch.value_count > 0)
            {
                free(cmd->branch.values);
                free(cmd->branch.commands);             
            }
        } else {
            if (cmd->cmd.args)
                free (cmd->cmd.args);
        }
        free(cmd);
    }
    free(commands);
}

static inline vaht_command** vaht_read_commands(vaht_resource* resource, uint16_t cmd_count)
{
    vaht_command** ret = calloc(cmd_count + 1, sizeof(vaht_command*));
    unsigned int i, j;
    for (i = 0; i < cmd_count; i++)
    {
        uint32_t read;
        uint16_t cmd, arg_count;
        read = vaht_resource_read(resource, sizeof(uint16_t), &cmd);
        read += vaht_resource_read(resource, sizeof(uint16_t), &arg_count);
        if (read != 2 * sizeof(uint16_t))
        {
            vaht_commands_free(ret);
            return NULL;
        }
        VAHT_SWAP_U16(cmd);
        VAHT_SWAP_U16(arg_count);
        
        uint16_t* args = NULL;
        if (arg_count > 0)
        {
            args = malloc(sizeof(uint16_t) * arg_count);
            read = vaht_resource_read(resource, sizeof(uint16_t) * arg_count, args);
            if (read != sizeof(uint16_t) * arg_count)
            {
                free(args);
                vaht_commands_free(ret);
                return NULL;
            }
        
            for (j = 0; j < arg_count; j++)
            {
                VAHT_SWAP_U16(args[j]);
            }
        }

        if (cmd != 8)
        {
            /* not a conditional! */
            ret[i] = malloc(sizeof(vaht_command));
            ret[i]->is_branch = 0;
            ret[i]->cmd.cmd = cmd;
            ret[i]->cmd.arg_count = arg_count;
            ret[i]->cmd.args = args;
        } else {
            /* it is a conditional */
            if (arg_count != 2)
            {
                if (args)
                    free(args);
                vaht_commands_free(ret);
                return NULL;
            }
            
            uint16_t var = args[0], value_count = args[1];
            free(args);
            uint16_t* values = NULL;
            vaht_command*** commands = NULL;
            if (value_count > 0)
            {
                values = malloc(sizeof(uint16_t) * value_count);
                commands = calloc(value_count, sizeof(vaht_command**));
            }
            
            ret[i] = malloc(sizeof(vaht_command));
            ret[i]->is_branch = 1;
            ret[i]->branch.var = var;
            ret[i]->branch.value_count = value_count;
            ret[i]->branch.values = values;
            ret[i]->branch.commands = commands;
            
            for (j = 0; j < value_count; j++)
            {
                uint16_t value, sub_cmd_count;
                read = vaht_resource_read(resource, sizeof(uint16_t), &value);
                read += vaht_resource_read(resource, sizeof(uint16_t), &sub_cmd_count);
                if (read != 2 * sizeof(uint16_t))
                {
                    vaht_commands_free(ret);
                    return NULL;
                }
                VAHT_SWAP_U16(value);
                VAHT_SWAP_U16(sub_cmd_count);
                
                values[j] = value;
                commands[j] = vaht_read_commands(resource, sub_cmd_count);
                if (commands[j] == NULL)
                {
                    /* conditional commands read falied */
                    vaht_commands_free(ret);
                    return NULL;
                }
            }
        }
    }
    
    return ret;
}

vaht_script* vaht_script_read(vaht_resource* resource)
{
    vaht_script* script = calloc(1, sizeof(vaht_script));
    
    uint32_t read;
    uint16_t handler_count;
    read = vaht_resource_read(resource, sizeof(uint16_t), &handler_count);
    if (read != sizeof(uint16_t))
    {
        vaht_script_free(script);
        return NULL;
    }
    VAHT_SWAP_U16(handler_count);
    if (!(handler_count < EVENT_COUNT))
    {
        vaht_script_free(script);
        return NULL;
    }
    
    unsigned int i;
    for (i = 0; i < handler_count; i++)
    {
        uint16_t event_type, cmd_count;
        read = vaht_resource_read(resource, sizeof(uint16_t), &event_type);
        read += vaht_resource_read(resource, sizeof(uint16_t), &cmd_count);
        if (read != 2 * sizeof(uint16_t))
        {
            vaht_script_free(script);
            return NULL;
        }
        VAHT_SWAP_U16(event_type);
        VAHT_SWAP_U16(cmd_count);
        
        if (!(event_type < EVENT_COUNT) || script->handlers[event_type] != NULL)
        {
            /* duplicate or bad event */
            vaht_script_free(script);
            return NULL;
        }
        
        script->handlers[event_type] = vaht_read_commands(resource, cmd_count);
        if (script->handlers[event_type] == NULL)
        {
            /* failed to read handler commands */
            vaht_script_free(script);
            return NULL;
        }
    }
    
    return script;
}

void vaht_script_free(vaht_script* script)
{
    unsigned int i;
    for (i = 0; i < EVENT_COUNT; i++)
    {
        if (script->handlers[i])
            vaht_commands_free(script->handlers[i]);        
    }
    free(script);
}


vaht_command** vaht_script_handler(vaht_script* script, enum vaht_script_event_t event)
{
    if (event >= EVENT_COUNT)
        return NULL;
    return script->handlers[event];
}

uint8_t vaht_command_branch(vaht_command* cmd)
{
    return cmd->is_branch;
}

uint16_t vaht_command_code(vaht_command* cmd)
{
    if (cmd->is_branch)
        return 8;
    return cmd->cmd.cmd;
}
uint16_t vaht_command_argument_count(vaht_command *cmd)
{
    if (cmd->is_branch)
        return 2;
    return cmd->cmd.arg_count;
}

uint16_t vaht_command_argument(vaht_command* cmd, uint16_t i)
{
    if (cmd->is_branch)
    {
        switch (i)
        {
        case 0:
            return cmd->branch.var;
        case 1:
            return cmd->branch.value_count;
        default:
            return 0;
        }
    } else {
        if (i >= cmd->cmd.arg_count)
            return 0;
        return cmd->cmd.args[i];
    }
    return 0;
}

uint16_t vaht_command_branch_variable(vaht_command *cmd)
{
    return cmd->branch.var;
}

uint16_t vaht_command_branch_count(vaht_command* cmd)
{
    return cmd->branch.value_count;
}

uint16_t vaht_command_branch_value(vaht_command* cmd, uint16_t i)
{
    if (i >= cmd->branch.value_count)
        return 0;
    return cmd->branch.values[i];
}

vaht_command** vaht_command_branch_body(vaht_command* cmd, uint16_t i)
{
    if (i >= cmd->branch.value_count)
        return NULL;
    return cmd->branch.commands[i];
}
