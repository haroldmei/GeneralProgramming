Usage: 
1: ClosestPointQuery.exe -c <number>: this will create a mesh randomly with <number> polygons; all the polygon points are bounded inside (0, 100.0). And then enter interaction;
2: ClosestPointQuery.exe -f <plyFile>: this will create a mesh from a real ply file, And then enter interaction;
!!!!Limitation:!!!!
The program can only calculate the closest point when the view point is outside the mesh area. For this reason we can test the correctness of the program by generating several hundreds random polygons and query a point that is outside (0, 100) area.
The testing .ply mesh files can be downloaded from http://www.kscan3d.com/gallery/

Optimizations needed:
0, The program is not reenter-able. To be more specific, BSPTree is not reenter-able and need to be fixed;
1, The one by one memory allocation is to be replaced by a bulk memory allocation;
2, The recursive call is to be elliminated to avoid the break of function stack;
3, More study needed to reduce the amount of intersections and the height of the BSP tree;
4, In CG rendering, there are some algorithms used to avoid overlapping drawing, like Viewing frustum culling, Occlusion culling, etc, could be used to remove hidden polygons and reduce the calculation of distance.

//+++++++++++++++++++++++++++++++ BSP Tree ++++++++++++++++++++++++++++++++++++++++++
Descriptions of important functions:
BSPTree::_BuildBSPTree
Recursively build a BSP tree for the input BSP node list.

BSPNode::SearchClosestNode
Similar to binary search with the view point as search key. It checks the relative position of the view point and the
current polygon. If the target is at the back of current polygon, search backward; or else search foreward,
until the search reaches a leaf. Store all the nodes alongside the search path, the closest point is in one 
of the node.

BSPNode::Traverse
Traverse the BSP tree, not used in this program. It is used for CG rendering.

//++++++++++++++++++++++++++++++++ GEOMETRY +++++++++++++++++++++++++++++++++++++++++
Geometry is described in: www.geometrictools.com/Documentation/DistancePoint3Triangle3.pdf
The following functions are all describes in the pdf:

BSPNode::ClosestPoint && BSPNode::closesPointOnTriangle
For a given point p, calculate the closest point of the current plane to p. This will be done 
in two steps:
1, split the current plane to one or more triangles;
2, call BSPNode::closesPointOnTriangle to calculate the closest point on those triangles, return the shortest amongest them

BSPNode::Split() and BSPNode::_SplitPoly
Split the current polygon with the given plane; this is necessary to partition the space.

CPoint::DotProduct
Calculate the dot product of two vectors; the dot product is a Scalar, it is the product of the two vectors' magnitude, and 
their angle's cosine value; 

CPoint::CrossProduct
Cross product of a plane, used for a plane's direction;

CPoint::Normalize && CPoint::Magnitude
...

