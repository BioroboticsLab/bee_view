// GMTL is (C) Copyright 2001-2010 by Allen Bierbaum
// Distributed under the GNU Lesser General Public License 2.1 with an
// addendum covering inlined code. (See accompanying files LICENSE and
// LICENSE.addendum or http://www.gnu.org/copyleft/lesser.txt)

// This file was originally part of PyJuggler.

// PyJuggler is (C) Copyright 2002, 2003 by Patrick Hartling
// Distributed under the GNU Lesser General Public License 2.1.  (See
// accompanying file COPYING.txt or http://www.gnu.org/copyleft/lesser.txt)

// Includes ====================================================================
#include <boost/python.hpp>
#include <gmtl/Coord.h>
#include <gmtl/EulerAngleOps.h>
#include <gmtl/CoordOps.h>
#include <gmtl/Output.h>
#include <gmtl-pickle.h>

// Using =======================================================================
using namespace boost::python;

// Module ======================================================================
void _Export_Coord_gmtl_Vec_double_4_gmtl_EulerAngle_double_gmtl_ZYX()
{
    scope* gmtl_Coord_gmtl_Vec_double_4_gmtl_EulerAngle_double_gmtl_ZYX_scope = new scope(
    class_< gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> > >("Coord4dZYX", init<  >())
        .def(init< const gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> > & >())
        .def(init< const gmtl::Vec<double,4> &, const gmtl::EulerAngle<double,gmtl::ZYX> & >())
        .def_readwrite("pos", &gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::mPos)
        .def_readwrite("rot", &gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::mRot)
        .def("getPos", &gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::getPos, return_internal_reference< 1 >())
        .def("getRot", &gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::getRot, return_internal_reference< 1 >())
        .def("set",
             (gmtl::Coord<gmtl::Vec<double, 4>, gmtl::EulerAngle<double, gmtl::ZYX> >& (gmtl::Coord<gmtl::Vec<double, 4>, gmtl::EulerAngle<double, gmtl::ZYX> >::*)(const gmtl::Coord<gmtl::Vec<double, 4>, gmtl::EulerAngle<double, gmtl::ZYX> >&)) &gmtl::Coord<gmtl::Vec<double, 4>, gmtl::EulerAngle<double, gmtl::ZYX> >::operator=,
             return_internal_reference<1>())
        .def_pickle(gmtlPickle::Coord_pickle< gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >())
        .def(self == self)
        .def(self != self)
        .def(self_ns::str(self))
    );

    enum_< gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::Params >("Params")
        .value("RotSize", gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::RotSize)
        .value("PosSize", gmtl::Coord<gmtl::Vec<double, 4>,gmtl::EulerAngle<double, gmtl::ZYX> >::PosSize)
    ;

    delete gmtl_Coord_gmtl_Vec_double_4_gmtl_EulerAngle_double_gmtl_ZYX_scope;

}
