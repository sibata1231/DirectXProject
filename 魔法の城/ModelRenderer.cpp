#include "ModelRenderer.h"

void ModelRenderer::Start() {
    Renderer::Start();
    m_shadowType = ShadowTypes::NONE;
    m_rendererTag = RendererTags::MODEL;
    m_material.SetAmbient(M_AMBIENT.x, M_AMBIENT.y, M_AMBIENT.z, M_AMBIENT.w);
    m_material.SetDiffuse(M_DIFFUSE.x, M_DIFFUSE.y, M_DIFFUSE.z, M_DIFFUSE.w);
    m_material.SetSpecular(M_SPECULAR.x, M_SPECULAR.y, M_SPECULAR.z, M_SPECULAR.w);
    m_material.SetEmissive(M_EMISSIVE.x, M_EMISSIVE.y, M_EMISSIVE.z, M_EMISSIVE.w);
}