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
#include <fstream>
#include <vector>
#include <glm\glm.hpp>
#include <assimp\Importer.hpp>
#include <assimp\scene.h>
#include <assimp\postprocess.h>
#include <glfw\glfw3.h>
#include <glfw\glfw3native.h>
#include <Graphics\GraphicsEngineVulkan\interface\EngineFactoryVk.h>
#include <Graphics\GraphicsEngine\interface\RenderDevice.h>
#include <Graphics\GraphicsEngine\interface\DeviceContext.h>
#include <Graphics\GraphicsEngine\interface\SwapChain.h>
using namespace Diligent;
using namespace Assimp;
using namespace glm;
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
    /***ASSIMP MODEL LOADING***/
    vector<vec3> vertices;
    vector<vec3> indices;
    const string objFilePath = "C:\\code\\c++\\game-engine\\GameEngine\\x64\\Debug\\monkey.obj";
    ifstream f(objFilePath);
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(objFilePath, aiProcessPreset_TargetRealtime_Quality);
    for (int currentMesh = 0; currentMesh < scene->mNumMeshes; currentMesh++)
    {
        for (int currentVertex = 0; currentVertex < scene->mMeshes[currentMesh]->mNumVertices; currentVertex++)
        {
            vec3 vertex;
            vertex.x = scene->mMeshes[currentMesh]->mVertices[currentVertex].x;
            vertex.y = scene->mMeshes[currentMesh]->mVertices[currentVertex].y;
            vertex.z = scene->mMeshes[currentMesh]->mVertices[currentVertex].z;
            vertices.push_back(vertex);
        }
        
        for (int currentFace = 0; currentFace < scene->mMeshes[currentMesh]->mNumFaces; currentFace++)
        {
            vec3 index;
            index.x = scene->mMeshes[currentMesh]->mVertices[currentFace].x;
            index.y = scene->mMeshes[currentMesh]->mVertices[currentFace].y;
            index.z = scene->mMeshes[currentMesh]->mVertices[currentFace].z;
            indices.push_back(index);
        }
    }

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
        cbuffer Constants
        {
            float4x4 g_WorldViewProj;
        };

        struct VSInput
        {
            float3 Pos : ATTRIB0;
            float4 Color : ATTRIB1;
        };

        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float3 Color : COLOR; 
        };

        void main(in VSInput VSIn,
                  out PSInput PSIn) 
        {
            PSIn.Pos   = mul( float4(VSIn.Pos,1.0), g_WorldViewProj);
            PSIn.Color = VSIn.Color;
        }
    )";
    (*renderDevice)->CreateShader(shaderCreateInfo, vertexShader);

    /***CREATE UNIFORM BUFFER THAT DESCRIBES THE LAYOUT OF VERTEX DATA***/
    BufferDesc uniformBufferDesc;
    uniformBufferDesc.Name = "transformation matrix buffer";
    uniformBufferDesc.Usage = USAGE_DYNAMIC;
    uniformBufferDesc.BindFlags = BIND_UNIFORM_BUFFER;
    uniformBufferDesc.CPUAccessFlags = CPU_ACCESS_WRITE;
    uniformBufferDesc.uiSizeInBytes = sizeof(mat4x4);
    IBuffer** vertexShaderConstants = new IBuffer * ();
    (*renderDevice)->CreateBuffer(uniformBufferDesc, nullptr, vertexShaderConstants);

    IShader** pixelShader = new IShader*();
    shaderCreateInfo.Desc.ShaderType = SHADER_TYPE_PIXEL;
    shaderCreateInfo.EntryPoint = "main";
    shaderCreateInfo.Desc.Name = "Sekhmet Pixel Shader";
    shaderCreateInfo.Source = R"(
        struct PSInput 
        { 
            float4 Pos   : SV_POSITION; 
            float4 Color : COLOR; 
        };

        struct PSOutput
        { 
            float4 Color : SV_TARGET; 
        };

        void main(in  PSInput  PSIn,
                  out PSOutput PSOut)
        {
            PSOut.Color = PSIn.Color;
        }
    )";
    (*renderDevice)->CreateShader(shaderCreateInfo, pixelShader);

    //Define the vertex shader input layout
    LayoutElement layoutElements[] =
    {
        LayoutElement{0,0,3,VT_FLOAT32, False},
        LayoutElement{1,0,4,VT_FLOAT32, False}
    };
    graphicsPipelineCreateInfo.GraphicsPipeline.InputLayout.LayoutElements = layoutElements;
    graphicsPipelineCreateInfo.GraphicsPipeline.InputLayout.NumElements = _countof(layoutElements);

    graphicsPipelineCreateInfo.pVS = *vertexShader;
    graphicsPipelineCreateInfo.pPS = *pixelShader;

    // Define variable type that will be used by default
    graphicsPipelineCreateInfo.PSODesc.ResourceLayout.DefaultVariableType = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    IPipelineState** pipelineState = new IPipelineState*();
    (*renderDevice)->CreateGraphicsPipelineState(graphicsPipelineCreateInfo, pipelineState);

    //initialize the constants section of the vertex shader
    (*pipelineState)->GetStaticVariableByName(SHADER_TYPE_VERTEX, "Constants")->Set(*vertexShaderConstants);

    //Bind static resources
    IShaderResourceBinding** shaderResourceBinding = new IShaderResourceBinding * ();
    (*pipelineState)->CreateShaderResourceBinding(shaderResourceBinding, true);

    //Create the Vertex Buffer
    BufferDesc vertexBufferDesc;
    vertexBufferDesc.Name = "Monkey Vertex Buffer";
    vertexBufferDesc.Usage = USAGE_IMMUTABLE;
    vertexBufferDesc.BindFlags = BIND_VERTEX_BUFFER;
    vertexBufferDesc.uiSizeInBytes = sizeof(vertices);
    BufferData vertexBufferData;
    vertexBufferData.pData = vertices.data();
    vertexBufferData.DataSize = sizeof(vertices.data());
    IBuffer** vertexBuffer = new IBuffer * ();
    (*renderDevice)->CreateBuffer(vertexBufferDesc, &vertexBufferData, vertexBuffer);

    //Create the Index Buffer to Avoid Repeating Vertex Data
    BufferDesc indexBufferDesc;
    indexBufferDesc.Name = "Monkey Index Buffer";
    indexBufferDesc.Usage = USAGE_IMMUTABLE;
    indexBufferDesc.BindFlags = BIND_INDEX_BUFFER;
    indexBufferDesc.uiSizeInBytes = sizeof(indices);
    BufferData indexBufferData;
    indexBufferData.pData = indices.data();
    indexBufferData.DataSize = sizeof(indices.data());
    IBuffer** indexBuffer = new IBuffer * ();
    (*renderDevice)->CreateBuffer(indexBufferDesc, &indexBufferData, indexBuffer);
    
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

        //Map the uniform buffer
        

        //Bind vertex buffer
        Uint32 offset = 0;
        (*deviceContext)->SetVertexBuffers(0, 1, vertexBuffer, &offset, RESOURCE_STATE_TRANSITION_MODE_TRANSITION, SET_VERTEX_BUFFERS_FLAG_RESET);
        (*deviceContext)->SetIndexBuffer(*indexBuffer, 0, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        //set the device context's graphics pipeline
        (*deviceContext)->SetPipelineState(*pipelineState);
        (*deviceContext)->CommitShaderResources(*shaderResourceBinding, RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        DrawIndexedAttribs drawAttrs;
        drawAttrs.IndexType = VT_UINT32;
        drawAttrs.NumIndices = indices.size();
        drawAttrs.Flags = DRAW_FLAG_VERIFY_ALL;
        (*deviceContext)->DrawIndexed(drawAttrs);
    }

    return 0;
};