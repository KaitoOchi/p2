#pragma once

namespace nsPortalEngine {

	/// <summary>
	/// モデルレンダー。
	/// </summary>
	class ModelRender : public IRenderer
	{
	public:
		ModelRender();
		~ModelRender();

		/// <summary>
		/// 初期化処理。
		/// </summary>
		/// <param name="filePath">ファイルパス</param>
		/// <param name="animationClip">アニメーションクリップ</param>
		/// <param name="numAnimationClip">アニメーションクリップの数</param>
		/// <param name="enModelUpAxis">モデルの上方向</param>
		/// <param name="isShadowCaster">trueなら影を与える</param>
		/// <param name="isShadowReceiver">trueなら影を落とす</param>
		void Init(
			const char* filePath,
			AnimationClip* animationClip = nullptr,
			const int numAnimationClip = 0,
			EnModelUpAxis enModelUpAxis = enModelUpAxisZ,
			const bool isShadowCaster = false,
			const bool isShadowReceiver = false
		);

		/// <summary>
		/// 更新処理。
		/// </summary>
		void Update();

		/// <summary>
		/// 描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void Draw(RenderContext& rc);

		/// <summary>
		/// アニメーションの再生。
		/// </summary>
		/// <param name="animNo">アニメーション番号</param>
		/// <param name="interpolateTime">線形補間の秒数</param>
		void PlayAnimation(int animNo, float interpolateTime = 0.0f)
		{
			m_animation.Play(animNo, interpolateTime);
		}

		/// <summary>
		/// アニメーションを再生しているか？
		/// </summary>
		/// <returns></returns>
		const bool IsPlayingAnimation() const
		{
			return m_animation.IsPlaying();
		}

		/// <summary>
		/// モデルの取得。
		/// </summary>
		/// <returns></returns>
		Model& GetModel()
		{
			return m_model;
		}

		/// <summary>
		/// 座標を設定。
		/// </summary>
		/// <param name="pos"></param>
		void SetPosition(const Vector3& pos)
		{
			m_position = pos;
		}

		/// <summary>
		/// 回転を設定。
		/// </summary>
		/// <param name="rot"></param>
		void SetRotation(const Quaternion& rot)
		{
			m_rotation = rot;
		}

		/// <summary>
		/// 拡大率を設定。
		/// </summary>
		/// <param name="scale"></param>
		void SetScale(const Vector3& scale)
		{
			m_scale = scale;
		}

		/// <summary>
		/// アニメーションイベントの追加。
		/// </summary>
		/// <param name="eventListener"></param>
		void AddAnimationEvent(AnimationEventListener eventListener)
		{
			m_animation.AddAnimationEventListener(eventListener);
		}

		/// <summary>
		/// アニメーション速度を設定。
		/// </summary>
		/// <param name="animationSpeed"></param>
		void SetAnimationSpeed(const float animationSpeed)
		{
			m_animationSpeed = animationSpeed;
		}

		/// <summary>
		/// ボーンIDの取得。
		/// </summary>
		/// <param name="boneName"></param>
		/// <returns></returns>
		int FindBoneID(const wchar_t* boneName)
		{
			return m_skeleton.FindBoneID(boneName);
		}

		/// <summary>
		/// ボーンの取得。
		/// </summary>
		/// <param name="boneNo"></param>
		/// <returns></returns>
		Bone* GetBone(int boneNo)
		{
			return m_skeleton.GetBone(boneNo);
		}

	private:
		/// <summary>
		/// スケルトンの初期化。
		/// </summary>
		/// <param name="filePath"></param>
		void InitSkeleton(const char* filePath);

		/// <summary>
		/// アニメーションの初期化。
		/// </summary>
		/// <param name="animationClips"></param>
		void InitAnimation(AnimationClip* animationClips, const int numAnimationClips);

		/// <summary>
		/// モデルの初期化。
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
		/// モデルの更新処理。
		/// </summary>
		void UpdateWorldMatrix();

	private:
		/// <summary>
		/// フォワードレンダーの描画処理。
		/// </summary>
		/// <param name="rc"></param>
		void OnForwardRender(RenderContext& rc) override;

	private:
		Skeleton	m_skeleton;					//スケルトン。
		Animation	m_animation;				//アニメーション。
		Model		m_model;					//モデル。
		Vector3		m_position;					//座標。
		Vector3		m_scale = Vector3::One;		//拡大率。
		Quaternion	m_rotation;					//回転。
		float		m_animationSpeed = 0.0f;	//アニメーション速度。
	};

}
