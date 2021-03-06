// Boost.Geometry (aka GGL, Generic Geometry Library)

// Copyright (c) 2014, Oracle and/or its affiliates.

// Licensed under the Boost Software License version 1.0.
// http://www.boost.org/users/license.html

// Contributed and/or modified by Menelaos Karavelas, on behalf of Oracle

#ifndef BOOST_GEOMETRY_TEST_SET_OPS_PL_PL_HPP
#define BOOST_GEOMETRY_TEST_SET_OPS_PL_PL_HPP


#include <boost/geometry/geometry.hpp>

namespace bg = ::boost::geometry;

#include <from_wkt.hpp>
#include <to_svg.hpp>

#include <algorithm>
#include <fstream>
#include <boost/core/ignore_unused.hpp>

#include <boost/geometry/policies/compare.hpp>
#include <boost/geometry/algorithms/equals.hpp>

#include <boost/geometry/algorithms/union.hpp>
#include <boost/geometry/algorithms/difference.hpp>
#include <boost/geometry/algorithms/intersection.hpp>
#include <boost/geometry/algorithms/sym_difference.hpp>

#include <boost/geometry/algorithms/detail/overlay/overlay_type.hpp>


//==================================================================
//==================================================================
// svg output
//==================================================================
//==================================================================

template <typename Output, typename G1, typename G2>
void set_operation_output(std::string const& set_op_id,
                          std::string const& caseid,
                          G1 const& g1, G2 const& g2,
                          Output const& output)
{
    boost::ignore_unused(set_op_id, caseid, g1, g2, output);

#if defined(TEST_WITH_SVG)
    typedef typename bg::coordinate_type<G1>::type coordinate_type;
    typedef typename bg::point_type<G1>::type point_type;

    std::ostringstream filename;
    filename << "svgs/" << set_op_id << "_" << caseid << ".svg";

    std::ofstream svg(filename.str().c_str());

    bg::svg_mapper<point_type> mapper(svg, 500, 500);

    mapper.add(g1);
    mapper.add(g2);

    mapper.map(g2, "stroke-opacity:1;stroke:rgb(153,204,0);stroke-width:4");
    mapper.map(g1, "stroke-opacity:1;stroke:rgb(51,51,153);stroke-width:2");

    BOOST_AUTO_TPL(it, output.begin());
    for (; it != output.end(); ++it)
    {
        mapper.map(*it,
                   "fill:rgb(255,0,255);stroke:rgb(0,0,0);stroke-width:1",
                   4);
    }
#endif
}


//==================================================================
//==================================================================
// testing equality of multi-points
//==================================================================
//==================================================================


struct equals
{
    template <typename MultiPoint1, typename MultiPoint2>
    static inline bool apply(MultiPoint1 const& multipoint1,
                             MultiPoint2 const& multipoint2)
    {
        MultiPoint1 mp1(multipoint1);
        MultiPoint2 mp2(multipoint2);

        std::sort(mp1.begin(), mp1.end(),
                  bg::less<typename bg::point_type<MultiPoint1>::type>());
        std::sort(mp2.begin(), mp2.end(),
                  bg::less<typename bg::point_type<MultiPoint2>::type>());

        if ( boost::size(mp1) != boost::size(mp2) )
        {
            return false;
        }

        BOOST_AUTO_TPL(it1, boost::begin(mp1));
        BOOST_AUTO_TPL(it2, boost::begin(mp2));
        for (; it1 != boost::end(mp1); ++it1, ++it2)
        {
            if ( !bg::equals(*it1, *it2) )
            {
                return false;
            }
        }
        return true;
    }
};



//==================================================================
//==================================================================
// struct for calling the appropriate set op function
//==================================================================
//==================================================================

template <bg::overlay_type OverlayType> struct set_op;


template<>
struct set_op<bg::overlay_difference>
{
    static inline std::string name() { return "difference"; }

    template <typename Geometry1, typename Geometry2, typename GeometryOut>
    static inline void apply(Geometry1 const& g1,
                             Geometry2 const& g2,
                             GeometryOut& gout)
    {
        bg::difference(g1, g2, gout);
    }
};


template<>
struct set_op<bg::overlay_union>
{
    static inline std::string name() { return "union"; }

    template <typename Geometry1, typename Geometry2, typename GeometryOut>
    static inline void apply(Geometry1 const& g1,
                             Geometry2 const& g2,
                             GeometryOut& gout)
    {
        bg::union_(g1, g2, gout);
    }
};


template<>
struct set_op<bg::overlay_intersection>
{
    static inline std::string name() { return "intersection"; }

    template <typename Geometry1, typename Geometry2, typename GeometryOut>
    static inline void apply(Geometry1 const& g1,
                             Geometry2 const& g2,
                             GeometryOut& gout)
    {
        bg::intersection(g1, g2, gout);
    }
};



//==================================================================
//==================================================================
// test the set operation of (point-like) geometries
//==================================================================
//==================================================================


template
<
    typename Geometry1,
    typename Geometry2,
    typename MultiPoint,
    bg::overlay_type OverlayType
>
class test_set_op_of_pointlike_geometries
{
private:
    template <typename G1, typename G2, typename MP>
    static inline void base_test(G1 const& geometry1,
                                 G2 const& geometry2,
                                 MP const& mp_expected,
                                 std::string const& case_id)
    {
        MultiPoint mp_output;

        set_op<OverlayType>::apply(geometry1, geometry2, mp_output);

        std::string op_name = set_op<OverlayType>::name();

        BOOST_CHECK_MESSAGE( equals::apply(mp_expected, mp_output),
                             op_name << " P/P: " << bg::wkt(geometry1)
                             << " " << bg::wkt(geometry2)
                             << " -> Expected: " << bg::wkt(mp_expected)
                             << " computed: " << bg::wkt(mp_output) );

        set_operation_output(op_name, case_id,
                             geometry1, geometry2, mp_output);

#ifdef BOOST_GEOMETRY_TEST_DEBUG
        std::cout << "Geometry #1: " << bg::wkt(geometry1) << std::endl;
        std::cout << "Geometry #2: " << bg::wkt(geometry2) << std::endl;
        std::cout << "expected " << op_name << " : "
                  << bg::wkt(mp_expected) << std::endl;
        std::cout << op_name << " : " << bg::wkt(mp_output) << std::endl;
        std::cout << std::endl;
        std::cout << "************************************" << std::endl;
        std::cout << std::endl;
        std::cout << std::endl;
#endif
    }


public:
    static inline void apply(Geometry1 const& geometry1,
                             Geometry2 const& geometry2,
                             MultiPoint const& mp_expected12,
                             MultiPoint const& mp_expected21,
                             std::string const& case_id)
    {
#ifdef BOOST_GEOMETRY_TEST_DEBUG
        std::cout << "test case: " << case_id << std::endl;
#endif

        base_test(geometry1, geometry2, mp_expected12, case_id);
        base_test(geometry2, geometry1, mp_expected21, case_id);

#ifdef BOOST_GEOMETRY_TEST_DEBUG
        std::cout << std::endl;
        std::cout << std::endl;
#endif
    }


    static inline void apply(Geometry1 const& geometry1,
                             Geometry2 const& geometry2,
                             MultiPoint const& mp_expected,
                             std::string const& case_id)
    {
        apply(geometry1, geometry2, mp_expected, mp_expected, case_id);
    }
};


#endif // BOOST_GEOMETRY_TEST_SET_OPS_PL_PL_HPP
