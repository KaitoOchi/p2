#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// ���f�������_�[�B
	/// </summary>
	class ModelRender : public IRenderer
	{
	public:
		ModelRender();
		~ModelRender();

		/// <summary>
		/// �����������B
		/// </summary>
		/// <param name="filePath">�t�@�C���p�X</param>
		/// <param name="animationClip">�A�j���[�V�����N���b�v</param>
		/// <param name="numAnimationClip">�A�j���[�V�����N���b�v�̐�</param>
		/// <param name="enModelUpAxis">���f���̏����</param>
		/// <param name="isShadowCaster">true�Ȃ�e��^����</param>
		/// <param name="isShadowReceiver">true�Ȃ�e�𗎂Ƃ�</param>
		void Init(
			const char* filePath,
			AnimationClip* animationClip = nullptr,
			const int numAnimationClip = 0,
			EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
			const bool isShadowCaster = false,
			const bool isShadowReceiver = false
		);

		/// <summary>
		/// �X�V�����B
		/// </summary>
		void Update();

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void Draw(RenderContext& rc);

		/// <summary>
		/// �A�j���[�V�����̍Đ��B
		/// </summary>
		/// <param name="animNo">�A�j���[�V�����ԍ�</param>
		/// <param name="interpolateTime">���`��Ԃ̕b��</param>
		void PlayAnimation(int animNo, float interpolateTime = 0.0f)
		{
			m_animation.Play(animNo, interpolateTime);
		}

		/// <summary>
		/// �A�j���[�V�������Đ����Ă��邩�H
		/// </summary>
		/// <returns></returns>
		const bool IsPlayingAnimation() const
		{
			return m_animation.IsPlaying();
		}

		/// <summary>
		/// ���f���̎擾�B
		/// </summary>
		/// <returns></returns>
		Model& GetModel()
		{
			return m_model;
		}

		/// <summary>
		/// ���W��ݒ�B
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
		}

		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rot"></param>
		void SetRotation(const Quaternion& rot)
		{
			m_rotation = rot;
		}

		/// <summary>
		/// �g�嗦��ݒ�B
		/// </summary>
		/// <param name="scale"></param>
		void SetScale(const Vector3& scale)
		{
			m_scale = scale;
		}

		/// <summary>
		/// �A�j���[�V�����C�x���g�̒ǉ��B
		/// </summary>
		/// <param name="eventListener"></param>
		void AddAnimationEvent(AnimationEventListener eventListener)
		{
			m_animation.AddAnimationEventListener(eventListener);
		}

		/// <summary>
		/// �A�j���[�V�������x��ݒ�B
		/// </summary>
		/// <param name="animationSpeed"></param>
		void SetAnimationSpeed(const float animationSpeed)
		{
			m_animationSpeed = animationSpeed;
		}

		/// <summary>
		/// �{�[��ID�̎擾�B
		/// </summary>
		/// <param name="boneName"></param>
		/// <returns></returns>
		int FindBoneID(const wchar_t* boneName)
		{
			return m_skeleton.FindBoneID(boneName);
		}

		/// <summary>
		/// �{�[���̎擾�B
		/// </summary>
		/// <param name="boneNo"></param>
		/// <returns></returns>
		Bone* GetBone(int boneNo)
		{
			return m_skeleton.GetBone(boneNo);
		}

	private:
		/// <summary>
		/// �X�P���g���̏������B
		/// </summary>
		/// <param name="filePath"></param>
		void InitSkeleton(const char* filePath);

		/// <summary>
		/// �A�j���[�V�����̏������B
		/// </summary>
		/// <param name="animationClips"></param>
		void InitAnimation(AnimationClip* animationClips, const int numAnimationClips);

		/// <summary>
		/// ���f���̏������B
		/// </summary>
		/// <param name="filePath"></param>
		/// <param name="enModelUpAxis"></param>
		/// <param name="isShadowCaster"></param>
		/// <param name="isShadowReceiver"></param>
		void InitModel(
			const char* filePath,
			EnModelUpAxis enModelUpAxis,
			const bool isShadowCaster,
			const bool isShadowReceiver
		);

		/// <summary>
		/// ���f���̍X�V�����B
		/// </summary>
		void UpdateWorldMatrix();

	private:
		/// <summary>
		/// �t�H���[�h�����_�[�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void OnForwardRender(RenderContext& rc) override;

	private:
		Skeleton	m_skeleton;					//�X�P���g���B
		Animation	m_animation;				//�A�j���[�V�����B
		Model		m_model;					//���f���B
		Vector3		m_position;					//���W�B
		Vector3		m_scale = Vector3::One;		//�g�嗦�B
		Quaternion	m_rotation;					//��]�B
		float		m_animationSpeed = 0.0f;	//�A�j���[�V�������x�B
	};

}