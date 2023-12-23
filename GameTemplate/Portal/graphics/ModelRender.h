#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// ���f�������_�[�B
	/// </summary>
	class ModelRender : public IRenderer
	{
	public:
		/// <summary>
		/// ���f���̏��������[�h�B
		/// </summary>
		enum ModelInitMode
		{
			enModelInitMode_DiferredLighting,		//�f�B�t�@�[�h���C�e�B���O�B
			enModelInitMode_ForwardLighting,		//�t�H���[�h�����_�����O�B
			enModelInitMode_UnDrawMainCamera,		//���C���J�����ɕ`�悳��Ȃ��B
			enModelInitMode_Outline,				//�֊s���B
			enModelInitMode_Dithering,				//�f�B�U�����O�B
		};

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
		/// <param name="enModelInitMode">���f���̏��������[�h</param>
		void Init(
			const char* filePath,
			AnimationClip* animationClip = nullptr,
			const int numAnimationClip = 0,
			const EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
			const bool isShadowCaster = false,
			const bool isShadowReceiver = false,
			const ModelInitMode enModelInitMode = enModelInitMode_DiferredLighting
		);

		/// <summary>
		/// ModelInitData���g�p���������������B
		/// </summary>
		/// <param name="modelInitData"></param>
		void InitModelInitData(ModelInitData& modelInitData);

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
			if (m_gBufferModel.IsInited()) {
				return m_gBufferModel;
			}
			else if (m_forwardRenderModel.IsInited()) {
				return m_forwardRenderModel;
			}
			return m_zprepassModel[0];
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
		/// <param name="enModelInitMode">���f���̏��������[�h</param>
		void InitModel(
			const char* filePath,
			EnModelUpAxis enModelUpAxis,
			const bool isShadowCaster,
			const bool isShadowReceiver,
			const ModelInitMode enModelInitMode = enModelInitMode_DiferredLighting
		);
		/// <summary>
		/// �V���h�E�p���f���̏������B
		/// </summary>
		/// <param name="tkmFilePath"></param>
		/// <param name="modelUpAxis"></param>
		void InitShadowModel(const char* tkmFilePath, const EnModelUpAxis modelUpAxis);
		/// <summary>
		/// ZPrepass�p���f���̏������B
		/// </summary>
		/// <param name="tkmFilePath"></param>
		/// <param name="modelUpAxis"></param>
		void InitZPrepassModel(const char* tkmFilePath, const EnModelUpAxis modelUpAxis);
		/// <summary>
		/// ���f���ɂ���X�P���g����ݒ�B
		/// </summary>
		/// <param name="modelInitData"></param>
		void SetModelHasSkeleton(ModelInitData& modelInitData);
		/// <summary>
		/// ���f���̍X�V�����B
		/// </summary>
		void UpdateWorldMatrix();

	private:
		/// <summary>
		/// �f�B�t�@�[�h���C�e�B���O�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void OnRenderGBuffer(RenderContext& rc) override;
		/// <summary>
		/// �t�H���[�h�����_�[�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void OnForwardRender(RenderContext& rc) override;
		/// <summary>
		/// �|�[�^���p�����_�[�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void OnPortalRender(RenderContext& rc, const int num, Camera& camera) override;
		/// <summary>
		/// �V���h�E�}�b�v�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		/// <param name="camera"></param>
		void OnRenderShadowMap(RenderContext& rc, Camera& camera) override;
		/// <summary>
		/// ZPrepass�̕`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void OnRenderToZPrepass(RenderContext& rc, const int num, Camera& camera) override;

	private:
		Skeleton							m_skeleton;					//�X�P���g���B
		Animation							m_animation;				//�A�j���[�V�����B
		Model								m_gBufferModel;				//GBuffer���f���B
		Model								m_forwardRenderModel;		//�t�H���[�h�����_�����O�p���f���B
		Model								m_shadowModel;				//�V���h�E���f���B
		std::array<Model, PORTAL_NUM>		m_portalModel;				//�|�[�^���z���̃��f���B
		std::array<Model, 1 + PORTAL_NUM>	m_zprepassModel;			//ZPrepass���f���B
		Vector3								m_position;					//���W�B
		Vector3								m_scale = Vector3::One;		//�g�嗦�B
		Quaternion							m_rotation;					//��]�B
		float								m_animationSpeed = 1.0f;	//�A�j���[�V�������x�B
	};

}
