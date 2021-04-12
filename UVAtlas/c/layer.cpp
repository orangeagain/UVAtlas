#include "pch.h"
#include "DirectXMesh.h"
#include "UVAtlas.h"

#if defined(_MSC_VER)
#   define EXPORT_C_API __declspec(dllexport)
#else
#	define EXPORT_C_API __attribute__((visibility("default")))
#endif

#ifdef __cplusplus
extern "C" {
#endif

    /* DirectXMesh */
    EXPORT_C_API HRESULT GenerateAdjacencyAndPointReps_UInt16(
        const uint16_t* indices, size_t nFaces,
        const DirectX::XMFLOAT3* positions, _In_ size_t nVerts,
        float epsilon,
        uint32_t* pointRep,
        uint32_t* adjacency)
    {
        return DirectX::GenerateAdjacencyAndPointReps(
            indices, nFaces,
            positions, nVerts,
            epsilon,
            pointRep,
            adjacency);
    }

    EXPORT_C_API HRESULT GenerateAdjacencyAndPointReps_UInt32(
        const uint32_t* indices, size_t nFaces,
        const DirectX::XMFLOAT3* positions, _In_ size_t nVerts,
        float epsilon,
        uint32_t* pointRep,
        uint32_t* adjacency)
    {
        return DirectX::GenerateAdjacencyAndPointReps(
            indices, nFaces,
            positions, nVerts,
            epsilon,
            pointRep,
            adjacency);
    }

    typedef enum ComputeNormalsFlags {
        CNORM_DEFAULT = DirectX::CNORM_DEFAULT,
        CNORM_WEIGHT_BY_AREA = DirectX::CNORM_WEIGHT_BY_AREA,
        CNORM_WEIGHT_EQUAL = DirectX::CNORM_WEIGHT_EQUAL,
        CNORM_WIND_CW = DirectX::CNORM_WIND_CW,
        __FORCE_U32 = 0x7FFFFFFF
    } ComputeNormalsFlags;

    EXPORT_C_API HRESULT ComputeNormals_UInt16(
        const uint16_t* indices, size_t nFaces,
        const DirectX::XMFLOAT3* positions, size_t nVerts,
        ComputeNormalsFlags flags,
        DirectX::XMFLOAT3* normals)
    {
        return DirectX::ComputeNormals(indices, nFaces, positions, nVerts, (DirectX::CNORM_FLAGS)flags, normals);
    }

    EXPORT_C_API HRESULT ComputeNormals_UInt32(
        const uint32_t* indices, size_t nFaces,
        const DirectX::XMFLOAT3* positions, size_t nVerts,
        ComputeNormalsFlags flags,
        DirectX::XMFLOAT3* normals)
    {
        return DirectX::ComputeNormals(indices, nFaces, positions, nVerts, (DirectX::CNORM_FLAGS)flags, normals);
    }

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
        uint32_t*           indices;
        uint32_t*           facePartitioning;
        uint32_t*           vertexRemapArray;
        float               stretch;
        size_t              charts;
    } uvatlas_result;

    EXPORT_C_API uvatlas_result* uvatlas_create_uint32(
        const DirectX::XMFLOAT3* positions, size_t nVerts,
        const uint32_t* indices, size_t nFaces,
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
            indices, DXGI_FORMAT_R32_UINT, nFaces,
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

        auto newIB = reinterpret_cast<const uint32_t*>(&vMeshOutIndexBuffer.front());

        atlas_result->verticesCount = static_cast<uint32_t>(vMeshOutVertexBuffer.size());
        atlas_result->indicesCount = static_cast<uint32_t>(nFaces * 3);
        atlas_result->vertices = new uvatlas_vertex[vMeshOutVertexBuffer.size()];
        atlas_result->indices = new uint32_t[atlas_result->indicesCount];
        atlas_result->facePartitioning = new uint32_t[vMeshOutVertexBuffer.size()];
        atlas_result->vertexRemapArray = new uint32_t[vMeshOutVertexBuffer.size()];
        memcpy(atlas_result->vertices, vMeshOutVertexBuffer.data(), vMeshOutVertexBuffer.size());
        memcpy(atlas_result->indices, newIB, atlas_result->indicesCount * sizeof(uint32_t));
        memcpy(atlas_result->facePartitioning, facePartitioning.data(), facePartitioning.size());
        memcpy(atlas_result->vertexRemapArray, vertexRemapArray.data(), vertexRemapArray.size());
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

    EXPORT_C_API HRESULT UVAtlasApplyRemap(
        const void* vbin,
        size_t stride,
        size_t nVerts,
        size_t nNewVerts,
        const uint32_t* vertexRemap,
        void* vbout)
    {
        return DirectX::UVAtlasApplyRemap(vbin, stride, nVerts, nNewVerts, vertexRemap, vbout);
    }

#ifdef __cplusplus
}
#endif

