#ifndef GRAPHICS_UTILITY_H_
#define GRAPHICS_UTILITY_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <string>
struct Vector3 : public DirectX::XMFLOAT3 {
    Vector3() = default;
    Vector3(float x, float y, float z) {
        this->x = x; this->y = y; this->z = z;
    }
    //コンストラクタ
    Vector3(const DirectX::XMVECTOR& other) :XMFLOAT3() {
        DirectX::XMVECTOR temp = other;
        XMStoreFloat3(this, temp);
    }

    inline BOOL operator == (const Vector3&r) const { return x == r.x && y == r.y && z == r.z; }
    inline BOOL operator != (const Vector3&r) const { return x != r.x || y != r.y || z != r.z; }
    inline DirectX::XMVECTOR operator *(const float r) const { return Vector3(x * r, y * r, z * r); }
    inline DirectX::XMVECTOR operator /(const float r) const { return Vector3(x / r, y / r, z / r); }

    // ベクトルの内積
    float VDot(Vector3 In) { return x * In.x + y * In.y + z * In.z; }
    // ベクトルの外積
    Vector3 VCross(Vector3 In) { return Vector3(y * In.z - z * In.y, z * In.x - x * In.z, x * In.y - y * In.x); }
    // ベクトルのスケーリング
    Vector3 VScale(float Scale) { Vector3 Result = { x * Scale, y * Scale, z * Scale }; return Result; }

    //代入
    Vector3& operator=(const DirectX::XMVECTOR& other) {
        DirectX::XMVECTOR temp = other;
        XMStoreFloat3(this, temp);
        return *this;
    }
    //キャスト
    operator DirectX::XMVECTOR() const {
        return XMLoadFloat3(this);
    }
    //長さ
    float Length() const {
        return (static_cast<Vector3>(DirectX::XMVector3Length(DirectX::XMVECTOR(*this)))).x;
    }
    //正規化
    void Normalize() {
        *this = DirectX::XMVector3Normalize(DirectX::XMVECTOR(*this));
    }
};

struct Vector2 {
    Vector2() {
        this->x = 0.0f;
        this->y = 0.0f;
    }

    Vector2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    float x;
    float y;
};

struct Color {
    float Red;
    float Green;
    float Blue;
    float Alpha;

    Color(float red, float green, float blue, float alpha) {
        Red = red;
        Green = green;
        Blue = blue;
        Alpha = alpha;
    }

    Color() {
        Red = Green = Blue = Alpha = 1.0f;
    }

};

//=====================================================================//
//! ポリゴン出力用カスタムバーテックス構造体
//=====================================================================//
struct CustomVertex {
    Vector3 Posision;		// 座標(x, y, z)
    Vector3 Normal;			// 法線
    Color   Color;			// 頂点カラー
    Vector2 TexturePos;		// テクスチャ座標(u, v)
};

struct ObjMaterial {
    ObjMaterial() {
        for (int i = 0; i < 4; i++) {
            Ambient[i] = 1.0f;
            Diffuse[i] = 1.0f;
            Specular[i] = 1.0f;
            Emissive[i] = 1.0f;
        }
        //TextureKeyWord = "";
        //TextureName = "";
    }

    void SetAmbient(float r, float g, float b, float factor) {
        Ambient[0] = r;
        Ambient[1] = g;
        Ambient[2] = b;
        Ambient[3] = factor;
    }

    void SetDiffuse(float r, float g, float b, float factor) {
        Diffuse[0] = r;
        Diffuse[1] = g;
        Diffuse[2] = b;
        Diffuse[3] = factor;
    }

    void SetSpecular(float r, float g, float b, float factor) {
        Specular[0] = r;
        Specular[1] = g;
        Specular[2] = b;
        Specular[3] = factor;
    }

    void SetEmissive(float r, float g, float b, float factor) {
        Emissive[0] = r;
        Emissive[1] = g;
        Emissive[2] = b;
        Emissive[3] = factor;
    }

    float Ambient[4];
    float Diffuse[4];
    float Specular[4];
    float Emissive[4];
    float Alpha;

    //std::string TextureKeyWord;
    //std::string TextureName;
};

enum class CullModes {
    NONE = 0,	// カリングしない
    FRONT,		// 前面カリング
    BACK,		// 背面カリング
    MAX,        // カリング最大数
};

enum class BlendStates {
    NONE = 0,	 //!< 半透明合成無し
    ALPHABLEND,	 //!< 半透明合成
    ADDITIVE,	 //!< 加算合成
    SUBTRACTION, //!< 減算合成
    MAX,         //!< ブレンドステート最大数
};

struct CameraBuffer {
    DirectX::XMFLOAT4X4 m_view;
    DirectX::XMFLOAT4X4 m_projection;
    DirectX::XMFLOAT4	m_cameraPos;
    DirectX::XMFLOAT4	m_cameraVector;
};

struct LightBuffer {
    DirectX::XMFLOAT4   m_pos;
    DirectX::XMFLOAT4   m_vector;
    DirectX::XMFLOAT4   m_ambient;
    DirectX::XMFLOAT4   m_diffuse;
    DirectX::XMFLOAT4   m_specular;
    DirectX::XMFLOAT4X4 m_matrix;
};

struct WorldBuffer {
    DirectX::XMFLOAT4X4 m_wvp;
    DirectX::XMFLOAT4X4 m_world;
};


struct MaterialBuffer {
    DirectX::XMFLOAT4 m_ambient;
    DirectX::XMFLOAT4 m_diffuse;
    DirectX::XMFLOAT4 m_specular;
    DirectX::XMFLOAT4 m_emissive;
};

struct TextureBuffer {
    DirectX::XMFLOAT4X4 m_tex;		// テクスチャ行列(転置行列)
};

#define MAX_INSTANCE_HEIGHT (5)
#define MAX_INSTANCE_WIDTH  (5)

struct InstanceBuffer {
    DirectX::XMMATRIX m_world[MAX_INSTANCE_HEIGHT * MAX_INSTANCE_WIDTH];
    InstanceBuffer() {
        for (int i = 0; i < MAX_INSTANCE_HEIGHT * MAX_INSTANCE_WIDTH; i++) {
            m_world[i] = DirectX::XMMatrixIdentity();
        }
    }
};

struct POMBuffer {
    float m_specularArea;  //!< スペキュラー範囲
    float m_specularPower; //!< スペキュラー強度
    float m_uFrame;        //!< UVフレーム(U)
    float m_vFrame;        //!< UVフレーム(V)
};

struct Wave {
    Vector2 dir;
    float   amplitude;
    float   waveLength;
};

struct TessellationBuffer {
    float tessellationAmount; //!< Tessellation係数
    float Hscale;             //!< サイズ
    float Hbias;              //!< バイアス
    float time;               //!< 時間
    Wave  waves[100];         //!< 波100生成
};

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) {if(x){(x)->Release();x=nullptr;}}
#endif
#ifndef SAFE_DELETE
#define SAFE_DELETE(x) {if(x){delete(x);x=nullptr;}}
#endif
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) {if(x){delete[](x);x=nullptr;}}
#endif

class ConstantBuffer { 
public:
    float           m_size;
    float           m_registerIndex;
    void*           m_buffer;
    std::string     m_BufferName;
public:
    int inline GetSize() {
        return (int)m_size;
    }
    int inline GetRegister() {
        return (int)m_registerIndex;
    }
};


#endif
