#include "MeshRenderer.h"

void MeshRenderer::Start() {
    Renderer::Start();
    m_rendererTag = RendererTags::MESH;
}