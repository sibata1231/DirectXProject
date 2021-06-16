#ifndef ___EMITTER_H___
#define ___EMITTER_H___

#include "Particle.h"
#include "ParticleMesh.h"
#include <list>

#define MAX_PARTICLE (MAX_INSTANCE_WIDTH * MAX_INSTANCE_HEIGHT)

class Emitter : public Component {
protected:
	Vector3			          m_speed;				// 基準速度(初速)
    bool				      m_isLooping;			// ループフラグ
	bool				      m_isActive;			// アクティブフラグ
	bool				      m_isGravity;			// 重力フラグ
	bool				      m_isPrewarm;			// 事前準備フラグ
	int					      m_maxParticle;		// パーティクル最大数
	int					      m_emitterGenerate;	// 毎フレーム発生数
	float				      m_lifeTime;			// 寿命時間
	float				      m_durationTime;		// エミッター起動時間
	float				      m_delayTime;			// 発生遅延時間
	float				      m_emitterCountTime;	// エミッター時間カウント用
   	BlendStates			      m_blendMode;		    // ブレンドステート
    ID3D11ShaderResourceView *m_texture;            // テクスチャ    
    XMINT2				      m_animationSize;		// アニメーション分割数
    ObjMaterial               m_material;           // マテリアル

    ParticleMesh*        m_particleMesh; // パーティクルメッシュ
    MeshRenderer*        m_meshRenderer; // MeshRenderer
public:
						Emitter();
	virtual				~Emitter();
    virtual void        Start();
	virtual void		Update();
    virtual void		SetEmitter();
	void				Draw();

    void                SetTexture(const char* passName);                                           // テクスチャ指定処理
    void                SetMaxParticle(int maxParticle);                                            // パーティクル最大数指定
    BlendStates			GetBlendState() { return m_blendMode; }
    ObjMaterial			GetMaterial() { return m_material; }
private:

public:
    std::vector<Object* >   m_particleList;		// パーティクル

protected:
	void				  SetDuration(float time) { m_durationTime = time * 60.0f; }				// 継続時間
	void				  SetLooping(bool set) { m_isLooping = set; }								// ループするかしないか
	void				  SetPrewarm(bool set) { m_isPrewarm = set; }								// 事前実行
	void				  SetStartDelay(float time) { m_delayTime = 60.0f * time; }					// 発生遅延
	void				  SetStartLifeTime(float time) { m_lifeTime = 60.0f * time; }				// 寿命
	void				  SetStartSpeed(Vector3 speed) { m_speed = speed; }							// 初速
    void				  SetStartSize(Vector3 size) { m_transform->m_scale = size; } // 初期サイズ
    void				  SetStartPos(Vector3 pos) { m_transform->m_position = pos; }				// 初期サイズ
	void				  SetStartRotation(Vector3 rotation) { m_transform->m_rotation = rotation; }// 初期回転速度
	//void				  SetStartColor(XMFLOAT4 color);											// 初期色
	void				  SetGenerate(int num) { m_emitterGenerate = num; }							// 発生数指定
	void				  SetGravityMultiplier(bool set) { m_isGravity = set; };					// 重力
	//void				  SetInheritVelocit();														// 継承速度
	//void				  SetSimulationSpace();														// 独自制御
	void				  SetPlayOnAwake(bool set) { m_isActive = set; };							// エミッタ発動処理
	void				  SetBlendState(BlendStates mode) { m_blendMode = mode; }					// ブレンドステート指定
	//void				  SetAnimation(AnimationMode m_animationMode,int sizeX, int sizeY);			// アニメーション指定関数
    void                  CreateParticle(const char* passName);
};

#endif