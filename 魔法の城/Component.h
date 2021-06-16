#ifndef ___COMPONENT_H___
#define ___COMPONENT_H___
#include <string>
#include <algorithm>

class Object;
class Transform;
class Component {
public:
    bool        m_isEnable;
    Object*     m_parent;
    Transform*  m_transform;
    std::string m_componentName = "<None.Component>";
public:
    Component() : m_isEnable(true) {}
    virtual ~Component()           {}
    virtual void Start()           {}
    virtual void Update()          {}
    virtual void Draw()            {}
    virtual void Debug()           {}
    virtual std::string GetId() { return m_componentName; }
    virtual std::string GetName() { 
        char chars[] = "";
        for (unsigned int i = 0; i < strlen(chars); ++i){
            m_componentName.erase(
                std::remove(
                    m_componentName.begin(),
                    m_componentName.end(),
                    chars[i]),
                m_componentName.end());
        }
        return m_componentName;
    }
    virtual void OnCollisionEnter(Object* object) {}
    virtual void OnCollisionStay (Object* object) {}
    virtual void OnCollisionExit (Object* object) {}
};

#endif