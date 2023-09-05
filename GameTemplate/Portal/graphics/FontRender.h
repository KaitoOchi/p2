#pragma once

namespace nsPortalEngine {

	class FontRender : public IRenderer
	{
	public:
		static const int MAX_TEXT_SIZE = 256;	//�����̍ő吔�B

		FontRender() {};
		~FontRender() {};

		/// <summary>
		/// �e�L�X�g��ݒ�B
		/// </summary>
		/// <param name="text"></param>
		void SetText(const wchar_t* text)
		{
			swprintf_s(m_text, text);
		}

		/// <summary>
		/// �e�L�X�g�̎擾�B
		/// </summary>
		/// <returns></returns>
		const wchar_t* GetText() const
		{
			return m_text;
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
		/// ���W�̎擾�B
		/// </summary>
		/// <returns></returns>
		const Vector3& GetPosition() const
		{
			return m_position;
		}

		/// <summary>
		/// ��]��ݒ�B
		/// </summary>
		/// <param name="rot"></param>
		void SetRotation(const float rot)
		{
			m_rotation = rot;
		}

		/// <summary>
		/// ��]�̎擾�B
		/// </summary>
		/// <returns></returns>
		const float GetRotation() const
		{
			return m_rotation;
		}

		/// <summary>
		/// �g�嗦��ݒ�B
		/// </summary>
		/// <param name="scale"></param>
		void SetScale(const float scale)
		{
			m_scale = scale;
		}

		/// <summary>
		/// �g�嗦�̎擾�B
		/// </summary>
		/// <returns></returns>
		const float GetScale() const
		{
			return m_scale;
		}

		/// <summary>
		/// �F��ݒ�B
		/// </summary>
		/// <param name="color"></param>
		void SetColor(const Vector4& color)
		{
			m_color = color;
		}

		/// <summary>
		/// �F�̎擾�B
		/// </summary>
		/// <returns></returns>
		const Vector4& GetColor() const
		{
			return m_color;
		}

		/// <summary>
		/// �s�{�b�g�̐ݒ�B
		/// </summary>
		/// <param name="pivot"></param>
		void SetPivot(const Vector2& pivot)
		{
			m_pivot = pivot;
		}

		/// <summary>
		/// �s�{�b�g�̎擾�B
		/// </summary>
		/// <returns></returns>
		const Vector2& GetPivot() const
		{
			return m_pivot;
		}

		/// <summary>
		/// �e�̃p�����[�^��ݒ�B
		/// </summary>
		/// <param name="isDrawShadow">�e��`�悷�邩�ǂ���</param>
		/// <param name="shadowOffset">�s�N�Z���̃I�t�Z�b�g��</param>
		/// <param name="shadowColor">�e�̐F</param>
		void SetShadowParam(
			const bool isDrawShadow,
			const float shadowOffset,
			const Vector4& shadowColor)
		{
			m_font.SetShadowParam(isDrawShadow, shadowOffset, shadowColor);
		}

		/// <summary>
		/// �`�揈���B
		/// </summary>
		/// <param name="rc"></param>
		void Draw(RenderContext& rc);

	private:
		/// <summary>
		/// 2D�`��p�X����Ă΂�鏈���B
		/// </summary>
		/// <param name="rc"></param>
		void On2DRender(RenderContext& rc) override;

	private:
		Font	m_font;								//�t�H���g�B
		Vector2 m_pivot = Sprite::DEFAULT_PIVOT;	//�s�{�b�g�B
		Vector3 m_position;							//���W�B
		Vector4 m_color = g_vec4White;				//�F�B
		wchar_t m_text[MAX_TEXT_SIZE];				//�����B
		float	m_rotation = 0.0f;					//�g�嗦�B
		float	m_scale = 1.0f;						//��]�B
	};

}
