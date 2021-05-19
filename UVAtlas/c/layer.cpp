#include "pch.h"
#include "DirectXMesh.h"
#include "UVAtlas.h"

#if defined(_MSC_VER)
#   define EXPORT_C_API __declspec(dllexport)
#else
#	define EXPORT_C_API __attribute__((visibility("default")))
#endif

extern "C" {

    EXPORT_C_API HRESULT __cdecl GenerateAdjacencyAndPointReps(
        _In_reads_(nFaces * 3) const uint32_t* indices, _In_ size_t nFaces,
        _In_reads_(nVerts) const DirectX::XMFLOAT3* positions, _In_ size_t nVerts,
        _In_ float epsilon,
        _Out_writes_opt_(nVerts) uint32_t* pointRep,
        _Out_writes_opt_(nFaces * 3) uint32_t* adjacency)
    {
        return DirectX::GenerateAdjacencyAndPointReps(indices, nFaces, positions, nVerts, epsilon, pointRep, adjacency);
    }

    typedef struct uvatlas_result {
        HRESULT                 code;
        uint32_t                verticesCount;
        uint32_t                indicesCount;
        DirectX::UVAtlasVertex* vertices;
        uint32_t* indices;
        uint32_t* facePartitioning;
        uint32_t* vertexRemapArray;
        float                   stretch;
        uint32_t                charts;
    }uvatlas_result;

    EXPORT_C_API uvatlas_result* __cdecl ComputeUV(
        const DirectX::XMFLOAT3* positions, size_t nVerts,
        const void* indices, size_t nFaces,
        size_t maxChartNumber, float maxStretch,
        size_t width, size_t height, float gutter,
        const uint32_t* adjacency,
        float callbackFrequency,
        DirectX::UVATLAS options)
    {
        uvatlas_result* atlas_result = new uvatlas_result();
        std::vector<DirectX::UVAtlasVertex> vertexBuffer;
        std::vector<uint8_t> indexBuffer;
        std::vector<uint32_t> facePartitioning;
        std::vector<uint32_t> vertexRemapArray;
        size_t charts;

        atlas_result->code = DirectX::UVAtlasCreate(positions, nVerts,
            indices, DXGI_FORMAT_R32_UINT, nFaces,
            maxChartNumber, maxStretch,
            width, height,
            gutter,
            adjacency, nullptr,
            nullptr,
            nullptr,
            callbackFrequency,
            (DirectX::UVATLAS)options,
            vertexBuffer,
            indexBuffer,
            &facePartitioning,
            &vertexRemapArray,
            &atlas_result->stretch,
            &charts);

        if (FAILED(atlas_result->code))
            return atlas_result;

        auto newIB = reinterpret_cast<const uint32_t*>(&indexBuffer.front());

        atlas_result->verticesCount = static_cast<uint32_t>(vertexBuffer.size());
        atlas_result->indicesCount = static_cast<uint32_t>(nFaces * 3);
        atlas_result->vertices = new DirectX::UVAtlasVertex[atlas_result->verticesCount];
        atlas_result->indices = new uint32_t[atlas_result->indicesCount];
        atlas_result->facePartitioning = new uint32_t[atlas_result->verticesCount];
        atlas_result->vertexRemapArray = new uint32_t[atlas_result->verticesCount];
        atlas_result->charts = static_cast<uint32_t>(charts);

        std::copy(vertexBuffer.begin(), vertexBuffer.end(), atlas_result->vertices);
        memcpy(atlas_result->indices, newIB, atlas_result->indicesCount * sizeof(uint32_t));
        std::copy(facePartitioning.begin(), facePartitioning.end(), atlas_result->facePartitioning);
        std::copy(vertexRemapArray.begin(), vertexRemapArray.end(), atlas_result->vertexRemapArray);
        return atlas_result;
    }

    EXPORT_C_API void uvatlas_delete(uvatlas_result* result)
    {
        delete[] result->vertices;
        delete[] result->indices;
        delete[] result->facePartitioning;
        delete[] result->vertexRemapArray;
        delete result;
        result = nullptr;
    }
}
