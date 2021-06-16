#ifndef ___SKY_LAVA_H___
#define ___SKY_LAVA_H___
#include "Model.h"

class SkyLava : public Component {
public:
    SkyLava();
    ~SkyLava();
    void Start();
    void Update();
    void Draw();
    void SetStatus(int status);
};

#endif