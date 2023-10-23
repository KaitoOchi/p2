#include "stdafx.h"
#include "PortalFrame.h"
#include "Player.h"

namespace
{
	const Vector3	PORTAL_COLLISION_SIZE = Vector3(60.0f, 106.0f, 1.0f);		//コリジョンサイズ。
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
}

bool PortalFrame::Start()
{
	//プレイヤーオブジェクトを検索。
	m_player = FindGO<Player>("player");

	//当たり判定を初期化。
	m_collisionObject = NewGO<CollisionObject>(0, "collision");
	m_collisionObject->CreateBox(m_position, m_rotation, PORTAL_COLLISION_SIZE);
	m_collisionObject->SetName("portal_red");

	//ポータルフレームを初期化。
	//ModelInitDataを使用して初期化する。
	ModelInitData modelInitData;
	modelInitData.m_tkmFilePath = "Assets/modelData/portal/portalFrame.tkm";
	modelInitData.m_fxFilePath = "Assets/shader/preProcess/RenderToGBuffer.fx";
	modelInitData.m_psEntryPointFunc = "PSMainPortalFrame";
	//テクスチャをポータル用レンダーターゲットに設定する。
	modelInitData.m_expandShaderResoruceView[0] = &RenderingEngine::GetInstance()->GetPortalRenderTarget(static_cast<int>(m_portalType)).GetRenderTargetTexture();
	modelInitData.m_expandShaderResoruceView[1] = &RenderingEngine::GetInstance()->GetZPrepassRenderTarget().GetRenderTargetTexture();
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

void PortalFrame::Collision()
{
	//当たり判定が無効なら。
	if (!m_collisionObject->IsEnable()) {
		return;
	}

	//ポータルとプレイヤーのベクトルが範囲外なら、衝突判定を行わない。
	Vector3 length = m_player->GetPosition() - m_position;
	if (length.LengthSq() > PORTAL_LENGTH * PORTAL_LENGTH) {
		return;
	}

	//プレイヤーと接触しているなら。
	if (m_collisionObject->IsHit(const_cast<CharacterController&>(m_player->GetCharacterController()))) {
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

		//プレイヤーの座標がポータルの座標を超えていたら。
		if (compareLargePos > compareSmallPos) {
			IsHit();
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

void PortalFrame::IsHit()
{
	Quaternion rot = m_anotherPortalFrame->GetRotation();
	//もう一方のポータルの座標まで移動する。
	m_player->SetWarp(m_anotherPortalFrame->GetPosition(), rot);

	m_isCollisionHit = false;
}

void PortalFrame::SetPortalFrame(const Vector3& pos, const Vector3& normal)
{
	m_isEnable = true;
	m_position = pos;
	m_normal = normal;

	//法線の方向に少し移動させる。
	m_position += m_normal * PORTAL_POS_NORMAL;

	//当たったオブジェクトの法線に合わせて回転する。
	Quaternion portalRot;
	portalRot.SetRotationYFromDirectionXZ(m_normal);
	portalRot.AddRotationDegX(m_normal.y * PORTAL_ROT_X);
	m_rotation = portalRot;

	//ポータルモデルを設定。
	m_portalFrameModelRender.SetPosition(m_position);
	m_portalFrameModelRender.SetRotation(m_rotation);
	m_portalFrameModelRender.Update();

	//ポータルの当たり判定を設定。
	m_collisionObject->SetPosition(m_position);
	m_collisionObject->SetRotation(m_rotation);

	//物理静的オブジェクトを設定。
	m_physicsStaticObject.SetPosition(m_position);
	m_physicsStaticObject.SetRotation(m_rotation);
}

void PortalFrame::Render(RenderContext& rc)
{
	if (m_isEnable) {
		m_portalFrameModelRender.Draw(rc);
	}
}