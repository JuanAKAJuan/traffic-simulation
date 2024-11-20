
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <gl/glut.h>
#include "Material.h"
#include "Vertex.h"
#include "Normal.h"
#include "Face.h"
#include "Texture.h"
#include "BoundingBox.h"
#include "Vector3D.h"

using namespace std;

/// \enum DisplayMode
/// \brief Enumeration for different display modes.
///
enum DisplayMode { Point,
    WireFrame,
    Filled };

/// \class ObjModel
/// \brief A class for parsing and drawing OBJ 3D models.
///
class ObjModel {
public:
    ObjModel(void);
    ~ObjModel(void);

    /// Read the OBJ and MTL files.
    /// \param fileName Name of the OBJ file
    virtual void ReadFile(string fileName);

    void Draw(); ///< Draw the mesh by drawing opaque and translucent materials
                 ///< separately.

    /// Get all the vertices of the mesh
    /// \return An STL vector that contains all the vertices of the mesh.
    vector<Vertex> GetVertices() { return vertices; }

    /// Get all the normals of the mesh
    /// \return An STL vector that contains all the normals of the mesh.
    vector<Normal> GetNormals() { return normals; }

    /// Get all the faces of the mesh
    /// \return An STL vector that contains all the faces of the mesh.
    vector<Face> GetFaces() { return faces; }

    /// Get all the material names of the mesh
    /// \return An STL vector that contains all the material names of the mesh.
    vector<string> GetMaterialNames() { return mtlNames; }

    /// Get the AABB bounding box of the mesh.
    BoundingBox GetBoundingBox() { return bb; }

    /// Set display mode
    /// \param displayMode Displaymode with values of Point, WireFrame, or Filled
    void SetDisplayMode(DisplayMode displayMode);

    /// Reset the program.
    /// Clear all the data structures for the mesh.  Initialize the bounding box
    void Reset();

    /// Get the center of the boundig box.
    /// \return Center of the bounding box.
    Vertex GetCenter();

    /// Get the size of the bounding box.
    /// \return Size of the bounding box.
    Vector3 GetSize();

protected:
    vector<Vertex> vertices; ///< All vertices of the mesh.
    vector<Normal> normals; ///< All normals of the mesh.
    vector<TextureCoord> textureCoords; ///< All texture coordinates of the mesh.
    vector<string> mtlNames; ///< All material names in the MTL file.
    map<string, Material> materials; ///< A map that maps material names to the
                                     ///< material structures (values).
    map<string, vector<int>>
        facesInMaterials; ///< A map that contains the face indices for each named
                          ///< material (key).
    map<string, int> textures; ///< A map that contains the texture object number
                               ///< for each named material.
    vector<Face> faces; ///< All faces in the mesh.
    BoundingBox bb; ///< Bounding box
    float xmin; ///< Minimum x of AABB
    float xmax; ///< Maximum x of AABB
    float ymin; ///< Minimum y of AABB
    float ymax; ///< Maximum y of AABB
    float zmin; ///< Minimum z of AABB
    float zmax; ///< Maximum z of AABB
    DisplayMode displayMode; ///< Current display mode.
    vector<string>
        opaqueMaterials; ///< Contain the names of all opaque materials.
    vector<string>
        translucentMaterials; ///< Contain the names of all translucent materials.

    /// A function to draw different materials.
    /// @param mtlName is the name of the material.
    /// @param opaque whether the material is opaque or not
    void DrawMaterials(string mtlName, bool opaque);

    string path; ///< Path of the MTL file.
};
