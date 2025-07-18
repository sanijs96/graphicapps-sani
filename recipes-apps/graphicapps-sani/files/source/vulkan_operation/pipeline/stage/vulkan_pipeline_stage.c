#include <stdio.h>
#include <malloc.h>

#include "common/common_def.h"
#include "vulkan/pipeline_stages.h"

#include "vulkan_pipeline_stage.h"

typedef struct stage_ctx {
    uint32_t state;
    const char * const name;
    pipeline_stage_template_t setting;
} stage_ctx_t;

const VkDynamicState dynamic_state[NUM_VULKAN_PIPELINE_DYNAMIC_STATES] = {
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR
};

const VkPipelineColorBlendAttachmentState default_attachment_state = {
    .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,

    .blendEnable = VK_TRUE,

    .srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
    .dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    .colorBlendOp = VK_BLEND_OP_ADD,

    .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
    .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
    .alphaBlendOp = VK_BLEND_OP_ADD
};

stage_ctx_t stages[NUM_VULKAN_PIPELINE_STAGES] = {
[VULKAN_PIPELINE_STAGE_VERTEX_SHADER] = {
    .name = "shader_vertex",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.vertex_shader = { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0,
                                    .stage = VK_SHADER_STAGE_VERTEX_BIT,
                                    .pName = NULL,
                                    .pSpecializationInfo = NULL }
},
[VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER] = {
    .name = "shader_fragment",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.fragment_shader = { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0,
                                    .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                                    .pName = NULL,
                                    .pSpecializationInfo = NULL }
},
[VULKAN_PIPELINE_STAGE_VERTEX_INPUT] = {
    .name = "input_vertex",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.vertex_input = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0, 
                                    .vertexBindingDescriptionCount = 0,
                                    .pVertexBindingDescriptions = NULL,
                                    .vertexAttributeDescriptionCount = 0,
                                    .pVertexAttributeDescriptions = NULL }
},
[VULKAN_PIPELINE_STAGE_INPUT_ASSEMBLY] = {
    .name = "input_assembly",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.input_assembly = { .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0,
                                    .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                                    .primitiveRestartEnable = VK_FALSE }
},
[VULKAN_PIPELINE_STAGE_VIEWPORT] = {
    .name = "viewport",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.viewport = { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0,
                                    .viewportCount = 0,
                                    .pViewports = NULL,
                                    .scissorCount = 0,
                                    .pScissors = NULL }

},
[VULKAN_PIPELINE_STAGE_DYNAMIC] = {
    .name = "dynamic",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.dynamic = { .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0,
                                    .dynamicStateCount = NUM_VULKAN_PIPELINE_DYNAMIC_STATES,
                                    .pDynamicStates = dynamic_state}
},
[VULKAN_PIPELINE_STAGE_RASTERIZER] = {
    .name = "rasterizer",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.rasterizer = { .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
                                    .pNext = NULL, .depthClampEnable = VK_FALSE,
                                    .rasterizerDiscardEnable = VK_FALSE,
                                    .polygonMode = VK_POLYGON_MODE_FILL,
                                    .cullMode = VK_CULL_MODE_BACK_BIT,
                                    .frontFace = VK_FRONT_FACE_CLOCKWISE,
                                    .depthBiasEnable = VK_FALSE,
                                    .depthBiasConstantFactor = 0.0f,
                                    .depthBiasClamp = 0.0f,
                                    .depthBiasSlopeFactor = 0.0f,
                                    .lineWidth = 1.0f }
},
[VULKAN_PIPELINE_STAGE_MULTISAMPLING] = {
    .name = "multisampling",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.multisampling = { .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0,
                                    .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
                                    .sampleShadingEnable = VK_FALSE,
                                    .minSampleShading = 1.0f,
                                    .pSampleMask = NULL,
                                    .alphaToCoverageEnable = VK_FALSE,
                                    .alphaToOneEnable = VK_FALSE }
},
[VULKAN_PIPELINE_STAGE_DEPTH_STENCIL] = {
    .name = "depth_stencil",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
},
[VULKAN_PIPELINE_STAGE_COLOR_BLENDING] = {
    .name = "color_blending",
    .state = VULKAN_PIPELINE_STAGE_STATE_DEFAULT,
    .setting.color_blend = { .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
                                    .pNext = NULL, .flags = 0, 
                                    .logicOpEnable = VK_FALSE,
                                    .logicOp = VK_LOGIC_OP_COPY,
                                    .attachmentCount = 1,
                                    .pAttachments = &default_attachment_state,
                                    .blendConstants = {0.0f, 0.0f, 0.0f, 0.0f}, }
},
};

uint32_t pipeline_stage_get_status(uint32_t stage_idx)
{
    return stages[stage_idx].state;
}

char *pipeline_stage_get_stage_name(uint32_t stage_idx)
{
    return (char *)stages[stage_idx].name;
}

pipeline_stage_template_t *pipeline_stage_get_creation_info(uint32_t stage_idx)
{
    return &stages[stage_idx].setting;
}

static uint32_t __pipeline_stage_get_shader_file_length(FILE *p_fstream)
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

static uint32_t __pipeline_stage_create_shader_module(pipeline_stage_template_t *p_setting,
                                                        char *filename, VkDevice *p_device)
{
    uint32_t res;
    uint32_t file_len;
    FILE *p_fstream;
    VkShaderModuleCreateInfo create_info;

    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pNext = NULL;
    create_info.flags = 0;

    p_fstream = fopen(filename, "r");
    if (p_fstream == NULL) {
        printf("check filename: %s", filename);
        return FAILURE;
    }

    file_len = __pipeline_stage_get_shader_file_length(p_fstream);
    if (file_len == 0) {
        printf("shader file read failure\n");

        fclose(p_fstream);
        return FAILURE;
    }

    uint8_t buf[file_len];
    if (fread(buf, file_len, 1, p_fstream) == 0) {
        printf("shader file read failure");

        fclose(p_fstream);
        return 0;
    }

    create_info.codeSize = file_len;
    create_info.pCode = (uint32_t *)buf;

    res = vkCreateShaderModule(*p_device, &create_info, NULL, &p_setting->vertex_shader.module);
    if (res != VK_SUCCESS) {
        printf("shader module create failure: %d\n", res);
    }

    fclose(p_fstream);

    return res;
}

uint32_t pipeline_stage_setup_vertex_shader_ctx(char *filename, VkDevice* p_device)
{
    uint32_t res;
    pipeline_stage_template_t *p_setting;

    p_setting = &stages[VULKAN_PIPELINE_STAGE_VERTEX_SHADER].setting;

    if ((filename == NULL) || (p_device == NULL)) {
        return FAILURE;
    }

    if (p_setting->vertex_shader.pName) {
        vkDestroyShaderModule(*p_device, p_setting->vertex_shader.module, NULL);
        p_setting->vertex_shader.pName = NULL;
    }

    res = __pipeline_stage_create_shader_module(p_setting, filename, p_device);
    if (res != VK_SUCCESS) {
        return FAILURE;
    }

    p_setting->vertex_shader.pName = "main";

    stages[VULKAN_PIPELINE_STAGE_VERTEX_SHADER].state = VULKAN_PIPELINE_STAGE_STATE_REGISTERED;

    return SUCCESS;
}

uint32_t pipeline_stage_setup_fragment_shader_ctx(char *filename, VkDevice* p_device)
{
    uint32_t res;
    pipeline_stage_template_t *p_setting;

    p_setting = &stages[VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER].setting;

    if ((filename == NULL) || (p_device == NULL)) {
        return FAILURE;
    }

    if (p_setting->fragment_shader.pName) {
        vkDestroyShaderModule(*p_device, p_setting->fragment_shader.module, NULL);
        p_setting->fragment_shader.pName = NULL;
    }

    res = __pipeline_stage_create_shader_module(p_setting, filename, p_device);
    if (res != VK_SUCCESS) {
        return FAILURE;
    }

    p_setting->fragment_shader.pName = "main";

    stages[VULKAN_PIPELINE_STAGE_FRAGMENT_SHADER].state = VULKAN_PIPELINE_STAGE_STATE_REGISTERED;

    return SUCCESS;
}

//uint32_t pipeline_stage_setup_vertex_input_ctx(void)
//{
//
//}
//
//uint32_t pipeline_stage_setup_input_assembly_ctx(void)
//{
//
//}

uint32_t pipeline_stage_setup_viewport_ctx(VkViewport *p_viewport, VkRect2D *p_scissor)
{
    uint32_t scissor_cnt;
    uint32_t viewport_cnt;
    VkRect2D *p_scissors;
    VkViewport *p_viewports;
    pipeline_stage_template_t *p_setting;

    p_setting = &stages[VULKAN_PIPELINE_STAGE_VIEWPORT].setting;

    scissor_cnt = p_setting->viewport.scissorCount;
    viewport_cnt = p_setting->viewport.viewportCount;

    p_scissors = (VkRect2D *)malloc(sizeof(VkRect2D) * (scissor_cnt + 1));
    p_viewports = (VkViewport *)malloc(sizeof(VkViewport) * (viewport_cnt + 1));

    for (uint32_t idx = 0; idx < viewport_cnt; idx++) {
        p_viewports[idx] = p_setting->viewport.pViewports[idx];
    }
    for (uint32_t idx = 0; idx < scissor_cnt; idx++) {
        p_scissors[idx] = p_setting->viewport.pScissors[idx];
    }

    if (viewport_cnt > 0) {
        free((void *)p_setting->viewport.pViewports);
    }
    if (scissor_cnt > 0) {
        free((void *)p_setting->viewport.pScissors);
    }

    p_viewports[viewport_cnt] = *p_viewport;
    p_scissors[scissor_cnt] = *p_scissor;

    p_setting->viewport.pViewports = p_viewports;
    p_setting->viewport.pScissors = p_scissors;

    p_setting->viewport.scissorCount++;
    p_setting->viewport.viewportCount++;

    stages[VULKAN_PIPELINE_STAGE_VIEWPORT].state = VULKAN_PIPELINE_STAGE_STATE_REGISTERED;

    return SUCCESS;
}


//uint32_t pipeline_stage_setup_dynamic_state_ctx(void)
//{
//    return SUCCESS;
//}
//
//uint32_t pipeline_stage_setup_rasterizer_ctx(void)
//{
//    return SUCCESS;
//}
//
//uint32_t pipeline_stage_setup_multisampling_ctx(void)
//{
//    return SUCCESS;
//}
//
