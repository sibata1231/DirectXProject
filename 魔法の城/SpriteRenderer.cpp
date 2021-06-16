#include "SpriteRenderer.h"

void SpriteRenderer::Start() {
    Renderer::Start();
    m_rendererTag = RendererTags::SPRITE;
}