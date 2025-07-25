#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "app/app_cmd_def.h"
#include "common/common_def.h"

#include "datascript_parser.h"
#include "datafile/datafile_resources.h"
#include "scriptfile/scriptfile_regvals.h"

enum directive_types {
    DIRECTIVE_CHAR_NONE                             = 0,
    DIRECTIVE_CHAR_COMMENT                          = '#',
    DIRECTIVE_CHAR_DELIMITER                        = ' ',

    DIRECTIVE_CHAR_COMMAND_REFERENCE                = '$',
    DIRECTIVE_CHAR_COMMAND_ASSIGNMENT               = '=',

    /*----------------------------------------------*\
    |* format_3d object[2] = {1.0f, 0.0f, 0,0f},    *|
    |*                       {0.0f, 1.0f, 1.0f};    *|
    \*----------------------------------------------*/
    DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_START        = '[',
    DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_CLOSE        = ']',
    DIRECTIVE_CHAR_RESOURCE_MEMBER_ENTRY_DELIM      = ',',
    DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_START     = '{',
    DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_CLOSE     = '}',
    DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_START     = '=',
    DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE     = ';',
};

typedef struct {
    FILE *p_fstream;
    char *p_filedata_str;
    uint32_t filedata_len;
    uint32_t filedata_offset;
} datascript_ctx_t;

typedef struct datascript_entry {
    datascript_ctx_t ctx;
    struct datascript_entry *p_prev;
} datascript_entry_t, *datascript_list_t;

static datascript_list_t datafile_list = NULL;
static datascript_list_t scriptfile_list = NULL;

static uint32_t __datascript_get_file_length(FILE *p_fstream)
{
    uint32_t file_len;
    fpos_t pos;

    // TODO: divide with units of size_t(=SIZE_MAX)
    if (fseek(p_fstream, 0, SEEK_END) != 0) {
        return 0;
    }
    if (fgetpos(p_fstream, &pos) != 0) {
        return 0;
    }

    file_len = pos.__pos;

    pos.__pos = 0;
    if (fsetpos(p_fstream, &pos) != 0) {
        return 0;
    }

    return file_len;
}

static uint32_t __datascript_setup_datascript_ctx(char *filename_str, datascript_ctx_t *p_ctx)
{
    uint32_t file_len;

    p_ctx->p_fstream = fopen(filename_str, "r");
    if (p_ctx->p_fstream == NULL) {
        printf("check filename: %s\n", filename_str);
        return FAILURE;
    }

    file_len = __datascript_get_file_length(p_ctx->p_fstream);
    if (file_len == 0) {
        printf("file length is invalid(%u)\n", file_len);
        return FAILURE;
    }

    p_ctx->p_filedata_str = malloc(file_len);

    if (fread(p_ctx->p_filedata_str, file_len, 1, p_ctx->p_fstream) == 0) {
        printf("file read failure\n");
        return FAILURE;
    }

    p_ctx->filedata_offset = 0;
    p_ctx->filedata_len = file_len;

    fclose(p_ctx->p_fstream);

    return SUCCESS;
}

uint32_t datascript_register_scriptfile(char *filename_str)
{
    datascript_entry_t *p_datascript;

    p_datascript = (datascript_entry_t *)malloc(sizeof(datascript_entry_t));

    if (__datascript_setup_datascript_ctx(filename_str, &p_datascript->ctx) == FAILURE) {
        free(p_datascript);
        return FAILURE;
    }

    p_datascript->p_prev = scriptfile_list;

    scriptfile_list = p_datascript;

    return SUCCESS;
}

uint32_t datascript_check_scriptfile_registered(void)
{
    if (scriptfile_list == NULL) {
        return FALSE;
    }

    return TRUE;
}

static char __datascript_search_runscript_directives(char *p_input_str)
{
    if (strchr(p_input_str, DIRECTIVE_CHAR_COMMENT)) {
        return DIRECTIVE_CHAR_COMMENT;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_COMMAND_REFERENCE)) {
        return DIRECTIVE_CHAR_COMMAND_REFERENCE;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_COMMAND_ASSIGNMENT)) {
        return DIRECTIVE_CHAR_COMMAND_ASSIGNMENT;
    }

    return DIRECTIVE_CHAR_NONE;
}

static uint32_t __datascript_replace_directive_to_regval(char *p_cmdstr)
{
    uint32_t regval_len;
    uint32_t directive_len;
    uint32_t replace_str_offset;

    char *cursor;
    char *regval_str;

    cursor = strchr(p_cmdstr, DIRECTIVE_CHAR_COMMAND_REFERENCE);

    regval_str = scriptfile_get_regval_entry_value(cursor);
    if (regval_str == NULL) {
        printf("argname not registered\n");
        return FAILURE;
    }

    regval_len = strlen(regval_str);

    char replace_str[regval_len + strlen(p_cmdstr)];
    memset(replace_str, 0, regval_len + strlen(p_cmdstr));

    replace_str_offset = 0;
    strncpy(&replace_str[replace_str_offset], p_cmdstr, cursor - p_cmdstr);

    replace_str_offset += cursor - p_cmdstr;
    strncpy(&replace_str[replace_str_offset], regval_str, regval_len);

    if (strchr(cursor, DIRECTIVE_CHAR_DELIMITER) != NULL) {
        cursor = strchr(cursor, DIRECTIVE_CHAR_DELIMITER);
    }
    else {
        cursor = strchr(cursor, '\n');
    }

    replace_str_offset += regval_len;
    strncpy(&replace_str[replace_str_offset], cursor, strlen(cursor));

    replace_str_offset += strlen(cursor);
    replace_str[replace_str_offset] = '\0';

    strcpy(p_cmdstr, replace_str);

    return SUCCESS;
}

static uint32_t __datascript_process_runscript_directives(char *p_cmdstr)
{
    uint32_t regval_len;

    char directive_type;
    char *p_cursor;

    p_cursor = p_cmdstr;

replace_directive_start:
    directive_type = __datascript_search_runscript_directives(p_cursor);

    switch (directive_type) {
        case DIRECTIVE_CHAR_COMMAND_REFERENCE:
            if (__datascript_replace_directive_to_regval(p_cmdstr) == FAILURE) {
                return FAILURE;
            }
            break;

        case DIRECTIVE_CHAR_COMMAND_ASSIGNMENT:
            // process later
            regval_len = strchr(p_cursor, DIRECTIVE_CHAR_COMMAND_ASSIGNMENT) - p_cursor;
            p_cursor = &p_cursor[regval_len + 1];

            goto replace_directive_start;

        case DIRECTIVE_CHAR_NONE:
            break;

        default:
            printf("directive type %u is not handled\n", directive_type);
            return FAILURE;
    }

    if (directive_type != DIRECTIVE_CHAR_NONE) {
        goto replace_directive_start;
    }

    return SUCCESS;
}

static uint32_t __datascript_get_next_cmdstr_from_scriptfile(char *buf_str)
{
    uint64_t cmd_strlen;
    uint32_t filedata_offset;
    char *p_runscript_data_str;

    filedata_offset = scriptfile_list->ctx.filedata_offset;

get_new_cmdstr:
    memset(buf_str, 0, strlen(buf_str));

    p_runscript_data_str = &scriptfile_list->ctx.p_filedata_str[filedata_offset];

    if (strchr(p_runscript_data_str, '\n') != NULL) {
        cmd_strlen = (uint64_t)(strchr(p_runscript_data_str, '\n') - p_runscript_data_str);
    }
    else {
        cmd_strlen = strlen(p_runscript_data_str);

        if (cmd_strlen == 0) {
            printf("EOF\n");

            return FAILURE;
        }
    }

    cmd_strlen++; // include eol char

    filedata_offset += cmd_strlen;

    if (cmd_strlen == 1) {
        goto get_new_cmdstr;
    }

    scriptfile_list->ctx.filedata_offset = filedata_offset;

    strncpy(buf_str, p_runscript_data_str, cmd_strlen);
    buf_str[cmd_strlen] = '\0';

    return SUCCESS;
}

static void __datascript_release_current_entry(datascript_list_t *p_list)
{
    datascript_entry_t *p_entry_prev;

    p_entry_prev = ((datascript_entry_t *)*p_list)->p_prev;

    free(((datascript_entry_t *)*p_list)->ctx.p_filedata_str);
    free(*p_list);

    *p_list = p_entry_prev;

    return;
}

static uint32_t __datascript_release_current_scriptfile_entry(void)
{
    scriptfile_release_entries(scriptfile_list->ctx.p_fstream);

    __datascript_release_current_entry(&scriptfile_list);

    return SUCCESS;
}

uint32_t datascript_get_command(char *input_str)
{
    uint32_t directive_type;

get_new_command:
    if (__datascript_get_next_cmdstr_from_scriptfile(input_str) == FAILURE) {
        __datascript_release_current_scriptfile_entry();

        return FAILURE;
    }

    directive_type = __datascript_search_runscript_directives(input_str);

    if (directive_type == DIRECTIVE_CHAR_COMMENT) {
        goto get_new_command;
    }
    else if (__datascript_process_runscript_directives(input_str) == FAILURE) {
        printf("directive processing failure\n");
        return FAILURE;
    }

    printf("%s\n", input_str);

    return SUCCESS;
}

uint32_t datascript_save_command_result(char *argname_str, uint32_t retval)
{
    char argname_ref_str[strlen(argname_str) + 1];

    argname_ref_str[0] = DIRECTIVE_CHAR_COMMAND_REFERENCE;

    strcpy(&argname_ref_str[1], argname_str);

    return scriptfile_save_command_regval_result(argname_ref_str, retval,
                                                    scriptfile_list->ctx.p_fstream);
}

uint32_t datascript_register_datafile(char *filename_str)
{
    datascript_entry_t *p_datascript;

    p_datascript = (datascript_entry_t *)malloc(sizeof(datascript_entry_t));

    if (__datascript_setup_datascript_ctx(filename_str, &p_datascript->ctx) == FAILURE) {
        free(p_datascript);
        return FAILURE;
    }

    p_datascript->p_prev = datafile_list;

    datafile_list = p_datascript;

    return SUCCESS;
}

uint32_t datascript_check_resource_registered(char *argname_str)
{
    if (datafile_get_resource_entry_idx(argname_str) == MAX_NUM_RESOURCE_OBJECT_MEMBERS) {
        return FALSE;
    }

    return TRUE;
}

uint32_t datascript_check_datafile_registered(void)
{
    if (datafile_list == NULL) {
        return FALSE;
    }

    return TRUE;
}

static char __datascript_search_datafile_directives(char *p_input_str)
{
    if (strchr(p_input_str, DIRECTIVE_CHAR_COMMENT)) {
        return DIRECTIVE_CHAR_COMMENT;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_START)) {
        return DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_START;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE)) {
        return DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_START)) {
        return DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_START;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_CLOSE)) {
        return DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_CLOSE;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_START)) {
        return DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_START;
    }

    if (strchr(p_input_str, DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_CLOSE)) {
        return DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_CLOSE;
    }

    return DIRECTIVE_CHAR_NONE;
}

static uint32_t __datascript_get_next_objstr_from_datafile(char *buf_str)
{
    uint64_t cmd_strlen;
    uint32_t filedata_offset;
    char *p_datafile_data_str;

    filedata_offset = datafile_list->ctx.filedata_offset;

    memset(buf_str, 0, strlen(buf_str));

get_new_object_data:
    p_datafile_data_str = &datafile_list->ctx.p_filedata_str[filedata_offset];

    if (p_datafile_data_str[0] == '\n') {
        filedata_offset++;
        goto get_new_object_data;
    }

    if (__datascript_search_datafile_directives(p_datafile_data_str) == DIRECTIVE_CHAR_COMMENT) {
        cmd_strlen = strchr(p_datafile_data_str, '\n') - p_datafile_data_str;
        filedata_offset += (cmd_strlen + 1);

        goto get_new_object_data;
    }

    if (strchr(p_datafile_data_str, DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE) == NULL) {
        printf("EOF\n");

        return FAILURE;
    }

    cmd_strlen = (strchr(p_datafile_data_str, DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE)
                                                                    - p_datafile_data_str);
    cmd_strlen++; // include eol char

    filedata_offset += cmd_strlen;

    datafile_list->ctx.filedata_offset = filedata_offset;

    strncpy(buf_str, p_datafile_data_str, cmd_strlen);
    buf_str[cmd_strlen] = '\0';

    return SUCCESS;
}

static uint32_t __datascript_setup_resource_info_ctx(resource_info_t *p_info, char *data_buf_str)
{
    uint32_t copy_str_len;
    char *p_cursor;
    char argstr[MAX_LENGTH_ARGUMENT_NAME];

    // get object type
    p_cursor = data_buf_str;
    copy_str_len = strchr(p_cursor, DIRECTIVE_CHAR_DELIMITER) - p_cursor;

    strncpy(argstr, p_cursor, copy_str_len);
    argstr[copy_str_len] = '\0';

    p_info->type = datafile_get_resource_type_from_typename(argstr);
    if (p_info->type == RESOURCE_FORMAT_TYPE_INVALID) {
        return FAILURE;
    }

    p_info->usage_flags = datafile_get_usage_flags_from_resource_type(p_info->type);
    if (p_info->usage_flags == 0) {
        return FAILURE;
    }

    // get object name
    p_cursor = strchr(p_cursor, DIRECTIVE_CHAR_DELIMITER);
    if (p_cursor == NULL) {
        printf("resource name not recognized\n");
        return FAILURE;
    }

    p_cursor++;
    copy_str_len = strchr(p_cursor, DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_START) - p_cursor;
    strncpy(p_info->name, p_cursor, copy_str_len);
    p_info->name[copy_str_len] = '\0';

    // get array count
    p_cursor = strchr(p_cursor, DIRECTIVE_CHAR_RESOURCE_ARRAY_SIZE_START);
    if (p_cursor == NULL) {
        printf("resource name not recognized\n");
        return FAILURE;
    }

    p_cursor++;
    p_info->count = *p_cursor - '0';

    return SUCCESS;
}

static uint32_t __datascript_register_resource_entries(resource_info_t *p_info, char *data_buf_str)
{
    uint32_t value_strlen;
    uint32_t value_entry_idx;
    uint32_t value_member_idx;

    char *p_cursor;
    char *p_cursor_limit;
    resource_member_entry_t member_value_strbuf[MAX_NUM_RESOURCE_OBJECT_MEMBERS];

    p_cursor = data_buf_str;

    value_member_idx = 0;

register_member_values:
    p_cursor = strchr(p_cursor, DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_START); // '{'
    p_cursor_limit = strchr(p_cursor, DIRECTIVE_CHAR_RESOURCE_MEMBER_DEFINE_CLOSE); // '}'

    value_entry_idx = 0;
    while (p_cursor_limit > p_cursor) {
        p_cursor++; // skip directive

        while (*p_cursor == DIRECTIVE_CHAR_DELIMITER) {
            p_cursor++;
        }

        if (strchr(p_cursor, DIRECTIVE_CHAR_RESOURCE_MEMBER_ENTRY_DELIM) != NULL) {
            value_strlen = strchr(p_cursor, DIRECTIVE_CHAR_RESOURCE_MEMBER_ENTRY_DELIM) - p_cursor;

            if (value_strlen + p_cursor > p_cursor_limit) {
                value_strlen = p_cursor_limit - p_cursor;
            }
        }
        else {
            value_strlen = p_cursor_limit - p_cursor;
        }

        memset(member_value_strbuf[value_entry_idx].value, 0, MAX_RESOURCE_VALUE_LENGTH_STR);
        strncpy(member_value_strbuf[value_entry_idx].value, p_cursor, value_strlen);
        member_value_strbuf[value_entry_idx].value[value_strlen] = '\0';

        p_cursor += value_strlen;

        value_entry_idx++;
    }

    if (datafile_save_resource_data(p_info, member_value_strbuf,
                                    value_entry_idx, value_member_idx) == FAILURE) {
        return FAILURE;
    }

    value_member_idx++;

    if (value_member_idx < p_info->count) {
        goto register_member_values;
    }

    return SUCCESS;
}

static uint32_t __datascript_release_current_datafile_entry(void)
{
    __datascript_release_current_entry(&datafile_list);

    return SUCCESS;
}

uint32_t datascript_get_resource_count(void)
{
    char *p_cursor;
    uint32_t resource_count;

    resource_count = 0;

    p_cursor = strchr(datafile_list->ctx.p_filedata_str, DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE);
    while (p_cursor != NULL) {
        resource_count++;
        p_cursor = strchr(++p_cursor, DIRECTIVE_CHAR_RESOURCE_OBJECT_DEFINE_CLOSE);
    }

    return resource_count;
}

uint32_t datascript_load_resource_info_from_datafile(resource_info_t *info_list)
{
    uint32_t list_idx;
    char data_buf_str[300];

    list_idx = 0;

get_new_datastr:
    memset(data_buf_str, 0, 300);

    if (__datascript_get_next_objstr_from_datafile(data_buf_str) == SUCCESS) {
        memset(&info_list[list_idx], 0, sizeof(resource_info_t));

        if (__datascript_setup_resource_info_ctx(&info_list[list_idx], data_buf_str) == FAILURE) {
            return FAILURE;
        }

        if (datafile_allocate_resource_data_entry(&info_list[list_idx]) == FAILURE) {
            return FAILURE;
        }

        info_list[list_idx].binding = datafile_get_resource_entry_idx(info_list[list_idx].name);
        if (info_list[list_idx].binding == MAX_NUM_RESOURCE_OBJECTS) {
            printf("resource entry fully occupied\n");
            return FAILURE;
        }

        if (__datascript_register_resource_entries(&info_list[list_idx], data_buf_str) == FAILURE) {
            return FAILURE;
        }

        list_idx++;

        goto get_new_datastr;
    }
    else {
        __datascript_release_current_datafile_entry();
    }

    // EOF
    return SUCCESS;
}

resource_member_list_t *datascript_get_resource_data(char *resource_name)
{
    return datafile_get_resource_data(resource_name);
}

void datascript_release_all_files(void)
{
    while (scriptfile_list != NULL) {
        __datascript_release_current_scriptfile_entry();
    }

    while (datafile_list != NULL) {
        __datascript_release_current_datafile_entry();
    }
}
