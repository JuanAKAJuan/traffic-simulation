#include <string>

using namespace std;

/// \struct Material
/// Structure for storing material information read from an OBJ MTL file.

struct Material {
	float Ka[3];  ///< Ambient reflection coefficients
	float Kd[3];  ///< Diffusive refletion coefficients
	float Ks[3];  ///< Specular reflection coefficients
	float Ns;   ///< Specular exponent for the current materials.  This defines
			    /// the focus of the specular highlight.

	/// Specify the dissolve for the current model. "factor" is the amount this material dissolves
	/// into the background. A factor of 1.0 is fully opaque.This is the default when a new material
	///	is created. A factor of 0.0 is fully dissolved(completely transparent).
	float d; 

	/// Illumination model used for the material
	/// 0 Color on and Ambient off
	/// 1 Color on and Ambient on
	///	2 Highlight on
	///	3 Reflection on and Ray trace on
	///	4 Transparency: Glass on
	///	Reflection : Ray trace on
	///	5 Reflection : Fresnel on and Ray trace on
	///	6 Transparency : Refraction on
	///	Reflection : Fresnel off and Ray trace on
	///	7 Transparency : Refraction on
	///	Reflection : Fresnel on and Ray trace on
	///	8 Reflection on and Ray trace off
	///	9 Transparency : Glass on
	///	Reflection : Ray trace off
	///	10 Casts shadows onto invisible surfaces
	int illum;

	string textureFileName;   ///< Name of the file that stores the texture.
};