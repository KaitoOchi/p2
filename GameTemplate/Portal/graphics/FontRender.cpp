#include "PortalEnginePreCompile.h"
#include "graphics/FontRender.h"

namespace nsPortalEngine {

	void FontRender::Draw(RenderContext& rc)
	{
		if (m_text != nullptr) {
			RenderingEngine::GetInstance()->AddRenderObject(this);
		}
	}

	void FontRender::On2DRender(RenderContext& rc)
	{
		m_font.Begin(rc);
		m_font.Draw(
			m_text,
			Vector2(m_position.x, m_position.y),
			m_color,
			m_rotation,
			m_scale,
			m_pivot);
		m_font.End(rc);
	}
}