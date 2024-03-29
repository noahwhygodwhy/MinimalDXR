#include "geometryData.h"
ID3D12RootSignature* rootSignature;
ID3D12StateObject* pso;
ID3D12DescriptorHeap* uavHeap;
ID3D12Resource* resources[10];
IDXGIFactory4* factory4;
IDXGIFactory6* factory6;
IDXGIAdapter4* adapter;
IDXGISwapChain1* swapChain;
ID3D12Device10* device;
ID3D12CommandQueue* commandQueue;
ID3D12CommandAllocator* commandAllocator;
ID3D12GraphicsCommandList4* commandList;
ID3D12Fence* fence;
HWND hwnd = nullptr;
void Flush() {
	static UINT64 value = 1;
	commandQueue->Signal(fence, value);
	fence->SetEventOnCompletion(value++, nullptr);}
_Use_decl_annotations_ int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR lpCmdLine, int nCmdShow) {
	hwnd = CreateWindow((LPWSTR) RegisterClassEx(new WNDCLASSEX{ sizeof(WNDCLASSEX), CS_HREDRAW | CS_VREDRAW, [](HWND hWndin, UINT msg, WPARAM wParam, LPARAM lParam) {return DefWindowProc(hWndin, msg, wParam, lParam); }, 0, 0, hInstance, NULL, LoadCursor(NULL, IDC_ARROW), NULL, L"", L"MyWindowClass" }), L"", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 800, 800, NULL, NULL, hInstance, NULL);
	CreateDXGIFactory2(0, IID_PPV_ARGS(&factory4));
	HRESULT hre = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device));
	factory4->QueryInterface(IID_PPV_ARGS(&factory6)); //extra line, but saves more during adapter enumeration
	factory6->EnumAdapterByGpuPreference(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter));
	device->CreateCommandQueue(new D3D12_COMMAND_QUEUE_DESC{D3D12_COMMAND_LIST_TYPE_DIRECT, 0, D3D12_COMMAND_QUEUE_FLAG_NONE, 0 }, IID_PPV_ARGS(&commandQueue));
	HRESULT asd = factory6->CreateSwapChainForHwnd(commandQueue, hwnd, new DXGI_SWAP_CHAIN_DESC1{ 800, 800, DXGI_FORMAT_R16G16B16A16_FLOAT, false, {1, 0}, DXGI_USAGE_RENDER_TARGET_OUTPUT, 2, DXGI_SCALING_STRETCH, DXGI_SWAP_EFFECT_FLIP_DISCARD, DXGI_ALPHA_MODE_IGNORE, 0 }, nullptr, nullptr, &swapChain);
	device->CreateDescriptorHeap(new D3D12_DESCRIPTOR_HEAP_DESC{.Type= D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, .NumDescriptors=1, .Flags= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE }, IID_PPV_ARGS(&uavHeap));
	device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 }, D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_TEXTURE2D, 0, 800, 800, 1, 1, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS }, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resources[8]));
	device->CreateUnorderedAccessView(resources[8], nullptr, new D3D12_UNORDERED_ACCESS_VIEW_DESC{.Format = DXGI_FORMAT_R16G16B16A16_FLOAT, .ViewDimension = D3D12_UAV_DIMENSION_TEXTURE2D}, uavHeap->GetCPUDescriptorHandleForHeapStart());
	void* uploadPointers[5];
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_CUSTOM, D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE, D3D12_MEMORY_POOL_L0, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof(monkeyVerts), 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE}, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resources[0]));
	resources[0]->Map(0, nullptr, (void**)&uploadPointers[0]);
	memcpy(uploadPointers[0], monkeyVerts, sizeof(monkeyVerts));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_CUSTOM, D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE, D3D12_MEMORY_POOL_L0, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof(monkeyIndices), 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE }, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resources[1]));
	resources[1]->Map(0, nullptr, (void**)&uploadPointers[1]);
	memcpy(uploadPointers[1], monkeyIndices, sizeof(monkeyIndices));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_CUSTOM, D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE, D3D12_MEMORY_POOL_L0, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, sizeof(monkeyInstances), 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE }, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resources[6]));	D3D12_RAYTRACING_GEOMETRY_DESC geomDesc2 = D3D12_RAYTRACING_GEOMETRY_DESC{.Type = D3D12_RAYTRACING_GEOMETRY_TYPE_TRIANGLES,.Flags = D3D12_RAYTRACING_GEOMETRY_FLAG_NONE,.Triangles = D3D12_RAYTRACING_GEOMETRY_TRIANGLES_DESC{.Transform3x4 = 0,.IndexFormat = DXGI_FORMAT_R32_UINT,.VertexFormat = DXGI_FORMAT_R32G32B32_FLOAT,.IndexCount = _countof(monkeyIndices),.VertexCount = _countof(monkeyVerts)/ 3,.IndexBuffer = resources[1]->GetGPUVirtualAddress(),.VertexBuffer = D3D12_GPU_VIRTUAL_ADDRESS_AND_STRIDE{.StartAddress = resources[0]->GetGPUVirtualAddress(), .StrideInBytes = 12}} };
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_INPUTS asInputs[] = {{.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL, .Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE, .NumDescs = 1, .DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY, .pGeometryDescs = &geomDesc2 },{.Type = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL, .Flags = D3D12_RAYTRACING_ACCELERATION_STRUCTURE_BUILD_FLAG_NONE, .NumDescs = _countof(monkeyInstances), .DescsLayout = D3D12_ELEMENTS_LAYOUT_ARRAY, .InstanceDescs = resources[6]->GetGPUVirtualAddress()}};
	D3D12_RAYTRACING_ACCELERATION_STRUCTURE_PREBUILD_INFO prebuildInfos[2];
	device->GetRaytracingAccelerationStructurePrebuildInfo(&asInputs[0], &prebuildInfos[0]);
	device->GetRaytracingAccelerationStructurePrebuildInfo(&asInputs[1], &prebuildInfos[1]);
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, prebuildInfos[0].ResultDataMaxSizeInBytes, 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS }, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, nullptr, IID_PPV_ARGS(&resources[3]));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, prebuildInfos[1].ResultDataMaxSizeInBytes, 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS }, D3D12_RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE, nullptr, IID_PPV_ARGS(&resources[2]));
	for (int i = 0; i < _countof(monkeyInstances); i++)	{ monkeyInstances[i].AccelerationStructure = resources[3]->GetGPUVirtualAddress(); } //is this cheating
	resources[6]->Map(0, nullptr, (void**)&uploadPointers[2]);
	memcpy(uploadPointers[2], monkeyInstances, sizeof(monkeyInstances));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, prebuildInfos[0].ScratchDataSizeInBytes, 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS }, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resources[5]));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_DEFAULT, D3D12_CPU_PAGE_PROPERTY_UNKNOWN, D3D12_MEMORY_POOL_UNKNOWN, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, prebuildInfos[1].ScratchDataSizeInBytes, 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS }, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(&resources[4]));
	D3D12_BUILD_RAYTRACING_ACCELERATION_STRUCTURE_DESC buildDescs[2] = {{resources[3]->GetGPUVirtualAddress(), asInputs[0], 0, resources[5]->GetGPUVirtualAddress()},{resources[2]->GetGPUVirtualAddress(), asInputs[1], 0, resources[4]->GetGPUVirtualAddress()} };
	D3D12_ROOT_PARAMETER params[] = {{.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE, .DescriptorTable = {.NumDescriptorRanges = 1,.pDescriptorRanges = new D3D12_DESCRIPTOR_RANGE{.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_UAV,.NumDescriptors = 1,}}},{.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV,.Descriptor = {.ShaderRegister = 0, .RegisterSpace = 0}},{.ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV,.Descriptor = {.ShaderRegister = 1, .RegisterSpace = 0}}};
	D3D12SerializeRootSignature(new D3D12_ROOT_SIGNATURE_DESC{ .NumParameters = std::size(params), .pParameters = params }, D3D_ROOT_SIGNATURE_VERSION_1_0, (ID3DBlob**)&uploadPointers[4], nullptr);
	device->CreateRootSignature(0, ((ID3DBlob*)uploadPointers[4])->GetBufferPointer(), ((ID3DBlob*)uploadPointers[4])->GetBufferSize(), IID_PPV_ARGS(&rootSignature));
	D3D12_STATE_SUBOBJECT subobjects[] = {{.Type = D3D12_STATE_SUBOBJECT_TYPE_DXIL_LIBRARY, .pDesc = new D3D12_DXIL_LIBRARY_DESC{.DXILLibrary = {.pShaderBytecode = compiledShader,.BytecodeLength = std::size(compiledShader)} }},{.Type = D3D12_STATE_SUBOBJECT_TYPE_HIT_GROUP, .pDesc = new D3D12_HIT_GROUP_DESC{.HitGroupExport = L"HitGroup", .Type = D3D12_HIT_GROUP_TYPE_TRIANGLES, .ClosestHitShaderImport = L"ClosestHit" }},{.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_SHADER_CONFIG, .pDesc = new D3D12_RAYTRACING_SHADER_CONFIG{.MaxPayloadSizeInBytes = 12, .MaxAttributeSizeInBytes = 8}},{.Type = D3D12_STATE_SUBOBJECT_TYPE_GLOBAL_ROOT_SIGNATURE, .pDesc = new D3D12_GLOBAL_ROOT_SIGNATURE{ rootSignature }},{.Type = D3D12_STATE_SUBOBJECT_TYPE_RAYTRACING_PIPELINE_CONFIG, .pDesc = new D3D12_RAYTRACING_PIPELINE_CONFIG{.MaxTraceRecursionDepth = 30 }} };
	device->CreateStateObject(new D3D12_STATE_OBJECT_DESC{ .Type = D3D12_STATE_OBJECT_TYPE_RAYTRACING_PIPELINE,.NumSubobjects = 5, .pSubobjects = subobjects }, IID_PPV_ARGS(&pso));
	device->CreateCommittedResource(new D3D12_HEAP_PROPERTIES{ D3D12_HEAP_TYPE_CUSTOM, D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE, D3D12_MEMORY_POOL_L0, 0, 0 }, D3D12_HEAP_FLAG_NONE, new D3D12_RESOURCE_DESC{ D3D12_RESOURCE_DIMENSION_BUFFER, 0, 3 * D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT, 1, 1, 1, DXGI_FORMAT_UNKNOWN, DXGI_SAMPLE_DESC{1, 0}, D3D12_TEXTURE_LAYOUT_ROW_MAJOR, D3D12_RESOURCE_FLAG_NONE }, D3D12_RESOURCE_STATE_COMMON, nullptr,IID_PPV_ARGS(&resources[7]));
	ID3D12StateObjectProperties* stateprops;
	pso->QueryInterface(&stateprops);
	resources[7]->Map(0, nullptr, &uploadPointers[3]);
	memcpy(static_cast<char*>(uploadPointers[3]) + (D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT * 0), stateprops->GetShaderIdentifier(L"RayGeneration"), D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	memcpy(static_cast<char*>(uploadPointers[3]) + (D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT * 1), stateprops->GetShaderIdentifier(L"Miss"), D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	memcpy(static_cast<char*>(uploadPointers[3]) + (D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT * 2), stateprops->GetShaderIdentifier(L"HitGroup"), D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES);
	commandList->Close();
	for (uint32_t frameIndex = 0; frameIndex < 5; frameIndex = (frameIndex + 1)%2) {
		PeekMessageW(NULL, NULL, 0, 0, PM_REMOVE);
		commandAllocator->Reset();
		commandList->Reset(commandAllocator, nullptr);
		commandList->BuildRaytracingAccelerationStructure(&buildDescs[0], 0, nullptr);
		commandList->ResourceBarrier(1, new D3D12_RESOURCE_BARRIER{ .Type = D3D12_RESOURCE_BARRIER_TYPE_UAV, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .UAV = {.pResource = resources[3]} });
		commandList->BuildRaytracingAccelerationStructure(&buildDescs[1], 0, nullptr);
		commandList->ResourceBarrier(1, new D3D12_RESOURCE_BARRIER{ .Type = D3D12_RESOURCE_BARRIER_TYPE_UAV, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .UAV = {.pResource = resources[2]} });
		commandList->SetPipelineState1(pso);
		commandList->SetComputeRootSignature(rootSignature);
		commandList->SetDescriptorHeaps(1, &uavHeap);
		D3D12_GPU_DESCRIPTOR_HANDLE uavTable = uavHeap->GetGPUDescriptorHandleForHeapStart();
		commandList->SetComputeRootDescriptorTable(0, uavTable); // u0 
		commandList->SetComputeRootShaderResourceView(1, resources[2]->GetGPUVirtualAddress()); // t0
		commandList->SetComputeRootShaderResourceView(2, resources[0]->GetGPUVirtualAddress()); //t1 
		commandList->DispatchRays(new D3D12_DISPATCH_RAYS_DESC{ .RayGenerationShaderRecord = {.StartAddress = resources[7]->GetGPUVirtualAddress(),.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES},.MissShaderTable = {.StartAddress = resources[7]->GetGPUVirtualAddress() + D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT,.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES},.HitGroupTable = {.StartAddress = resources[7]->GetGPUVirtualAddress() + 2 * D3D12_RAYTRACING_SHADER_TABLE_BYTE_ALIGNMENT,.SizeInBytes = D3D12_SHADER_IDENTIFIER_SIZE_IN_BYTES},.Width = 800,.Height = 800,.Depth = 1 });
		swapChain->GetBuffer(frameIndex, IID_PPV_ARGS(&resources[9]));
		D3D12_RESOURCE_BARRIER barriers[] = { D3D12_RESOURCE_BARRIER{.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = resources[8], .StateBefore = D3D12_RESOURCE_STATE_COMMON, .StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE} }, D3D12_RESOURCE_BARRIER{.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = resources[9], .StateBefore = D3D12_RESOURCE_STATE_PRESENT, .StateAfter = D3D12_RESOURCE_STATE_COPY_DEST} }, D3D12_RESOURCE_BARRIER{.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = resources[8], .StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE, .StateAfter = D3D12_RESOURCE_STATE_COMMON} }, D3D12_RESOURCE_BARRIER{.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION, .Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE, .Transition = {.pResource = resources[9], .StateBefore = D3D12_RESOURCE_STATE_COPY_DEST, .StateAfter = D3D12_RESOURCE_STATE_PRESENT} }};
		commandList->ResourceBarrier(2, &barriers[0]);
		commandList->CopyResource(resources[9], resources[8]);
		commandList->ResourceBarrier(2, &barriers[2]);
		HRESULT a = commandList->Close();
		commandQueue->ExecuteCommandLists(1, CommandListCast(&commandList));
		Flush();
		swapChain->Present(1, 0);
	}
}