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
#include <gmtl/Point.h>
#include <gmtl-pickle.h>

// Using =======================================================================
using namespace boost::python;

// Module ======================================================================
void _Export_Point_float_3()
{
    scope* gmtl_Point_float_3_scope = new scope(
    class_< gmtl::Point<float,3>, bases< gmtl::VecBase<float,3> > >("Point3f", init<  >())
        .def(init< const gmtl::Point<float,3> & >())
        .def(init< const gmtl::VecBase<float,3> & >())
        .def(init< const float &, const float &, const float & >())
        .def_pickle(gmtlPickle::Point3_pickle<float>())
    );

    enum_< gmtl::Point<float,3>::Params >("Params")
        .value("Size", gmtl::Point<float,3>::Size)
    ;

    delete gmtl_Point_float_3_scope;

}
