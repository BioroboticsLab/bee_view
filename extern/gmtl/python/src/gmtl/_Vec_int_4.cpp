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
#include <gmtl/Vec.h>
#include <gmtl-pickle.h>

// Using =======================================================================
using namespace boost::python;

// Module ======================================================================
void _Export_Vec_int_4()
{
    scope* gmtl_Vec_int_4_scope = new scope(
    class_< gmtl::Vec<int,4>, bases< gmtl::VecBase<int,4> > >("Vec4i", init<  >())
        .def(init< const gmtl::Vec<int,4> & >())
        .def(init< const gmtl::VecBase<int,4> & >())
        .def(init< const int &, const int &, const int &, const int & >())
        .def_pickle(gmtlPickle::Vec4_pickle<int>())
    );

    enum_< gmtl::Vec<int,4>::Params >("Params")
        .value("Size", gmtl::Vec<int,4>::Size)
    ;

    delete gmtl_Vec_int_4_scope;

}
