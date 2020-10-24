#include <glm/vec3.hpp>
#include <glm/vec2.hpp>

namespace SekhmetEngine
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TextureCoordinates;
	};
}