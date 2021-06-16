#ifndef ___FX_SNOW_H___
#define ___FX_SNOW_H___

#include "Emitter.h"

class FxSnow : public Emitter {
public:
    FxSnow() {};
    virtual ~FxSnow() {};
    virtual void Start() override;
    virtual void Update() override;
    virtual void SetEmitter() override;
};

#endif