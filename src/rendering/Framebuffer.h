#pragma once

class Framebuffer
{
public:
	Framebuffer(int width, int height);
	~Framebuffer();

	void EnableFramebuffer();
	void BindDraw();
	void BindRead();
	
	void ActivateTextures() const;

private:
	int width, height;
	unsigned int framebuffer, colorTexture, depthRenderbuffer;
};