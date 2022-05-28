#pragma once

#include <vector>

struct FramebufferAttachment
{
	unsigned int id;
	int format;
	int internalFormat;
};

enum FramebufferAttachmentFormat
{
	COLOR_RGBA8,
	COLOR_RGB8,
	UNSIGNED_BYTE,
	BYTE,
	UNSIGNED_INT,
	INT,
	DEPTH
};

struct FramebufferSpecification
{
	int width, height;
	std::vector<FramebufferAttachmentFormat> fbAttachments;
};

class Framebuffer
{
public:
	Framebuffer();
	Framebuffer(const FramebufferSpecification& specification);
	~Framebuffer();

	void SetSpecification(const FramebufferSpecification& specification) { m_specification = specification; }
	void Create();

	void Bind() const;
	void UnBind() const;

	unsigned int GetId() const { return m_id; }
	unsigned int GetColorAttachment(unsigned int index) const;
	unsigned int GetDepthAttachment() const { return m_depthAttachment.id; }

	void Resize(int width, int height);

	template<typename T>
	void ClearColorAttachment(unsigned int colorAttachmentIndex, const void* data);

	template<typename T>
	void ReadPixels(unsigned int colorAttachmentIndex, int x, int y, int width, int height, void* pixels);

private:
	void AttachColorBuffer(int internalFormat, int format);
	void AttachDepthBuffer();

private:
	unsigned int m_id;
	FramebufferSpecification m_specification;
	std::vector<FramebufferAttachment> m_colorAttachments;
	FramebufferAttachment m_depthAttachment;
};