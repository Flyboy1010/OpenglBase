#pragma once

#include <glm/glm.hpp>
#include "Texture.h"
#include "Shader.h"
#include "Buffer.h"
#include "VertexArray.h"

class Renderer
{
public:
	static void Init();
	static void Destroy();
	
	static void SetClearColor(const glm::vec4& color);
	static void Clear();
	static void SetViewport(int x, int y, int width, int height);
	static void SetViewportAspectRatio(int windowWidth, int windowHeight, float targetAspectRatio);
	static void SetLineWidth(float width);
	
	static void StartBatch();
	static void Flush();
	
	static void DrawTexture(const Texture* texture, const glm::vec2& position, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	static void DrawTexture(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	static void DrawTexture(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec2& srcPosition, const glm::vec2& srcSize, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	static void DrawTexture(const Texture* texture, const glm::vec2& position, float radians, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	static void DrawTexture(const Texture* texture, const glm::vec2& position, const glm::vec2& size, float radians, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	static void DrawTexture(const Texture* texture, const glm::vec2& position, const glm::vec2& size, const glm::vec2& srcPposition, const glm::vec2& srcSize, float radians, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	
	static void DrawLine(const glm::vec2& p1, const glm::vec2& p2, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });
	static void DrawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

private:
	Renderer() {}
	~Renderer() {}
};
