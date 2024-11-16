#include <vector>

using namespace std;

/// \struct Face
/// Define indices of vertices, normals, and textures for a face.

struct Face {
	vector<int> vertices;  ///< Vertex indices
	vector<int> normals;   ///< Normal indices
	vector<int> textures;  ///< Texture coordinate indices
};