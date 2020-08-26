#ifndef slic3r_FillAdaptive_hpp_
#define slic3r_FillAdaptive_hpp_

#include "../AABBTreeIndirect.hpp"

#include "FillBase.hpp"

namespace Slic3r {

namespace FillAdaptive_Internal
{
    struct CubeProperties
    {
        double edge_length;     // Lenght of edge of a cube
        double height;          // Height of rotated cube (standing on the corner)
        double diagonal_length; // Length of diagonal of a cube a face
        double line_z_distance; // Defines maximal distance from a center of a cube on Z axis on which lines will be created
        double line_xy_distance;// Defines maximal distance from a center of a cube on X and Y axis on which lines will be created
    };

    struct Cube
    {
        Vec3d center;
        size_t depth;
        CubeProperties properties;
        std::vector<Cube*> children;
    };

    struct Octree
    {
        Cube *root_cube;
        Vec3d origin;
    };
}; // namespace FillAdaptive_Internal

//
// Some of the algorithms used by class FillAdaptive were inspired by
// Cura Engine's class SubDivCube
// https://github.com/Ultimaker/CuraEngine/blob/master/src/infill/SubDivCube.h
//
class FillAdaptive : public Fill
{
public:
    virtual ~FillAdaptive() {}

protected:
    virtual Fill* clone() const { return new FillAdaptive(*this); };
	virtual void _fill_surface_single(
	    const FillParams                &params, 
	    unsigned int                     thickness_layers,
	    const std::pair<float, Point>   &direction, 
	    ExPolygon                       &expolygon, 
	    Polylines                       &polylines_out);

	virtual bool no_sort() const { return true; }

    void generate_polylines(FillAdaptive_Internal::Cube *cube, double z_position, const Vec3d &origin, std::vector<Polylines> &polylines_out);

    void merge_polylines(Polylines &polylines, const Line &new_line);

public:
    static FillAdaptive_Internal::Octree* build_octree(
            TriangleMesh &triangleMesh,
            coordf_t line_spacing,
            const BoundingBoxf3 &printer_volume,
            const Vec3d &cube_center);

    static void expand_cube(
            FillAdaptive_Internal::Cube *cube,
            const std::vector<FillAdaptive_Internal::CubeProperties> &cubes_properties,
            const Transform3d &rotation_matrix,
            const AABBTreeIndirect::Tree3f &distanceTree,
            const TriangleMesh &triangleMesh);
};

} // namespace Slic3r

#endif // slic3r_FillAdaptive_hpp_