//===========================================================================
//                                                                           
// File: BaryCoordSystem.h                                                 
//                                                                           
// Created: Thu May  3 13:52:01 2001                                         
//                                                                           
// Author: Atgeirr F Rasmussen <atgeirr@sintef.no>
//                                                                           
// Revision: $Id: BaryCoordSystem.h,v 1.15 2006-04-19 09:14:37 jbt Exp $
//                                                                           
//===========================================================================


#ifndef _BARYCOORDSYSTEM_H
#define _BARYCOORDSYSTEM_H

#include "GoTools/utils/Array.h"
#include "GoTools/utils/Volumes.h"
#include <algorithm>

namespace Go {

/** Encapsulates a barycentric coordinatesystem.
 *  It is designed to work in any dimension, but at the time of writing,
 *  only dimension 2 and 3 is supported by the functions in 
 *  the included header "Volumes.h"
 *
 *  You specify the dimension with a template parameter:
 *
 *      BaryCoordSystem<2> bc(corner_pointer);
 *
 *  ... or by using the supplied typedefs:
 *
 *      BaryCoordSystem2D bc(corner_pointer);
 *
 *  This file also contains the small function called
 *  triangle_area_signed().
 */


template <int Dim>
class BaryCoordSystem {
public:
    /// Empty default constructor.
    BaryCoordSystem()
    {}
    /// Constructor. Takes an array of points that will become
    /// the corners of the coordinate simplex.
    BaryCoordSystem(const Array<double, Dim>* corners)
    {
	std::copy(corners, corners+Dim+1, corners_);
	total_volume_ = simplex_volume(corners);
    }

    /// Set corners.
    void setCorners(const Array<double, Dim>* corners)
    {
	std::copy(corners, corners+Dim+1, corners_);
	total_volume_ = simplex_volume(corners);
    }

    /// Returns the corner points.
    const Array<double,Dim>& corner(int i) const
    {
	return corners_[i];
    }

    /// Input is a barycentric point, output is the corresponding
    /// cartesian point.
    template<class T>
    Array<T, Dim> baryToCart(const Array<T, Dim+1>& bary_pt) const;

    /// Input is a cartesian point, output is the corresponding
    /// barycentric point.
    template<class T>
    Array<T, Dim+1> cartToBary(const Array<T, Dim>& cart_pt) const;

    /// Read from input stream
    void read(std::istream& is)
    {
	bool is_good = is.good();
	if (!is_good) {
	    THROW("Invalid input file!");
	}
	for (int i = 0; i <= Dim; ++i)
	    is >> corners_[i];
	total_volume_ = simplex_volume(corners_);
		
    }
    /// Write to output stream
    void write(std::ostream& os) const
    {
	for (int i = 0; i < Dim; ++i)
	    os << corners_[i] << std::endl;
	os << corners_[Dim];
    }

private:
    Array<double, Dim> corners_[Dim+1];
    double total_volume_;
};


// Implementation of inline functions

template <int Dim>
template <class T>
inline Array<T, Dim>
BaryCoordSystem<Dim>::baryToCart(const Array<T, Dim+1>& bary_pt) const
{
    Array<T, Dim> cart_pt = corners_[0] * bary_pt[0];
    for (int i = 1; i < Dim+1; ++i) {
	cart_pt += corners_[i] * bary_pt[i];
    }
    return cart_pt;
}


template <int Dim>
template <class T>
inline Array<T, Dim+1>
BaryCoordSystem<Dim>::cartToBary(const Array<T, Dim>& cart_pt) const
{
    static Array<T, Dim> subsimplex[Dim+1];
    for (int i = 1; i < Dim+1; ++i) {
	for (int d = 0; d < Dim; ++d) {
	    subsimplex[i][d] = T(corners_[i][d]);
	}
    }

    Array<T,Dim+1> bary_pt;
    for (int i = 0; i < Dim+1; ++i) {
	subsimplex[i] = cart_pt;
	bary_pt[i] = simplex_volume(subsimplex);
	for (int d = 0; d < Dim; ++d) { 
	    subsimplex[i][d] = T(corners_[i][d]);
	}
    }
    bary_pt /= total_volume_;
    return bary_pt;
}


template<class T, int Dim>
inline Array<T, Dim> operator* (const Array<double, Dim>& a, const T b)
{
    Array<T, Dim> result;
    for (int i = 0; i < Dim; ++i) {
	result[i] = a[i] * b;
    }
    return result;
}

					
// Typedefs

typedef BaryCoordSystem<2> BaryCoordSystem2D;
typedef BaryCoordSystem<3> BaryCoordSystem3D;


}; // namespace Go


namespace std {


/// Read BaryCoordSystem from input stream
template<int Dim>
inline std::istream& operator >> (std::istream& is,
				  Go::BaryCoordSystem<Dim>& bc)
{
    bc.read(is);
    return is;
}


/// Write BaryCoordSystem to output stream
template<int Dim>
inline std::ostream& operator << (std::ostream& os,
				  const Go::BaryCoordSystem<Dim>& bc)
{
    bc.write(os);
    return os;
}


} // namespace std


#endif // _BARYCOORDSYSTEM_H

