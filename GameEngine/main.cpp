/*
* Copyright (c) 2020 Robert Reyes
* License file: https://github.com/TyrannusX/SekhmetEngine/blob/main/LICENSE
*/

/***DILIGENT MACRO DEFINITIONS***/
#ifndef PLATFORM_WIN32
#    define PLATFORM_WIN32 1
#endif

#ifndef ENGINE_DLL
#    define ENGINE_DLL 1
#endif

#ifndef D3D11_SUPPORTED
#    define D3D11_SUPPORTED 1
#endif

#ifndef D3D12_SUPPORTED
#    define D3D12_SUPPORTED 1
#endif

#ifndef GL_SUPPORTED
#    define GL_SUPPORTED 1
#endif

#ifndef VULKAN_SUPPORTED
#    define VULKAN_SUPPORTED 1
#endif

/***GLFW MACRO DEFINITIONS***/
#define GLFW_EXPOSE_NATIVE_WIN32
#define GLFW_INCLUDE_VULKAN

#include <iostream>
#include <glfw\glfw3.h>
#include <glfw\glfw3native.h>
#include <Graphics\GraphicsEngineVulkan\interface\EngineFactoryVk.h>
#include <Graphics\GraphicsEngine\interface\RenderDevice.h>
#include <Graphics\GraphicsEngine\interface\DeviceContext.h>
#include <Graphics\GraphicsEngine\interface\SwapChain.h>
using namespace Diligent;
using namespace std;

struct DestroyglfwWin
{
    void operator()(GLFWwindow* ptr)
    {
        glfwDestroyWindow(ptr);
    }
};

int main(int argc, char** argv)
{
    /***GLFW SETUP***/
    if (!glfwInit())
    {
        cerr << "Failed to initialize GLFW!" << endl;
        return -1;
    }
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Gaming Time", NULL, NULL);
    glfwMakeContextCurrent(window);

    /***DILIGENT RENDERER SETUP***/

    //define swap chain, device, and context
    SwapChainDesc  swapChainDesc;
    IRenderDevice** renderDevice = new IRenderDevice*();
    IDeviceContext** deviceContext = new IDeviceContext*();
    ISwapChain** swapChain = new ISwapChain*();

    GetEngineFactoryVkType getEngineFactoryVk = LoadGraphicsEngineVk();
    EngineVkCreateInfo engineCreateInfo;
    engineCreateInfo.EnableValidation = true;
    IEngineFactoryVk* engineFactoryVk = getEngineFactoryVk();
    engineFactoryVk->CreateDeviceAndContextsVk(engineCreateInfo, renderDevice, deviceContext);
    Win32NativeWindow windowToRenderTo{ glfwGetWin32Window(window) };
    engineFactoryVk->CreateSwapChainVk(*renderDevice, *deviceContext, swapChainDesc, windowToRenderTo, swapChain);

    /***DILIGENT GRAPHICS PIPELINE CONFIGURATION***/
    GraphicsPipelineStateCreateInfo graphicsPipelineCreateInfo;
    graphicsPipelineCreateInfo.PSODesc.Name = "Sekhmet Pipeline State Object";
    graphicsPipelineCreateInfo.PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    graphicsPipelineCreateInfo.GraphicsPipeline.NumRenderTargets = 1;
    graphicsPipelineCreateInfo.GraphicsPipeline.RTVFormats[0] = (*swapChain)->GetDesc().ColorBufferFormat;
    graphicsPipelineCreateInfo.GraphicsPipeline.DSVFormat = (*swapChain)->GetDesc().DepthBufferFormat;
    graphicsPipelineCreateInfo.GraphicsPipeline.PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    graphicsPipelineCreateInfo.GraphicsPipeline.RasterizerDesc.CullMode = CULL_MODE_NONE;
    graphicsPipelineCreateInfo.GraphicsPipeline.DepthStencilDesc.DepthEnable = False;

    ShaderCreateInfo shaderCreateInfo;
    shaderCreateInfo.SourceLanguage = SHADER_SOURCE_LANGUAGE_HLSL;
    shaderCreateInfo.UseCombinedTextureSamplers = true;
    IShader** vertexShader = new IShader*();
    shaderCreateInfo.Desc.ShaderType = SHADER_TYPE_VERTEX;
    shaderCreateInfo.EntryPoint = "main";
    shaderCreateInfo.Desc.Name = "Sekhmet Vertex Shader";
    shaderCreateInfo.Source = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        void main(in  uint    VertId : SV_VertexID,
                  out PSInput PSIn) 
        {
            float4 Pos[3];
            Pos[0] = float4(-0.5, -0.5, 0.0, 1.0);
            Pos[1] = float4( 0.0, +0.5, 0.0, 1.0);
            Pos[2] = float4(+0.5, -0.5, 0.0, 1.0);

            float3 Col[3];
            Col[0] = float3(1.0, 0.0, 0.0); // red
            Col[1] = float3(0.0, 1.0, 0.0); // green
            Col[2] = float3(0.0, 0.0, 1.0); // blue

            PSIn.Pos   = Pos[VertId];
            PSIn.Color = Col[VertId];
        }
    )";
    (*renderDevice)->CreateShader(shaderCreateInfo, vertexShader);

    IShader** pixelShader = new IShader*();
    shaderCreateInfo.Desc.ShaderType = SHADER_TYPE_PIXEL;
    shaderCreateInfo.EntryPoint = "main";
    shaderCreateInfo.Desc.Name = "Sekhmet Pixel Shader";
    shaderCreateInfo.Source = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        struct PSOutput
        { 
            float4 Color : SV_TARGET; 
        };

        void main(in  PSInput  PSIn,
                  out PSOutput PSOut)
        {
            PSOut.Color = float4(PSIn.Color.rgb, 1.0);
        }
    )";
    (*renderDevice)->CreateShader(shaderCreateInfo, pixelShader);

    graphicsPipelineCreateInfo.pVS = *vertexShader;
    graphicsPipelineCreateInfo.pPS = *pixelShader;

    IPipelineState** pipelineState = new IPipelineState*();
    (*renderDevice)->CreateGraphicsPipelineState(graphicsPipelineCreateInfo, pipelineState);

    /***THE MAIN LOOP***/
    while (!glfwWindowShouldClose(window))
    {
        //poll events (user input, etc)
        glfwPollEvents();

        //set render context
        ITextureView** renderTargetTextureView = new ITextureView*((*swapChain)->GetCurrentBackBufferRTV());
        ITextureView* depthTextureView = (*swapChain)->GetDepthBufferDSV();
        (*deviceContext)->SetRenderTargets(1, renderTargetTextureView, depthTextureView, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //clear back buffer
        const float ClearColor[] = { 0.350f, 0.350f, 0.350f, 1.0f };
        (*deviceContext)->ClearRenderTarget(*renderTargetTextureView, ClearColor, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        (*deviceContext)->ClearDepthStencil(depthTextureView, CLEAR_DEPTH_FLAG, 1.0f, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //set the device context's graphics pipeline
        (*deviceContext)->SetPipelineState(*pipelineState);

        DrawAttribs drawAttributes;
        drawAttributes.NumVertices = 3;
        (*deviceContext)->Draw(drawAttributes);
        (*swapChain)->Present();
    }

    return 0;
}