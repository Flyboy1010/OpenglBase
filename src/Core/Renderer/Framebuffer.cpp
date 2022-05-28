#include "Core/Renderer/Framebuffer.h"
#include <GL/glew.h>
#include <cassert>

/* FRAMEBUFFER */

Framebuffer::Framebuffer()
{
    m_id = 0;
    m_depthAttachment = { 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT };
}

Framebuffer::Framebuffer(const FramebufferSpecification& specification)
{
    m_id = 0;
    m_depthAttachment = { 0, GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT };
    m_specification = specification;

	Create();
}

Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &m_id);

    for (auto& colorAttachment : m_colorAttachments)
        glDeleteTextures(1, &colorAttachment.id);

	glDeleteTextures(1, &m_depthAttachment.id);
}

void Framebuffer::Bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);
}

void Framebuffer::UnBind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned int Framebuffer::GetColorAttachment(unsigned int index) const
{
	if (index >= m_colorAttachments.size())
		return 0;

	return m_colorAttachments[index].id;
}

void Framebuffer::AttachColorBuffer(int internalFormat, int format)
{
    unsigned int colorAttachmentId;
    int colorAttachmentIndex = m_colorAttachments.size();

    glGenTextures(1, &colorAttachmentId);
    glBindTexture(GL_TEXTURE_2D, colorAttachmentId);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_specification.width, m_specification.height, 0, format, GL_UNSIGNED_BYTE, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachmentIndex, GL_TEXTURE_2D, colorAttachmentId, 0);

    m_colorAttachments.push_back({ colorAttachmentId, format, internalFormat });
}

void Framebuffer::AttachDepthBuffer()
{
    glGenTextures(1, &m_depthAttachment.id);
    glBindTexture(GL_TEXTURE_2D, m_depthAttachment.id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_specification.width, m_specification.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_depthAttachment.id, 0);
}

void Framebuffer::Create()
{
    // create framebuffer

    glGenFramebuffers(1, &m_id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_id);

    // add attachments

    for (int i = 0; i < m_specification.fbAttachments.size(); i++)
    {
        const auto& fbAttachment = m_specification.fbAttachments[i];

        switch (fbAttachment)
        {
        case FramebufferAttachmentFormat::COLOR_RGBA8:
            AttachColorBuffer(GL_RGBA8, GL_RGBA);
            break;
        case FramebufferAttachmentFormat::COLOR_RGB8:
            AttachColorBuffer(GL_RGB8, GL_RGB);
            break;
        case FramebufferAttachmentFormat::UNSIGNED_BYTE:
            AttachColorBuffer(GL_R8UI, GL_RED_INTEGER);
            break;
        case FramebufferAttachmentFormat::BYTE:
            AttachColorBuffer(GL_R8I, GL_RED_INTEGER);
            break;
        case FramebufferAttachmentFormat::UNSIGNED_INT:
            AttachColorBuffer(GL_R32UI, GL_RED_INTEGER);
            break;
        case FramebufferAttachmentFormat::INT:
            AttachColorBuffer(GL_R32I, GL_RED_INTEGER);
            break;
        case FramebufferAttachmentFormat::DEPTH:
            AttachDepthBuffer();
            break;
        }
    }

    // check if there is at least one color attachment

    if (m_colorAttachments.size() > 0)
    {
        // buffers to draw

        GLenum buffers[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
        glDrawBuffers(m_colorAttachments.size(), buffers);
    }
    else
    {
        // if there are not any color buffer attached (example just a depth buffer) then we need to explicitly tell OpenGL we're not going to render any color data

        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }

    // check if the framebuffer is succesfully created

    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);

    // unbind the framebuffer
 
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Resize(int width, int height)
{
    // delete the framebuffer and color and depth attachments

    glDeleteFramebuffers(1, &m_id);

    for (auto& colorAttachment : m_colorAttachments)
        glDeleteTextures(1, &colorAttachment.id);

    glDeleteTextures(1, &m_depthAttachment.id);

    m_colorAttachments.clear();

    // recreate with new dimensions

    m_specification.width = width;
    m_specification.height = height;

    Create();
}

template<>
void Framebuffer::ClearColorAttachment<unsigned char>(unsigned int colorAttachmentIndex, const void* data)
{
    const auto& colorAttachment = m_colorAttachments[colorAttachmentIndex];
    glClearTexImage(colorAttachment.id, 0, colorAttachment.format, GL_UNSIGNED_BYTE, data);
}

template<>
void Framebuffer::ClearColorAttachment<int>(unsigned int colorAttachmentIndex, const void* data)
{
    const auto& colorAttachment = m_colorAttachments[colorAttachmentIndex];
    glClearTexImage(colorAttachment.id, 0, colorAttachment.format, GL_INT, data);
}

template<>
void Framebuffer::ReadPixels<unsigned char>(unsigned int colorAttachmentIndex, int x, int y, int width, int height, void* pixels)
{
    // get the color attachment

    const auto& colorAttachment = m_colorAttachments[colorAttachmentIndex];

    // set the color attachment to read from

    glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);

    // read the pixels

    glReadPixels(x, y, width, height, colorAttachment.format, GL_UNSIGNED_BYTE, pixels);
}

template<>
void Framebuffer::ReadPixels<int>(unsigned int colorAttachmentIndex, int x, int y, int width, int height, void* pixels)
{
    // get the color attachment
    
    const auto& colorAttachment = m_colorAttachments[colorAttachmentIndex];

    // set the color attachment to read from

    glReadBuffer(GL_COLOR_ATTACHMENT0 + colorAttachmentIndex);

    // read the pixels

    glReadPixels(x, y, width, height, colorAttachment.format, GL_INT, pixels);
}