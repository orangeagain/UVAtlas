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

    static std::vector<DirectX::UVAtlasVertex> vMeshOutVertexBuffer;
    static std::vector<uint8_t> vMeshOutIndexBuffer;

    EXPORT_C_API HRESULT uvatlas_create(
        const DirectX::XMFLOAT3* positions, size_t nVerts,
        const void* indices, DXGI_FORMAT indexFormat, size_t nFaces,
        size_t maxChartNumber, float maxStretch,
        size_t width, size_t height,
        float gutter,
        const uint32_t* adjacency, const uint32_t* falseEdgeAdjacency,
        const float* pIMTArray,
        /*std::function<HRESULT(float percentComplete)> statusCallBack,*/
        float callbackFrequency,
        uvatlas_options options)
    {
        return DirectX::UVAtlasCreate(positions, nVerts,
            indices, indexFormat, nFaces,
            maxChartNumber, maxStretch,
            width, height,
            gutter,
            adjacency, falseEdgeAdjacency,
            pIMTArray,
            nullptr,
            callbackFrequency,
            (DirectX::UVATLAS)options,
            vMeshOutVertexBuffer,
            vMeshOutIndexBuffer);
    }

#ifdef __cplusplus
}
#endif

