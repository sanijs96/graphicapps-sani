#ifndef __APPS_CMD_DEF_H__
#define __APPS_CMD_DEF_H__

#include <stdint.h>

#define APPS_CMD_TYPE_SHIFT     (16)
#define APPS_CMD_ID_SHIFT       (8)

#define OPCODE_VK(cmd_id, subcmd_id)                    \
            _APPS_CMD_OPCODE(APPS_CMD_TYPE_VULKAN,      \
                            CMD_V(cmd_id), SUBCMD_V(cmd_id, subcmd_id))

#define _APPS_CMD_OPCODE(cmd_type, cmd_id, subcmd_id)        \
                        (cmd_type   << APPS_CMD_TYPE_SHIFT | \
                         cmd_id     << APPS_CMD_ID_SHIFT   | \
                         subcmd_id)

#define OPCODE_ETC(cmd_id, subcmd_id)           \
        _APPS_CMD_OPCODE(APPS_CMD_TYPE_ETC,     \
                            CMD_E(cmd_id), SUBCMD_E(cmd_id, subcmd_id))


#define CMD_V(cid)              APPS_CMD_ID_VULKAN_ ## cid
#define SUBCMD_V(cid, subcid)   APPS_SUBCMD_ID_VULKAN_ ## cid ## _ ## subcid

#define CMD_E(cid)              APPS_CMD_ID_ETC_ ## cid
#define SUBCMD_E(cid, subcid)   APPS_SUBCMD_ID_ETC_ ## cid ## _ ## subcid

enum apps_cmd_type {
    APPS_CMD_TYPE_VULKAN,
    APPS_CMD_TYPE_ETC,
};

enum apps_cmd_id_etc {
    APPS_CMD_ID_ETC_WINDOW,
    APPS_CMD_ID_ETC_CONSOLE,
};

enum apps_subcmd_id_etc {
    APPS_SUBCMD_ID_ETC_WINDOW_RESIZE,
    APPS_SUBCMD_ID_ETC_CONSOLE_EXIT,
};

typedef struct __command_arg {
    uint8_t type;
    char *value;
} command_arg_t;

typedef struct __command {
    char *input;

    struct {
        uint32_t opcode;
        uint32_t num_args;
        command_arg_t *p_args;
    };
} command_t;

#endif
