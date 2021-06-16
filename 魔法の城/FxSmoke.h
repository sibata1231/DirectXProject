#ifndef ___FX_SMOKE_H___
#define ___FX_SMOKE_H___

#include "Emitter.h"

class FxSmoke : public Emitter {
public:
    FxSmoke() {};
    virtual ~FxSmoke() {};
    virtual void Start() override;
    virtual void Update() override;
    virtual void SetEmitter() override;
};

#endif