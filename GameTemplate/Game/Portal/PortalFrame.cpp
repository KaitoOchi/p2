#include "stdafx.h"
#include "PortalFrame.h"
#include "Player.h"
#include "EnergyBall.h"
#include "GameCamera.h"
#include "PortalCamera.h"
#include "Stage.h"

namespace
{
	const Vector3	PORTAL_COLLISION_SIZE = Vector3(80.0f, 106.0f, 1.0f);		//コリジョンサイズ。
	const float		PORTAL_POS_NORMAL = 0.1f;									//ポータル設置時の座標。
	const float		PORTAL_ROT_X = 180.0f;										//ポータル設置時の上下回転。
	const float		PORTAL_LENGTH = 100.0f;										//ポータルの衝突判定を開始する範囲。
}


PortalFrame::PortalFrame()
{
	m_position = Vector3(0.0f, -1000.0f, 0.0f);
}

PortalFrame::~PortalFrame()
{
	DeleteGO(m_collisionObject);
	DeleteGO(m_frameEffect);
}

bool PortalFrame::Start()
{
	//プレイヤーオブジェクトを検索。
	m_player = FindGO<Player>("player");
	m_gameCamera = FindGO<GameCamera>("gameCamera");
	m_portalCamera = FindGO<PortalCamera>("portalCamera");
	m_stage = FindGO<Stage>("stage");

	//当たり判定を初期化。
	m_collisionObject = NewGO<CollisionObject>(0, "collision");
	m_collisionObject->CreateBox(m_position, m_rotation, PORTAL_COLLISION_SIZE);
	m_collisionObject->SetName("portal_red");
	m_collisionObject->SetIsEnable(false);

	//ポータルフレームを初期化。
	//ModelInitDataを使用して初期化する。
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = "Assets/modelData/portal/portalFrame.tkm";
	modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";

	if (m_portalType == enPortalType_Blue) {
		modelInitData.m_psEntryPointFunc = "PSBluePortal";
	}
	else if (m_portalType == enPortalType_Red) {
		modelInitData.m_psEntryPointFunc = "PSRedPortal";
	}

	modelInitData.m_expandConstantBuffer = &RenderingEngine::GetInstance()->GetLightCB();
	modelInitData.m_expandConstantBufferSize = sizeof(RenderingEngine::GetInstance()->GetLightCB());
	modelInitData.m_expandShaderResoruceView[0] = &RenderingEngine::GetInstance()->GetZPrepassRenderTarget().GetRenderTargetTexture();

	m_portalFrameModelRender.InitModelInitData(modelInitData);
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.Update();

	//ポータルフレームの当たり判定を初期化。
	m_portalFrameCollisionModelRender.Init("Assets/modelData/portal/portalFrameCollision.tkm");
	m_physicsStaticObject.CreateFromModel(m_portalFrameCollisionModelRender.GetModel(), m_portalFrameCollisionModelRender.GetModel().GetWorldMatrix(), 0.0f);
	m_physicsStaticObject.SetPosition(m_position);
	m_physicsStaticObject.GetbtCollisionObject()->setUserIndex(enCollisionAttr_PortalFrame);

	return true;
}

void PortalFrame::Update()
{
	if (!m_isEnable) {
		return;
	}

	Collision();
}

/// <summary>
/// 当たり判定の処理。
/// </summary>
void PortalFrame::Collision()
{
	//当たり判定が無効なら。
	if (!m_anotherPortalFrame->GetIsEnable()) {
		return;
	}

	CollisionPlayer();
	CollisionEnergyBall();
}

/// <summary>
/// プレイヤーとの当たり判定処理。
/// </summary>
void PortalFrame::CollisionPlayer()
{
	//ポータルとプレイヤーのベクトルが範囲外なら、衝突判定を行わない。
	Vector3 length = m_player->GetPosition() - m_position;
	if (length.LengthSq() > PORTAL_LENGTH * PORTAL_LENGTH) {
		return;
	}

	//プレイヤーと接触しているなら。
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(*m_player->GetCharacterController()))) {
		if (!m_isCollisionHit) {
			//キャラクターコントローラーをポータルに入った状態にする。
			m_player->SetIsPortal(true);
			m_isCollisionHit = true;
		}

		//比較用の座標を設定。
		float compareSmallPos = 0.0f;
		float compareLargePos = 0.0f;

		if (m_normal.x >= 1.0f) {
			compareLargePos = m_position.x;
			compareSmallPos = m_player->GetPosition().x;
		}
		else if (m_normal.x <= -1.0f) {
			compareLargePos = m_player->GetPosition().x;
			compareSmallPos = m_position.x;
		}
		else if (m_normal.z >= 1.0f) {
			compareLargePos = m_position.z;
			compareSmallPos = m_player->GetPosition().z;
		}
		else if (m_normal.z <= -1.0f) {
			compareLargePos = m_player->GetPosition().z;
			compareSmallPos = m_position.z;
		}
		else if (m_normal.y >= 1.0f) {
			compareLargePos = m_position.y;
			compareSmallPos = m_player->GetPosition().y;
		}
		else if (m_normal.y <= -1.0f) {
			compareLargePos = m_player->GetPosition().y;
			compareSmallPos = m_position.y;
		}

		//プレイヤーの座標がポータルの座標を超えていたら。
		if (compareLargePos > compareSmallPos) {
			IsHitPlayer();
		}
	}
	//衝突していないなら。
	else {
		if (m_isCollisionHit) {
			m_player->SetIsPortal(false);
			m_isCollisionHit = false;
		}
	}
}

/// <summary>
/// エネルギーボールとの当たり判定処理。
/// </summary>
void PortalFrame::CollisionEnergyBall()
{
	for (auto& ball : m_stage->GetEnergyBallObjects())
	{
		//ポータルとプレイヤーのベクトルが範囲外なら、衝突判定を行わない。
		Vector3 length = ball->GetPosition() - m_position;
		if (length.LengthSq() > PORTAL_LENGTH * PORTAL_LENGTH) {
			return;
		}

		//エネルギーボールと接触しているなら。
		if (m_collisionObject->IsHit(ball->GetCollisionObject())) {
			//比較用の座標を設定。
			float compareSmallPos = 0.0f;
			float compareLargePos = 0.0f;

			if (m_normal.x >= 1.0f) {
				compareLargePos = m_position.x;
				compareSmallPos = ball->GetPosition().x;
			}
			else if (m_normal.x <= -1.0f) {
				compareLargePos = ball->GetPosition().x;
				compareSmallPos = m_position.x;
			}
			else if (m_normal.z >= 1.0f) {
				compareLargePos = m_position.z;
				compareSmallPos = ball->GetPosition().z;
			}
			else if (m_normal.z <= -1.0f) {
				compareLargePos = ball->GetPosition().z;
				compareSmallPos = m_position.z;
			}
			else if (m_normal.y >= 1.0f) {
				compareLargePos = m_position.y;
				compareSmallPos = ball->GetPosition().y;
			}
			else if (m_normal.y <= -1.0f) {
				compareLargePos = ball->GetPosition().y;
				compareSmallPos = m_position.y;
			}

			//プレイヤーの座標がポータルの座標を超えていたら。
			if (compareLargePos > compareSmallPos) {
				IsHitEnergyBall(ball);
			}
		}
	}
}

/// <summary>
/// プレイヤーに当たったときの処理。
/// </summary>
void PortalFrame::IsHitPlayer()
{
	//2つのポータルの角度を計算。
	float angle = acosf(m_normal.Dot(m_anotherPortalFrame->GetNormal()));
	angle = Math::RadToDeg(angle);

	//外積を求め、y軸を使って±を補正する。
	Vector3 cross = m_normal;
	cross.Cross(m_anotherPortalFrame->GetNormal());
	if (cross.y <= -1.0f) {
		angle *= -1.0f;
	}

	//もう一方のポータルの座標まで移動する。
	m_player->SetWarp(m_anotherPortalFrame->GetPosition(), angle);

	m_isCollisionHit = false;
}

/// <summary>
/// エネルギーボールに当たったときの処理。
/// </summary>
void PortalFrame::IsHitEnergyBall(EnergyBall* ball)
{
	//もう一方のポータルの座標まで移動する。
	ball->SetWarp(m_anotherPortalFrame->GetPosition(), m_anotherPortalFrame->GetNormal());
}

//衝突したときに呼ばれる関数オブジェクト(地面用)
struct SweepResultGround : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;									//衝突フラグ。
	bool isHitMe = false;								//自身に衝突したかどうか。
	bool isResultGround = true;							//地面衝突を調べているかどうか。
	Vector3 hitPos = Vector3(0.0f, -FLT_MAX, 0.0f);		//衝突点。
	Vector3 startPos;									//レイの始点。
	Vector3 hitNormal;									//衝突点の法線。
	btCollisionObject* me = nullptr;					//自分自身。自分自身との衝突を除外するためのメンバ。
	float dist = FLT_MAX;								//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。

	//衝突したときに呼ばれるコールバック関数。
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (isHitMe) {
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject == me
			|| convexResult.m_hitCollisionObject->getUserIndex() == EnCollisionAttr::enCollisionAttr_PortalFrame
			) {
			//自分に衝突した。
			isHitMe = true;
			isHit = false;
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
			//ゴーストオブジェクトに衝突した。
			return 0.0f;
		}

		//地面と天井どちらの衝突を調べているか。
		float up = 1.0f;
		if (isResultGround == false) {
			up = -1.0f;
		}

		//衝突点の法線を引っ張ってくる。
		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;
		//上方向と法線のなす角度を求める。
		float angle = hitNormalTmp.y * up;
		angle = fabsf(acosf(angle));
		if (angle < Math::PI * 0.3f		//地面の傾斜が54度より小さいので地面とみなす。
			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Ground //もしくはコリジョン属性が地面と指定されている。
			) {
			//衝突している。
			isHit = true;
			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;
			//衝突点の距離を求める。。
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			float distTmp = vDist.Length();
			if (dist > distTmp) {
				//この衝突点の方が近いので、最近傍の衝突点を更新する。
				hitPos = hitPosTmp;
				hitNormal = *(Vector3*)&convexResult.m_hitNormalLocal;
				dist = distTmp;
			}
		}
		return 0.0f;
	}
};

//衝突したときに呼ばれる関数オブジェクト(壁用)
struct SweepResultWall : public btCollisionWorld::ConvexResultCallback
{
	bool isHit = false;						//衝突フラグ。
	bool isHitMe = false;					//自身に衝突したかどうか。
	Vector3 hitPos;							//衝突点。
	Vector3 startPos;						//レイの始点。
	float dist = FLT_MAX;					//衝突点までの距離。一番近い衝突点を求めるため。FLT_MAXは単精度の浮動小数点が取りうる最大の値。
	Vector3 hitNormal;						//衝突点の法線。
	btCollisionObject* me = nullptr;		//自分自身。自分自身との衝突を除外するためのメンバ。

	//衝突したときに呼ばれるコールバック関数。
	virtual	btScalar	addSingleResult(btCollisionWorld::LocalConvexResult& convexResult, bool normalInWorldSpace)
	{
		if (isHitMe) {
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject == me
			|| convexResult.m_hitCollisionObject->getUserIndex() == EnCollisionAttr::enCollisionAttr_PortalFrame
			) {
			//自分に衝突した。
			isHitMe = true;
			isHit = false;
			return 0.0f;
		}
		if (convexResult.m_hitCollisionObject->getInternalType() == btCollisionObject::CO_GHOST_OBJECT) {
			//ゴーストオブジェクトに衝突した。
			return 0.0f;
		}

		//衝突点の法線を引っ張ってくる。
		Vector3 hitNormalTmp = *(Vector3*)&convexResult.m_hitNormalLocal;

		//上方向と衝突点の法線のなす角度を求める。
		float angle = fabsf(acosf(hitNormalTmp.y));
		if (angle >= Math::PI * 0.3f		//地面の傾斜が54度以上なので壁とみなす。
			|| convexResult.m_hitCollisionObject->getUserIndex() == enCollisionAttr_Character	//もしくはコリジョン属性がキャラクタなので壁とみなす。
			) {
			isHit = true;
			Vector3 hitPosTmp = *(Vector3*)&convexResult.m_hitPointLocal;

			//交点との距離を調べる。
			Vector3 vDist;
			vDist.Subtract(hitPosTmp, startPos);
			vDist.y = 0.0f;
			float distTmp = vDist.Length();
			if (distTmp < dist) {
				//この衝突点の方が近いので、最近傍の衝突点を更新する。
				hitPos = hitPosTmp;
				dist = distTmp;
				hitNormal = hitNormalTmp;
			}
		}
		return 0.0f;
	}
};

/// <summary>
/// ポータルが設置できるかどうか。
/// </summary>
/// <param name="pos"></param>
/// <param name="normal"></param>
/// <returns></returns>
const bool PortalFrame::IsCanPut(const Vector3& pos, const Vector3& normal)
{
	bool isHit = false;
	Vector3 startPos = pos;
	startPos.x += fabsf(normal.x) * 5.0f;
	startPos.z += fabsf(normal.z) * 5.0f;


	Vector3 finalPos = pos;

	////右方向を調べる。
	//{
	//	btTransform start, end;
	//	start.setIdentity();
	//	end.setIdentity();
	//	//視点は座標の中心。
	//	start.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
	//	//終点はコリジョンの右辺。
	//	end.setOrigin(btVector3(
	//		startPos.x - ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z)),
	//		startPos.y,
	//		startPos.z - ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x))
	//	));

	//	SweepResultWall callback;
	//	callback.me = &m_collisionObject->GetbtCollisionObject();
	//	callback.startPos = startPos;

	//	PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

	//	//衝突した。
	//	if (callback.isHit) {
	//		finalPos.x = (callback.hitPos.x - (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z));
	//		finalPos.z = (callback.hitPos.z - (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x));
	//		isHit = callback.isHit;
	//	}
	//}

	////左方向を調べる。
	//{
	//	//右方向を調べる。
	//	btTransform start, end;
	//	start.setIdentity();
	//	end.setIdentity();
	//	//視点は座標の中心。
	//	start.setOrigin(btVector3(pos.x, pos.y, pos.z));
	//	//終点はコリジョンの左辺。
	//	end.setOrigin(btVector3(
	//		startPos.x + ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z)),
	//		startPos.y,
	//		startPos.z + ((PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x))
	//	));

	//	SweepResultWall callback;
	//	callback.me = &m_collisionObject->GetbtCollisionObject();
	//	callback.startPos = startPos;

	//	PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

	//	//衝突した。
	//	if (callback.isHit) {
	//		finalPos.x = (callback.hitPos.x + (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.z));
	//		finalPos.z = (callback.hitPos.z + (PORTAL_COLLISION_SIZE.x / 2.0f) * fabsf(normal.x));
	//		isHit = callback.isHit;
	//	}
	//}

	////上方向を調べる。
	//{
	//	btTransform start, end;
	//	start.setIdentity();
	//	end.setIdentity();
	//	//視点は座標の中心。
	//	start.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
	//	//終点はコリジョンの上辺。
	//	end.setOrigin(btVector3(
	//		startPos.x,
	//		startPos.y + (PORTAL_COLLISION_SIZE.y / 2.0f),
	//		startPos.z));

	//	SweepResultGround callback;
	//	callback.me = &m_collisionObject->GetbtCollisionObject();
	//	callback.isResultGround = false;
	//	callback.startPos = startPos;

	//	PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

	//	//衝突した。
	//	if (callback.isHit) {
	//		finalPos.y = callback.hitPos.y - (PORTAL_COLLISION_SIZE.y / 2.0f);
	//		isHit = callback.isHit;
	//	}
	//}

	//下方向を調べる。
	{
		btTransform start, end;
		start.setIdentity();
		end.setIdentity();
		//視点は座標の中心。
		start.setOrigin(btVector3(startPos.x, startPos.y, startPos.z));
		//終点はコリジョンの下辺。
		end.setOrigin(btVector3(
			startPos.x,
			startPos.y - (PORTAL_COLLISION_SIZE.y / 2.0f),
			startPos.z
		));

		SweepResultGround callback;
		callback.me = &m_collisionObject->GetbtCollisionObject();
		callback.isResultGround = true;
		callback.startPos = startPos;

		PhysicsWorld::GetInstance()->ConvexSweepTest((const btConvexShape*)m_collisionObject->GetGhostObject().GetBody(), start, end, callback);

		//衝突した。
		if (callback.isHit) {
			finalPos.y = callback.hitPos.y + (PORTAL_COLLISION_SIZE.y / 2.0f);
			isHit = callback.isHit;
		}
	}

	if (isHit) {
		m_position = finalPos;
	}

	return isHit;
}

void PortalFrame::Reset()
{
	m_position = Vector3(0.0f, -1000.0f, 0.0f);
	m_collisionObject->SetPosition(m_position);
	m_collisionObject->SetIsEnable(false);
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.Update();
	m_physicsStaticObject.SetPosition(m_position);
	m_isEnable = false;

	if (m_frameEffect != nullptr) {
		m_frameEffect->Stop();
		DeleteGO(m_frameEffect);
	}
}


/// <summary>
/// ポータルフレームを設定。
/// </summary>
/// <param name="pos">座標</param>
/// <param name="normal">法線</param>
void PortalFrame::SetPortalFrame(const Vector3& pos, const Vector3& normal)
{
	//if (IsCanPut(pos, normal) == false) {
	//	return;
	//}
	m_position = pos;

	m_isEnable = true;
	m_normal = normal;

	//法線の方向に少し移動させる。
	m_position += m_normal * PORTAL_POS_NORMAL;

	//当たったオブジェクトの法線に合わせて回転する。
	Quaternion portalRot;
	portalRot.SetRotationYFromDirectionXZ(m_normal);
	portalRot.AddRotationDegX(-m_normal.y * PORTAL_ROT_X);
	m_rotation = portalRot;

	//法線がY軸なら、カメラの回転に応じてポータルも回転させる。
	if (m_normal.y < -0.99f) {
		m_rotation.AddRotationZ(Math::PI / 2.0f - fabsf(m_gameCamera->GetRotRadian().x));
	}
	else if (m_normal.y > 0.99f) {
		m_rotation.AddRotationZ(Math::PI / 2.0f - m_gameCamera->GetRotRadian().x);
	}

	//ポータルカメラを更新。
	m_portalCamera->UpdateAngle();

	//ポータルモデルを設定。
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.SetRotation(m_rotation);
	m_portalFrameModelRender.Update();

	//ポータルの当たり判定を設定。
	m_collisionObject->SetPosition(m_position);
	m_collisionObject->SetRotation(m_rotation);
	m_collisionObject->SetIsEnable(true);

	//物理静的オブジェクトを設定。
	m_physicsStaticObject.SetPosition(m_position);
	m_physicsStaticObject.SetRotation(m_rotation);

	if (m_frameEffect != nullptr) {
		m_frameEffect->Stop();
		DeleteGO(m_frameEffect);
	}

	//枠組みエフェクトを再生。
	m_frameEffect = NewGO<EffectEmitter>(0);
	m_frameEffect->Init(EffectEmitter::enEffect_PortalFrame_Blue);
	m_frameEffect->SetPosition(m_position);
	m_frameEffect->SetRotation(m_rotation);
	m_frameEffect->Play();
}

void PortalFrame::Render(RenderContext& rc)
{
	if (m_isEnable) {
		m_portalFrameModelRender.Draw(rc);
	}
}