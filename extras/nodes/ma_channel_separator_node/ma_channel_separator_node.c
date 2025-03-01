#ifndef miniaudio_channel_separator_node_c
#define miniaudio_channel_separator_node_c

#include "ma_channel_separator_node.h"

#include <string.h> /* For memset(). */

MA_API ma_channel_separator_node_config ma_channel_separator_node_config_init(ma_uint32 channels)
{
    ma_channel_separator_node_config config;

    memset(&config, 0, sizeof(config));
    config.nodeConfig = ma_node_config_init();  /* Input and output channels will be set in ma_channel_separator_node_init(). */
    config.channels   = channels;

    return config;
}


static void ma_channel_separator_node_process_pcm_frames(ma_node* pNode, const float** ppFramesIn, ma_uint32* pFrameCountIn, float** ppFramesOut, ma_uint32* pFrameCountOut)
{
    ma_channel_separator_node* pSplitterNode = (ma_channel_separator_node*)pNode;

    (void)pFrameCountIn;
    
    ma_deinterleave_pcm_frames(ma_format_f32, ma_node_get_input_channels(pSplitterNode, 0), *pFrameCountOut, (const void*)ppFramesIn[0], (void**)ppFramesOut);
}

static ma_node_vtable g_ma_channel_separator_node_vtable =
{
    ma_channel_separator_node_process_pcm_frames,
    NULL,
    1,  /* 1 input bus. */
    MA_NODE_BUS_COUNT_UNKNOWN,  /* Output bus count is determined by the channel count and is unknown until the node instance is initialized. */
    0   /* Default flags. */
};

MA_API ma_result ma_channel_separator_node_init(ma_node_graph* pNodeGraph, const ma_channel_separator_node_config* pConfig, const ma_allocation_callbacks* pAllocationCallbacks, ma_channel_separator_node* pSeparatorNode)
{
    ma_result result;
    ma_node_config baseConfig;
    ma_uint32 inputChannels[1];
    ma_uint32 outputChannels[MA_MAX_NODE_BUS_COUNT];
    ma_uint32 iChannel;

    if (pSeparatorNode == NULL) {
        return MA_INVALID_ARGS;
    }

    memset(pSeparatorNode, 0, sizeof(*pSeparatorNode));

    if (pConfig == NULL) {
        return MA_INVALID_ARGS;
    }

    if (pConfig->channels > MA_MAX_NODE_BUS_COUNT) {
        return MA_INVALID_ARGS; /* Channel count cannot exceed the maximum number of buses. */
    }

    inputChannels[0] = pConfig->channels;

    /* All output channels are mono. */
    for (iChannel = 0; iChannel < pConfig->channels; iChannel += 1) {
        outputChannels[iChannel] = 1;
    }

    baseConfig = pConfig->nodeConfig;
    baseConfig.vtable          = &g_ma_channel_separator_node_vtable;
    baseConfig.outputBusCount  = pConfig->channels; /* The vtable has an unknown channel count, so must specify it here. */
    baseConfig.pInputChannels  = inputChannels;
    baseConfig.pOutputChannels = outputChannels;

    result = ma_node_init(pNodeGraph, &baseConfig, pAllocationCallbacks, &pSeparatorNode->baseNode);
    if (result != MA_SUCCESS) {
        return result;
    }

    return MA_SUCCESS;
}

MA_API void ma_channel_separator_node_uninit(ma_channel_separator_node* pSeparatorNode, const ma_allocation_callbacks* pAllocationCallbacks)
{
    /* The base node is always uninitialized first. */
    ma_node_uninit(pSeparatorNode, pAllocationCallbacks);
}

#endif  /* miniaudio_channel_separator_node_c */
