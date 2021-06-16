#ifndef ___OBJECT_H___
#define ___OBJECT_H___

#include "SystemMain.h"
#include <vector>
#include <list>
#include <map>
#include <functional>
#include <random>
#include <memory>
#include "Component.h"
#include "GraphicsUtility.h"

class Object {
public:
    UINT                  m_id;            // オブジェクト識別番号
    bool			      m_isActive;	   // 存在するかどうか
    bool                  m_isDeath;       // 生存フラグ
    std::list<Component*> m_componentList; // コンポーネントリスト
    Transform*            m_transform;     // トランスフォーム
    Object*               m_parent;        // オブジェクトの親オブジェクト
public:
			     Object(Object* parent);
    virtual	     ~Object();
    virtual void Update();
    virtual void Draw();
    virtual void Debug();
            void SetActive(bool isActive) { m_isActive = isActive; }

    //オブジェクトが持っているコンポーネントを取得
    template<class T>
    T* GetComponent() {
        for (auto com : m_componentList) {
            T* buff = dynamic_cast<T*>(com);
            if (buff != nullptr) {
                return buff;
            }
        }
        return nullptr;
    }

    //オブジェクトが持っているコンポーネントを取得
    Component* GetComponentID(std::string name) {
        for (auto com : m_componentList) {
            if (com->GetId().find(name) != std::string::npos) {
                return com;
            }
        }
        return nullptr;
    }

    //オブジェクトが持っているコンポーネント名を取得
    std::list<std::string> GetComponentName() {
        std::list<std::string> m_comName;
        for (auto com : m_componentList) {
            m_comName.push_back(com->GetId());
        }
        return m_comName;
    }
    //オブジェクトが持っているコンポーネントを追加
    template<class T>
    T* AddComponent() {
        T* buff                 = new T();
        buff->m_parent          = this;
        buff->m_transform       = AddComponentIf<Transform>();
        buff->m_componentName   = typeid(buff).name();
#if _DEBUG
        //printf("Component[Add]...%20s\n", buff->m_componentName.c_str());
#endif
        m_componentList.push_back(buff);
        buff->Start();
        return buff;
    }

    //オブジェクトが持っているコンポーネントを削除
    template<class T>
    void RemoveComponent() {
        for (auto itr = m_componentList.begin(); itr != m_componentList.end();) {
            std::shared_ptr<T> buff  = dynamic_cast<std::shared_ptr<T>>(itr);
            if (buff != nullptr) {
                itr   = m_componentList.erase(itr);
                continue;
            }
            itr++;
        }
    }

    //オブジェクトが持っているコンポーネントがなければ追加、あれば戻り値返却
    template<class T>
    T* AddComponentIf() {
        T* buff = this->GetComponent<T>();
        if (buff) {
            return buff;
        }
        return AddComponent<T>();
    }

    void OnCollisionEnter(Object* object) {
        for (auto com : m_componentList) {
            com->OnCollisionEnter(object);
        }
    }

    void OnCollisionStay(Object* object) {
        for (auto com : m_componentList) {
            com->OnCollisionStay(object);
        }
    }

    void OnCollisionExit(Object* object) {
        for (auto com : m_componentList) {
            com->OnCollisionExit(object);
        }
    }
};

enum class RendererTags {
    NONE = 0,   //!< 初期(何もなし)
    BACKGROUND, //!< 背景(モデル)
    MESH,       //!< メッシュ用(メッシュ)
    MODEL,      //!< モデル用(モデル)
    SPRITE,     //!< 2D用(2D)
};

class ObjectManager {    
private:
    std::vector<Object*> m_objectbufferList;
	std::vector<Object*> m_objectList;

    Object*              m_shadow;

    std::multimap<RendererTags, Object*> m_rendererList;
    void Delete();
    void OnCollision();
    void ShadowRenderer();
    void ObjectRenderer();

public:
    void Init();
    void Release();

    void Add(Object* object);
	void Delete(std::string name);
    void Delete(int id);
    void RendererSort();
    void Debug();
	void AllDelete();
    void Update();
    void Draw();
    Object* Find(std::string name);
    std::list<Object*> FindTag(std::string name);
};
#endif