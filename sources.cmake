target_sources(Vox PRIVATE
	"src/Vox.cpp"
	"src/core/logging/Logging.cpp"
	"src/core/math/Math.cpp"
	"src/core/services/InputService.cpp"
	"src/core/services/ServiceLocator.cpp"
	"src/editor/Editor.cpp"
	"src/physics/BroadPhaseLayer.cpp"
	"src/physics/CharacterController.cpp"
	"src/physics/ContactListener.cpp"
	"src/physics/ObjectBroadPhaseLayerFilter.cpp"
	"src/physics/ObjectPairLayerFilter.cpp"
	"src/physics/PhysicsServer.cpp"
	"src/physics/RayCastResultNormal.cpp"
	"src/physics/SpringArm.cpp"
	"src/physics/VoxelBody.cpp"
	"src/rendering/ArrayTexture.cpp"
	"src/rendering/Camera.cpp"
	"src/rendering/DebugRenderer.cpp"
	"src/rendering/Framebuffer.cpp"
	"src/rendering/FullscreenQuad.cpp"
	"src/rendering/GBuffer.cpp"
	"src/rendering/Light.cpp"
	"src/rendering/Renderer.cpp"
	"src/rendering/RenderTexture.cpp"
	"src/rendering/Texture.cpp"
	"src/rendering/mesh/Primitive.cpp"
	"src/rendering/mesh/MeshInstance.cpp"
	"src/rendering/mesh/MeshInstanceContainer.cpp"
	"src/rendering/mesh/Model.cpp"
	"src/rendering/mesh/VoxelMesh.cpp"
	"src/rendering/shaders/ComputeShader.cpp"
	"src/rendering/shaders/DeferredShader.cpp"
	"src/rendering/shaders/PixelShader.cpp"
	"src/rendering/shaders/Shader.cpp"
	"src/rendering/shaders/VoxelShader.cpp"
	"src/voxel/CollisionOctree.cpp"
	"src/voxel/Octree.cpp"
	"src/voxel/Vector.cpp"
	"src/voxel/Voxel.cpp"
	"src/voxel/VoxelChunk.cpp"
	"src/voxel/VoxelGrid.cpp"
	"src/voxel/VoxelWorld.cpp"
	"src/character/Character.cpp"
	"src/physics/TypeConversions.cpp"
	"src/core/datatypes/Transform.cpp" 
	"src/rendering/mesh/ModelNode.cpp" 
	"src/rendering/mesh/SkeletalModel.cpp"
	"src/rendering/mesh/AnimationSampler.cpp"
)