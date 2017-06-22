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
void _Export_Point_int_2()
{
    scope* gmtl_Point_int_2_scope = new scope(
    class_< gmtl::Point<int,2>, bases< gmtl::VecBase<int,2> > >("Point2i", init<  >())
        .def(init< const gmtl::Point<int,2> & >())
        .def(init< const gmtl::VecBase<int,2> & >())
        .def(init< const int &, const int & >())
        .def_pickle(gmtlPickle::Point2_pickle<int>())
    );

    enum_< gmtl::Point<int,2>::Params >("Params")
        .value("Size", gmtl::Point<int,2>::Size)
    ;

    delete gmtl_Point_int_2_scope;

}
