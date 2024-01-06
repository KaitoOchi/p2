#include "stdafx.h"
#include "PortalGun.h"
#include "PortalCamera.h"
#include "Game.h"

namespace
{
	const Vector3	PORTALGUN_POS = Vector3(10.0f, 0.0f, 20.0f);	//�|�[�^���K���̍��W�B
	const float		PORTALGUN_ROT_Y = 1.15f;						//�|�[�^���K���̏㉺�̉�]�ʁB
	const float		CURSOR_SIZE = 64.0f;							//�J�[�\���T�C�Y�B
	const float		RAY_LENGTH = 5000.0f;							//���C�̒����B
	const float		SHOT_EFFECT_SPEED = 200.0f;						//�G�t�F�N�g�̍Đ����x�B
	const float		SHOT_DURATION = 0.5f;							//���ˊԊu�B
	const float		SHAKE_SPEED = 8.0f;								//��ʗh��̑��x�B
	const float		SHAKE_POWER = 2.0f;								//��ʗh��̗ʁB
}


PortalGun::PortalGun()
{
	//�|�[�^���t���[�����������B
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = nullptr;
	}
}

PortalGun::~PortalGun()
{

}

bool PortalGun::Start()
{
	//�|�[�^���������B
	std::vector<PortalFrame*> portalFrames;
	portalFrames = FindGOs<PortalFrame>("portalFrame");
	//�|�[�^���J�����������B
	m_portalCamera = FindGO<PortalCamera>("portalCamera");
	//�Q�[���V�[���������B
	m_game = FindGO<Game>("game");

	m_shakeMoveSpeedTmp = m_position;
	
	//�|�C���^�Ɋi�[�B
	for (int i = 0; i < PORTAL_NUM; i++) {
		m_portalFrame[i] = portalFrames[i];
		m_portalCamera->SetPortalFramePointer(m_portalFrame[i], i);
	}

	//�|�[�^���K�����������B
	m_portalGunModelRender.Init("Assets/modelData/portal/portalGun.tkm", 0, 0, enModelUpAxisZ, true, true, ModelRender::enModelInitMode_ForwardLighting);
	m_portalGunModelRender.SetPosition(PORTALGUN_POS);
	m_portalGunModelRender.Update();

	//�N���X�w�A���������B
	m_cursorSpriteRender[0].Init("Assets/sprite/UI/cursor_blue.DDS", CURSOR_SIZE, CURSOR_SIZE);
	m_cursorSpriteRender[0].SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	m_cursorSpriteRender[0].Update();
	m_cursorSpriteRender[1].Init("Assets/sprite/UI/cursor_red.DDS", CURSOR_SIZE, CURSOR_SIZE);
	m_cursorSpriteRender[1].SetMulColor(Vector4(0.0f, 0.0f, 0.0f, 0.0f));
	m_cursorSpriteRender[1].Update();
	m_cursorSpriteRender[2].Init("Assets/sprite/UI/cursor.DDS", CURSOR_SIZE, CURSOR_SIZE);

	return true;
}

void PortalGun::Update()
{
	if (m_shotDuration <= 0.0f) {
		//�|�[�^����ݒu�B
		if (g_pad[0]->IsTrigger(enButtonLB1)) {
			SetPortal(PortalFrame::enPortalType_Blue);
		}
		//�ԃ|�[�^����ݒu�B
		else if (g_pad[0]->IsTrigger(enButtonRB1)) {
			SetPortal(PortalFrame::enPortalType_Red);
		}
	}
	else {
		m_shotDuration -= g_gameTime->GetFrameDeltaTime();
	}

	SetPortalGunModel();
}

/// <summary>
/// �|�[�^���̐ݒu�����B
/// </summary>
/// <param name="type">�|�[�^���^�C�v</param>
void PortalGun::SetPortal(const PortalFrame::PortalType type)
{
	//�J�����̑O�������擾�B
	Vector3 forward = g_camera3D->GetForward();

	//���������I�u�W�F�N�g�̏ڍׁB
	PhysicsWorld::RayHitObject hit;
	//���C�̎��_�ƏI�_��ݒ�B
	Vector3 rayStartPos = g_camera3D->GetPosition();
	Vector3 rayEndPos = rayStartPos + (forward * RAY_LENGTH);
	//���C���΂��B
	bool isHit = PhysicsWorld::GetInstance()->RayTest(rayStartPos, rayEndPos, hit);

	//������Ȃ������ꍇ�A�������Ȃ��B
	if (isHit == false) {
		return;
	}

	//���������I�u�W�F�N�g�̍��W�ɍ��킹�Đݒu����B
	Vector3 portalPos = hit.position;
	//�|�[�^����ݒu�B
	int num = static_cast<int>(type);
	m_portalFrame[num]->SetPortalFrame(portalPos, hit.normal);

	//�J�[�\����ݒ�B
	m_cursorSpriteRender[num].SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
	m_cursorSpriteRender[num].Update();

	Quaternion rot;
	rot.SetRotation(Vector3::AxisZ, g_camera3D->GetForward());

	//���˃G�t�F�N�g���Đ��B
	EffectEmitter* shotEffect = NewGO<EffectEmitter>(0);
	shotEffect->Init(EffectEmitter::enEffect_PortalShot_Blue);
	shotEffect->SetPosition(g_camera3D->GetPosition());
	shotEffect->SetRotation(rot);
	shotEffect->SetTime(SHOT_EFFECT_SPEED * g_gameTime->GetFrameDeltaTime());
	shotEffect->Play();

	//���ː������Z�B
	m_game->AddShotNum();

	//���ˊԊu��ݒ�B
	m_shotDuration = SHOT_DURATION;
}

void PortalGun::ResetPortal()
{
	for (int i = 0; i < PORTAL_NUM; i++) {
		//�|�[�^�������Z�b�g�B
		m_portalFrame[i]->Reset();
		//�J�[�\����ݒ�B
		m_cursorSpriteRender[i].SetMulColor(Vector4(1.0f, 1.0f, 1.0f, 0.0f));
		m_cursorSpriteRender[i].Update();
	}
}

void PortalGun::Reset()
{
	m_isIdle = false;
	m_shakeMoveSpeedTmp = Vector3::Zero;
	m_shakeTimer = 0.0f;
}

/// <summary>
/// ���f���̐ݒ菈���B
/// </summary>
void PortalGun::SetPortalGunModel()
{
	Vector3 forward = g_camera3D->GetForward();

	Vector3 pos = PORTALGUN_POS;
	pos.Normalize();

	float rotY = forward.y * 90.0f;

	Quaternion rot;
	rot.SetRotationYFromDirectionXZ(forward);
	rot.AddRotationDegX(-rotY);
	rot.Apply(pos);
	pos *= 30.0f;

	//���W��ݒ�B
	Vector3 portalGunPos = g_camera3D->GetPosition();
	portalGunPos += pos;
	portalGunPos.y -= 10.0f;

	//��]��ݒ�B
	Quaternion portalGunRot;
	portalGunRot.SetRotationYFromDirectionXZ(forward);
	portalGunRot.AddRotationDegX(-rotY);

	m_position = portalGunPos;

	Shake();

	//�|�[�^���K����ݒ�B
	m_portalGunModelRender.SetPosition(m_position);
	m_portalGunModelRender.SetRotation(portalGunRot);
	m_portalGunModelRender.Update();
}

void PortalGun::Shake()
{
	m_shakeTimer += g_gameTime->GetFrameDeltaTime() * SHAKE_SPEED;

	Vector2 input;
	input.x = g_pad[0]->GetLStickXF();
	input.y = g_pad[0]->GetLStickYF();
	float shakeValue = max(fabsf(input.x), fabsf(input.y));

	Vector3 shakeMoveSpeed;

	//�X�e�B�b�N�̓��͂�����Ȃ�B
	if (shakeValue) {
		if (m_isIdle) {
			m_shakeTimer = 0.0f;
			m_isIdle = false;
		}

		//�h���ǉ��B
		shakeMoveSpeed.y = sin(m_shakeTimer * shakeValue) * SHAKE_POWER;
		m_shakeMoveSpeedTmp = shakeMoveSpeed;
	}
	else {
		if (!m_isIdle) {
			m_shakeTimer = 0.0f;
			m_isIdle = true;
		}

		//1�b�𒴂��Ȃ��悤�ɂ���B
		m_shakeTimer = min(m_shakeTimer, 1.0f);

		//���`��ԂŌ��̈ʒu�ɖ߂��B
		shakeMoveSpeed.Lerp(m_shakeTimer, m_shakeMoveSpeedTmp + m_position, m_position);
		shakeMoveSpeed -= m_position;
	}

	//�h��̈ړ��ʂ����Z�B
	m_position += shakeMoveSpeed;
	m_portalGunModelRender.SetPosition(shakeMoveSpeed);
}

void PortalGun::Render(RenderContext& rc)
{
	//�|�[�^���K����`��B
	m_portalGunModelRender.Draw(rc);

	//�N���X�w�A�̕`��B
	m_cursorSpriteRender[2].Draw(rc);
	m_cursorSpriteRender[0].Draw(rc);
	m_cursorSpriteRender[1].Draw(rc);
}