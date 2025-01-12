#pragma once

namespace Vox
{
	class FullscreenQuad
	{
	public:
		FullscreenQuad();
		~FullscreenQuad();

		unsigned int GetVaoId() const;

	private:
		unsigned int vaoId, vboPosition, vboTextureCoord;
	};
}