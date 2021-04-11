#include "pch.h"
#include "UVAtlas.h"

#if defined(_MSC_VER)
#   define EXPORT_C_API __declspec(dllexport)
#else
#	define EXPORT_C_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

    typedef enum uvatlas_options {
        UVATLAS_DEFAULT = 0x00,
        UVATLAS_GEODESIC_FAST = 0x01,
        UVATLAS_GEODESIC_QUALITY = 0x02,
        UVATLAS_LIMIT_MERGE_STRETCH = 0x04,
        UVATLAS_LIMIT_FACE_STRETCH = 0x08,
        _UVATLAS_FORCE_U32 = 0x7FFFFFFF
    } uvatlas_options;

    typedef struct uvatlas_vertex
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT2 uv;
    } uvatlas_vertex;

    typedef struct uvatlas_result {
        uint32_t            verticesCount;
        uvatlas_vertex*     vertices;
        uint32_t            indicesCount;
        uint8_t*            indices;
        float               stretch;
        size_t              charts;
    } uvatlas_result;

    EXPORT_C_API uvatlas_result* uvatlas_create(
        const DirectX::XMFLOAT3* positions, size_t nVerts,
        const void* indices, bool is32Bit, size_t nFaces,
        size_t maxChartNumber, float maxStretch,
        size_t width, size_t height,
        float gutter,
        const uint32_t* adjacency, const uint32_t* falseEdgeAdjacency,
        const float* pIMTArray,
        /*std::function<HRESULT(float percentComplete)> statusCallBack,*/
        float callbackFrequency,
        uvatlas_options options,
        HRESULT* result)
    {
        uvatlas_result* atlas_result = new uvatlas_result();

        std::vector<DirectX::UVAtlasVertex> vMeshOutVertexBuffer;
        std::vector<uint8_t> vMeshOutIndexBuffer;
        std::vector<uint32_t> facePartitioning;
        std::vector<uint32_t> vertexRemapArray;

        *result = DirectX::UVAtlasCreate(positions, nVerts,
            indices, is32Bit ? DXGI_FORMAT_R32_UINT : DXGI_FORMAT_R16_UINT, nFaces,
            maxChartNumber, maxStretch,
            width, height,
            gutter,
            adjacency, falseEdgeAdjacency,
            pIMTArray,
            nullptr,
            callbackFrequency,
            (DirectX::UVATLAS)options,
            vMeshOutVertexBuffer,
            vMeshOutIndexBuffer,
            &facePartitioning,
            &vertexRemapArray,
            &atlas_result->stretch,
            &atlas_result->charts);

        if (FAILED(*result))
        {
            delete atlas_result;
            return nullptr;
        }

        atlas_result->verticesCount = static_cast<uint32_t>(vMeshOutVertexBuffer.size());
        atlas_result->indicesCount = static_cast<uint32_t>(vMeshOutIndexBuffer.size());
        atlas_result->vertices = new uvatlas_vertex[vMeshOutVertexBuffer.size()];
        atlas_result->indices = new uint8_t[vMeshOutIndexBuffer.size()];
        memcpy(atlas_result->vertices, vMeshOutVertexBuffer.data(), vMeshOutVertexBuffer.size());
        memcpy(atlas_result->indices, vMeshOutIndexBuffer.data(), vMeshOutIndexBuffer.size());
        return atlas_result;
    }

    EXPORT_C_API void uvatlas_delete(uvatlas_result* result)
    {
        delete[] result->vertices;
        delete[] result->indices;
        delete result;
        result = nullptr;
    }

#ifdef __cplusplus
}
#endif

