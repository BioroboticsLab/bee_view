// GMTL is (C) Copyright 2001-2010 by Allen Bierbaum
// Distributed under the GNU Lesser General Public License 2.1 with an
// addendum covering inlined code. (See accompanying files LICENSE and
// LICENSE.addendum or http://www.gnu.org/copyleft/lesser.txt)

#ifndef _GMTL_PARAMETRIC_CURVE_TEST_H_
#define _GMTL_PARAMETRIC_CURVE_TEST_H_

#include <cppunit/extensions/HelperMacros.h>

namespace gmtlTest
{

class ParametricCurveTest : public CppUnit::TestFixture
{
   CPPUNIT_TEST_SUITE(ParametricCurveTest);

   CPPUNIT_TEST(testQuadraticCurve);

public:
   void testQuadraticCurve();
};

}

#endif

