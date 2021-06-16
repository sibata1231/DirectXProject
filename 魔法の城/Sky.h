#ifndef ___SKY_H___
#define ___SKY_H___
#include "Model.h"

class Sky : public Component {
public:
    Sky();
    ~Sky();
    void Start();
    void Update();
    void Draw();
    void SetStatus(int status);
};

#endif