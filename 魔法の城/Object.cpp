#include "Object.h"
#include "Transform.h"
#include "ModelRenderer.h"
#include <map>
#include "Light.h"
#include "DirectGraphics.h"

Object::Object(Object* parent) : m_parent(parent){
	m_isActive  = true;
    m_isDeath   = false;
    static std::mt19937 rand_src;
    m_id = rand_src();
    m_transform = new Transform();
    m_transform->m_object = this;
    m_componentList.emplace_back(m_transform);
}

Object::~Object() {
    m_isActive = false;
    for (auto component : m_componentList) {
        SAFE_DELETE(component);
    }
    m_componentList.clear();
}

void Object::Update() {
    for (auto component : m_componentList) {
        if (!component->m_isEnable) {
            continue;
        }
        component->Update();
    }
}

void Object::Debug() {
    for (auto component : m_componentList) {
        if (!component->m_isEnable) {
            continue;
        }
        if (ImGui::TreeNode(component->GetName().c_str())) {
            component->Debug();
            ImGui::TreePop();
        }
    }
}

void Object::Draw() {
    for (auto component : m_componentList) {
        if (!component->m_isEnable) {
            continue;
        }
        component->Draw();
    }
}

#include "Shadow.h"
void ObjectManager::Init() {
    //m_shadow = new Object(nullptr);
    //m_shadow->AddComponent<Shadow>();
}

void ObjectManager::Release() {
    ObjectManager::AllDelete();
    //SAFE_DELETE(m_shadow);
}

void ObjectManager::Add(Object* object) {
	m_objectbufferList.push_back(object);
}

void ObjectManager::Delete(std::string name) {
    for (auto itr = m_objectList.begin(); itr != m_objectList.end();) {
        Object* obj = *itr;
		if (obj->GetComponent<Transform>()->GetName() == name) {
            obj->m_isDeath = true;
		}
		itr++;
    }
}

void ObjectManager::Delete(int id) {
    for (auto object : m_objectbufferList) {
        if (object->m_id == id) {
            object->m_isDeath = true;
        }
    }
}

void ObjectManager::Delete() {
    for (auto itr = m_objectbufferList.begin(); itr != m_objectbufferList.end();) {
        Object *obj = *itr;
        if (obj->m_isDeath) {
#ifdef _DEBUG
            printf("DEL[Name : %15s],[Id : %10d] \n", obj->m_transform->m_name.c_str(), obj->m_id);
#endif
            SAFE_DELETE(obj);
            itr = m_objectbufferList.erase(itr);
            continue;
        }
        itr++;
    }
    m_objectList = m_objectbufferList;
}

void ObjectManager::AllDelete() {
	for (auto object :  m_objectList) {
        SAFE_DELETE(object);
	}
    m_objectbufferList.clear();
	m_objectList.clear();
    m_rendererList.clear();
}

void ObjectManager::Update() {
    // 影情報更新
    //m_shadow->Update();

    // オブジェクト情報更新
    m_objectList = m_objectbufferList;
    for (auto object : m_objectList) {
        if (object->m_isDeath || !object->m_isActive) {
            continue;
        }
        object->Update();
    }
    Delete();       // 削除
    OnCollision();  // 個リジョンチェック
    RendererSort(); // レンダリング操作
}

void ObjectManager::Debug() {

    // 影情報更新
    //if (ImGui::CollapsingHeader(m_shadow->m_transform->m_name.c_str())) {
    //    m_shadow->Debug();
    //}

    // オブジェクト情報更新
    std::map<std::string, std::list<Object*>> debugObject;
    for (auto itr = m_objectList.begin(); itr != m_objectList.end(); itr++) {
        Object * object = *itr;
        if (object->m_parent) {
            debugObject[object->m_parent->m_transform->m_name].push_back(object);
        } else {
            debugObject[object->m_transform->m_name].push_back(object);
        }
    }
    std::string preName;
    for (auto itr = debugObject.begin(); itr != debugObject.end(); itr++) {
        std::string name    = itr->first;
        std::list<Object* > objectList = itr->second;
        if (ImGui::CollapsingHeader(name.c_str())) {
            for (auto objectItr = objectList.begin(); objectItr != objectList.end(); objectItr++) {
                Object* object = *objectItr;
                ImGui::Checkbox("Active", &object->m_isActive);
                if (ImGui::TreeNode(std::to_string(object->m_id).c_str(),object->m_transform->m_name.c_str())) {
                    object->Debug();
                    ImGui::TreePop();
                }
            }
        }
    }
}

// Rendererソート
void ObjectManager::RendererSort() {
    m_rendererList.clear();
    for (auto itr = m_objectbufferList.begin(); itr != m_objectbufferList.end(); itr++) {
        Object * obj = *itr;
        if (obj->GetComponentID("MeshRenderer")) {
            m_rendererList.insert(std::make_pair(RendererTags::MESH, obj));
            continue;
        } else if (obj->GetComponentID("ModelRenderer")) {
            ModelRenderer* modelRenderer = obj->GetComponent<ModelRenderer>();
            if (modelRenderer->m_rendererTag == RendererTags::BACKGROUND) {
                m_rendererList.insert(std::make_pair(RendererTags::BACKGROUND, obj));
            }
            else if (modelRenderer->m_rendererTag == RendererTags::MODEL) {
                m_rendererList.insert(std::make_pair(RendererTags::MODEL, obj));
            }
            continue;
        } else if (obj->GetComponentID("SpriteRenderer")) {
            m_rendererList.insert(std::make_pair(RendererTags::SPRITE, obj));
            continue;
        }
    }

}

// 影描画
void ObjectManager::ShadowRenderer() {
    float viewD = 10.0f;
    // 光源視線計算
    DirectX::XMVECTOR lightPos    = Light::GetInstance().m_position;
    DirectX::XMVECTOR lightDir    = Light::GetInstance().m_direction;
    DirectX::XMVECTOR eye         = lightPos;
    DirectX::XMVECTOR focus       = DirectX::XMVectorAdd(lightPos, DirectX::XMVectorScale(lightDir, viewD));
    DirectX::XMMATRIX lightView   = DirectX::XMMatrixLookAtLH(eye, focus, DirectX::XMVectorSet(0, 1, 0, 0));                      // Lightの視線情報
    DirectX::XMMATRIX lightProj   = DirectX::XMMatrixOrthographicLH(100, 100, 0.1f, viewD);                                       // ScaleModel (100)
    DirectX::XMMATRIX lightScreen = DirectX::XMMatrixScaling(0.5f, -0.5f, 1.0f) * DirectX::XMMatrixTranslation(0.5f, 0.5f, 0.0f); // Screen位置調整

    float color[] = { 0, 0, 0, 0 };
    DirectGraphics *graphics = &DirectGraphics::GetInstance();

    // 影情報描画
    graphics->SetRenderTargets("DepthWrite", "DepthWrite", color);
    graphics->UpdateViewProjection(lightView, lightProj, lightPos, lightDir);
    for (auto itr = m_rendererList.begin(); itr != m_rendererList.end(); itr++) {
        RendererTags tag = itr->first;
        Object*      object = itr->second;
        if (tag != RendererTags::MODEL || object->GetComponent<ModelRenderer>()->m_shadowType == ShadowTypes::NONE) {
            continue;
        }
        DirectGraphics::GetInstance().UpdateShader(DirectGraphics::ShaderType::TYPE_VERTEX, "DepthWrite");
        DirectGraphics::GetInstance().UpdateShader(DirectGraphics::ShaderType::TYPE_PIXEL , "DepthWrite");
        DirectGraphics::GetInstance().UpdateLayout("DepthWrite");
        object->Draw();
    }

    // 影描画
    graphics->SetRenderTargets("Default", "Default");
    graphics->UpdateCamera();
    graphics->UpdateLight();
    ID3D11ShaderResourceView* texture[] = {
        graphics->GetRenderTargetTexture("DepthWrite"),
    };
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL,DirectGraphics::TextureData(0, 1, nullptr),"DepthShadow");
    graphics->SetTexture(DirectGraphics::ShaderType::TYPE_PIXEL,DirectGraphics::TextureData(1, 1, texture),"DepthShadow");
    graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_VERTEX,"DepthShadow");
    graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_PIXEL , "DepthShadow");
    graphics->UpdateLayout("DepthShadow");
    graphics->UpdateLight(DirectX::XMMatrixTranspose(lightView * lightProj * lightScreen));
    m_shadow->Draw();
}

// 通常描画
void ObjectManager::ObjectRenderer() {
    DirectGraphics *graphics = &DirectGraphics::GetInstance();
    graphics->SetRenderTargets("Default", "Default");

    for (auto itr = m_rendererList.begin(); itr != m_rendererList.end(); itr++) {
        RendererTags tag    = itr->first;
        Object*      object = itr->second;
        if (!object->m_isActive) {
            continue;
        }
        if (tag == RendererTags::MODEL || tag == RendererTags::BACKGROUND || tag == RendererTags::MESH) {
            std::string shaderName = object->GetComponent<Renderer>()->m_shaderName;
            graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_VERTEX, shaderName);
            graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_PIXEL,  shaderName);
            graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_HULL,   shaderName);
            graphics->UpdateShader(DirectGraphics::ShaderType::TYPE_DOMAIN, shaderName);
            graphics->UpdateLayout(shaderName);
        }
        object->Draw();
    }
}

void ObjectManager::Draw() {
    //ShadowRenderer();
    ObjectRenderer();
}

Object * ObjectManager::Find(std::string name) {
    for (auto itr = m_objectbufferList.begin(); itr != m_objectbufferList.end(); itr++) {
        Object * obj = *itr;
        if (obj->GetComponent<Transform>()->GetName() == name) {
            return obj;
        }
    }
    return nullptr;
}

std::list<Object * > ObjectManager::FindTag(std::string name) {
    std::list<Object *> objectList;
    for (auto itr = m_objectbufferList.begin(); itr != m_objectbufferList.end(); itr++) {
        Object * obj = *itr;
        if (obj->GetComponent<Transform>()->GetTagName() == name) {
            objectList.push_back(obj);
        }
    }
    return objectList;
}

#define SIZE (20.0f)
void ObjectManager::OnCollision() {
    return;
    size_t listSize = m_objectList.size();
    for (int a = 0; a < (listSize - 1); a++) {
        for (int b = (a + 1); b < listSize; b++) {
            Transform *At = m_objectList[a]->GetComponent<Transform>();
            Transform *Bt = m_objectList[b]->GetComponent<Transform>();
            if (At == nullptr || Bt == nullptr) {
                continue;
            }            
            float vx = (At->m_position.x - Bt->m_position.x);
            float vy = (At->m_position.y - Bt->m_position.y);
            float vz = (At->m_position.z - Bt->m_position.z);
            float len = sqrtf(vx * vx + vy * vy + vz * vz);
            float distance = SIZE + SIZE + SIZE - len;
            if (distance < 0) {
                continue;
            }
            if (len > 0) {
                len = 1 / len;
            }

            vx *= len;
            vy *= len;            
            vz *= len;
            // 補正
            distance /= 2.0f;
            At->m_position.x += vx * distance;
            At->m_position.y += vy * distance;
            At->m_position.z += vz * distance;
            Bt->m_position.x -= vx * distance;
            Bt->m_position.y -= vy * distance;
            Bt->m_position.z -= vy * distance;
        }
    }
    m_objectList = m_objectbufferList;
}

/* BBOX
            Transform *At = m_objectList[a]->GetComponent<Transform>();
            Transform *Bt = m_objectList[b]->GetComponent<Transform>();
            if (At == nullptr || Bt == nullptr) {
                continue;
            }
            float scale = 1.0f;
            float& Ax = At->m_position.x;
            float& Ay = At->m_position.y;
            float& Az = At->m_position.z;
            float& Aw = scale;
            float& Ah = scale;
            float& Ad = scale;
            float& Bx = Bt->m_position.x;
            float& By = Bt->m_position.y;
            float& Bz = Bt->m_position.z;
            float& Bw = scale;
            float& Bh = scale;
            float& Bd = scale;
            bool hit = Ax - Aw <= Bx + Bw &&
                Bx - Bw <= Ax + Aw &&
                Ay - Ah <= By + Bh &&
                By - Bh <= Ay + Ah &&
                Az - Ad <= Bz + Bd &&
                Bz - Bd <= Az + Ad;
            if (hit) {
                m_objectList[a]->OnCollisionEnter(m_objectList[b]);
                m_objectList[b]->OnCollisionEnter(m_objectList[a]);
            }

*/