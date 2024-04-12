#include "translateshaderclass.h"


TranslateShaderClass::TranslateShaderClass()
{
    m_vertexShader = 0;
    m_pixelShader = 0;
    m_layout = 0;
    m_matrixBuffer = 0;
    m_sampleState = 0;
    m_translateBuffer = 0;
}


TranslateShaderClass::TranslateShaderClass(const TranslateShaderClass& other)
{
}


TranslateShaderClass::~TranslateShaderClass()
{
}


bool TranslateShaderClass::Initialize(ID3D11Device* device, HWND hwnd)
{
    Logger::Get().Log("Initilaizing TranslateShaderClass", __FILE__, __LINE__);

    bool result;
    wchar_t vsFilename[128];
    wchar_t psFilename[128];
    int error;

    // Set the filename of the vertex shader.
    error = wcscpy_s(vsFilename, 128, L"translate.vs");
    if (error != 0)
    {
        Logger::Get().Log("Failed to copy vsFilename", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Set the filename of the pixel shader.
    error = wcscpy_s(psFilename, 128, L"translate.ps");
    if (error != 0)
    {
        Logger::Get().Log("Failed to copy psFilename", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Initialize the vertex and pixel shaders.
    result = InitializeShader(device, hwnd, vsFilename, psFilename);
    if (!result)
    {
        Logger::Get().Log("Failed to initialize shader", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    Logger::Get().Log("TranslateShaderClass initialized", __FILE__, __LINE__);

    return true;
}


void TranslateShaderClass::Shutdown()
{
    // Shutdown the vertex and pixel shaders as well as the related objects.
    ShutdownShader();

    return;
}

bool TranslateShaderClass::Render(ID3D11DeviceContext * deviceContext, int indexCount, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, float translation)
{
    bool result;


    // Set the shader parameters that it will use for rendering.
    result = SetShaderParameters(deviceContext, worldMatrix, viewMatrix, projectionMatrix, texture, translation);
    if (!result)
    {
        Logger::Get().Log("Failed to set shader parameters", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Now render the prepared buffers with the shader.
    RenderShader(deviceContext, indexCount);

    return true;
}


bool TranslateShaderClass::InitializeShader(ID3D11Device * device, HWND hwnd, WCHAR * vsFilename, WCHAR * psFilename)
{
    Logger::Get().Log("Initializing translate shader", __FILE__, __LINE__);

    HRESULT result;
    ID3D10Blob* errorMessage;
    ID3D10Blob* vertexShaderBuffer;
    ID3D10Blob* pixelShaderBuffer;
    D3D11_INPUT_ELEMENT_DESC polygonLayout[2];
    unsigned int numElements;
    D3D11_BUFFER_DESC matrixBufferDesc;
    D3D11_SAMPLER_DESC samplerDesc;
    D3D11_BUFFER_DESC translateBufferDesc;


    // Initialize the pointers this function will use to null.
    errorMessage = 0;
    vertexShaderBuffer = 0;
    pixelShaderBuffer = 0;

    // Compile the vertex shader code.
    result = D3DCompileFromFile(vsFilename, NULL, NULL, "TranslateVertexShader", "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
    &vertexShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
        }
        // If there was nothing in the error message then it simply could not find the shader file itself.
        else
        {
            Logger::Get().Log("Failed to compile shader", __FILE__, __LINE__, Logger::LogLevel::Error);
        }

        return false;
    }

     // Compile the pixel shader code.
    result = D3DCompileFromFile(psFilename, NULL, NULL, "TranslatePixelShader", "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0,
    &pixelShaderBuffer, &errorMessage);
    if (FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if (errorMessage)
        {
            OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // If there was nothing in the error message then it simply could not find the file itself.
        else
        {
            Logger::Get().Log("Failed to compile shader", __FILE__, __LINE__, Logger::LogLevel::Error);
        }

        return false;
    }

    // Create the vertex shader from the buffer.
    result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to create vertex shader", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Create the pixel shader from the buffer.
    result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to create pixel shader", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Create the vertex input layout description.
    polygonLayout[0].SemanticName = "POSITION";
    polygonLayout[0].SemanticIndex = 0;
    polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    polygonLayout[0].InputSlot = 0;
    polygonLayout[0].AlignedByteOffset = 0;
    polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[0].InstanceDataStepRate = 0;

    polygonLayout[1].SemanticName = "TEXCOORD";
    polygonLayout[1].SemanticIndex = 0;
    polygonLayout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
    polygonLayout[1].InputSlot = 0;
    polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    polygonLayout[1].InstanceDataStepRate = 0;

    // Get a count of the elements in the layout.
    numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

    // Create the vertex input layout.
    result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(),
        vertexShaderBuffer->GetBufferSize(), &m_layout);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to create input layout", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
    vertexShaderBuffer->Release();
    vertexShaderBuffer = 0;

    pixelShaderBuffer->Release();
    pixelShaderBuffer = 0;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to create matrix buffer", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Create a texture sampler state description.
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MipLODBias = 0.0f;
    samplerDesc.MaxAnisotropy = 1;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    samplerDesc.BorderColor[0] = 0;
    samplerDesc.BorderColor[1] = 0;
    samplerDesc.BorderColor[2] = 0;
    samplerDesc.BorderColor[3] = 0;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    // Create the texture sampler state.
    result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to create sampler state", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Setup the description of the texture translation dynamic constant buffer that is in the pixel shader.
    translateBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    translateBufferDesc.ByteWidth = sizeof(TranslateBufferType);
    translateBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    translateBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    translateBufferDesc.MiscFlags = 0;
    translateBufferDesc.StructureByteStride = 0;

    // Create the constant buffer pointer so we can access the pixel shader constant buffer from within this class.
    result = device->CreateBuffer(&translateBufferDesc, NULL, &m_translateBuffer);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to create translate buffer", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    Logger::Get().Log("Translate shader initialized", __FILE__, __LINE__);

    return true;
}


void TranslateShaderClass::ShutdownShader()
{
    Logger::Get().Log("Shutting down translate shader", __FILE__, __LINE__);

    // Release the texture translation constant buffer.
    if (m_translateBuffer)
    {
            m_translateBuffer->Release();
            m_translateBuffer = 0;
    }

    // Release the sampler state.
    if (m_sampleState)
    {
        m_sampleState->Release();
        m_sampleState = 0;
    }

    // Release the matrix constant buffer.
    if (m_matrixBuffer)
    {
        m_matrixBuffer->Release();
        m_matrixBuffer = 0;
    }

    // Release the layout.
    if (m_layout)
    {
        m_layout->Release();
        m_layout = 0;
    }

    // Release the pixel shader.
    if (m_pixelShader)
    {
        m_pixelShader->Release();
        m_pixelShader = 0;
    }

    // Release the vertex shader.
    if (m_vertexShader)
    {
        m_vertexShader->Release();
        m_vertexShader = 0;
    }

    Logger::Get().Log("Translate shader shut down", __FILE__, __LINE__);  

    return;
}


void TranslateShaderClass::OutputShaderErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFilename)
{
    char* compileErrors;
    unsigned long long bufferSize, i;
    ofstream fout;


    // Get a pointer to the error message text buffer.
    compileErrors = (char*)(errorMessage->GetBufferPointer());

    // Get the length of the message.
    bufferSize = errorMessage->GetBufferSize();

    // Open a file to write the error message to.
    fout.open("shader-error.txt");

    // Write out the error message.
    for (i = 0; i < bufferSize; i++)
    {
        fout << compileErrors[i];
    }

    // Close the file.
    fout.close();

    // Release the error message.
    errorMessage->Release();
    errorMessage = 0;

    // Pop a message up on the screen to notify the user to check the text file for compile errors.
    MessageBox(hwnd, L"Error compiling shader.  Check shader-error.txt for message.", shaderFilename, MB_OK);

    return;
}


bool TranslateShaderClass::SetShaderParameters(ID3D11DeviceContext * deviceContext, XMMATRIX worldMatrix, XMMATRIX viewMatrix,
    XMMATRIX projectionMatrix, ID3D11ShaderResourceView * texture, float translation)
{
    HRESULT result;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    unsigned int bufferNumber;
    TranslateBufferType* dataPtr2;


    // Transpose the matrices to prepare them for the shader.
    worldMatrix = XMMatrixTranspose(worldMatrix);
    viewMatrix = XMMatrixTranspose(viewMatrix);
    projectionMatrix = XMMatrixTranspose(projectionMatrix);

    // Lock the constant buffer so it can be written to.
    result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to map matrix buffer", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Get a pointer to the data in the constant buffer.
    dataPtr = (MatrixBufferType*)mappedResource.pData;

    // Copy the matrices into the constant buffer.
    dataPtr->world = worldMatrix;
    dataPtr->view = viewMatrix;
    dataPtr->projection = projectionMatrix;

    // Unlock the constant buffer.
    deviceContext->Unmap(m_matrixBuffer, 0);

    // Set the position of the constant buffer in the vertex shader.
    bufferNumber = 0;

    // Finally set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

    // Set shader texture resource in the pixel shader.
    deviceContext->PSSetShaderResources(0, 1, &texture);

    // Lock the texture translation constant buffer so it can be written to.
    result = deviceContext->Map(m_translateBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
    {
        Logger::Get().Log("Failed to map translate buffer", __FILE__, __LINE__, Logger::LogLevel::Error);
        return false;
    }

    // Get a pointer to the data in the texture translation constant buffer.
    dataPtr2 = (TranslateBufferType*)mappedResource.pData;

    // Copy the translation value into the texture translation constant buffer.
    dataPtr2->translation = translation;

    // Unlock the buffer.
    deviceContext->Unmap(m_translateBuffer, 0);

    // Set the position of the texture translation constant buffer in the pixel shader.
    bufferNumber = 0;

    // Now set the texture translation constant buffer in the pixel shader with the updated values.
    deviceContext->PSSetConstantBuffers(bufferNumber, 1, &m_translateBuffer);

    return true;
}


void TranslateShaderClass::RenderShader(ID3D11DeviceContext * deviceContext, int indexCount)
{
    // Set the vertex input layout.
    deviceContext->IASetInputLayout(m_layout);

    // Set the vertex and pixel shaders that will be used to render this triangle.
    deviceContext->VSSetShader(m_vertexShader, NULL, 0);
    deviceContext->PSSetShader(m_pixelShader, NULL, 0);

    // Set the sampler state in the pixel shader.
    deviceContext->PSSetSamplers(0, 1, &m_sampleState);

    // Render the geometry.
    deviceContext->DrawIndexed(indexCount, 0, 0);

    return;
}