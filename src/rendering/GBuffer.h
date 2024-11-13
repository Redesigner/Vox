class GBuffer
{
public:
    GBuffer(int width, int height);
    ~GBuffer();

private:

    unsigned int framebuffer;

    unsigned int positionTexture;
    unsigned int normalTexture;
    unsigned int albedoSpecTexture;

    unsigned int depthRenderbuffer;

    unsigned int width;
    unsigned int height;

    GBuffer (const GBuffer&) = delete;
    GBuffer& operator= (const GBuffer&) = delete;
};