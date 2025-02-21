#include"UI/Panels/FrameBuffer.h"
#include<glad/glad.h>
UI::Panels::Framebuffer::Framebuffer(uint16_t p_width, uint16_t p_height)
{
	/* Generate OpenGL objects */
	glGenFramebuffers(1, &m_bufferID);
	glGenTextures(1, &m_renderTexture);
	glGenRenderbuffers(1, &m_depthStencilBuffer);

	/* Setup texture */
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	/* Setup framebuffer */
	Bind();
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_renderTexture, 0);
	Unbind();

	Resize(p_width, p_height);
}

UI::Panels::Framebuffer::~Framebuffer()
{
	/* Destroy OpenGL objects */
	glDeleteBuffers(1, &m_bufferID);
	glDeleteTextures(1, &m_renderTexture);
	glDeleteRenderbuffers(1, &m_depthStencilBuffer);
}

void UI::Panels::Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_bufferID);
}

void UI::Panels::Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void UI::Panels::Framebuffer::Resize(uint16_t p_width, uint16_t p_height)
{
	width = p_width;
	height = p_height;

	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, p_width, p_height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindRenderbuffer(GL_RENDERBUFFER, m_depthStencilBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_STENCIL, p_width, p_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_depthStencilBuffer);
	Unbind();
}

uint32_t UI::Panels::Framebuffer::GetID()
{
	return m_bufferID;
}

uint32_t UI::Panels::Framebuffer::GetTextureID()
{
	return m_renderTexture;
}

uint32_t UI::Panels::Framebuffer::GetRenderBufferID()
{
	return m_depthStencilBuffer;
}
