// ==========================================================================
// Filename:    coords.cpp
//
// Description: Contains the python wrappers for the coordinate objects
//              using boost and python.
//
// Author:      L.R. McFarland
// Created:     2014nov13
// ==========================================================================

#include <boost/python.hpp>

#include "angle.h"
#include "Cartesian.h"
#include "datetime.h"
#include "spherical.h"

using namespace boost::python;


// overload wrappers

void (Coords::angle::*setAngleValue)(const double&) = &Coords::angle::setValue;
void (Coords::angle::*setAngleRadians)(const double&) = &Coords::angle::setRadians;

void (Coords::Cartesian::*setX)(const double&) = &Coords::Cartesian::x;
void (Coords::Cartesian::*setY)(const double&) = &Coords::Cartesian::y;
void (Coords::Cartesian::*setZ)(const double&) = &Coords::Cartesian::z;

void (Coords::spherical::*setR)(const double&) = &Coords::spherical::r;
void (Coords::spherical::*setTheta)(const Coords::angle&) = &Coords::spherical::theta;
void (Coords::spherical::*setPhi)(const Coords::angle&) = &Coords::spherical::phi;


BOOST_PYTHON_MODULE(coords) {


  class_<Coords::angle>("angle")

    .def("deg2rad", &Coords::angle::deg2rad)
    .staticmethod("deg2rad")

    .def("rad2deg", &Coords::angle::rad2deg)
    .staticmethod("rad2deg")

    // constructors
    .def(init<>()) // default
    .def(init<double>()) // degrees
    .def(init<double, double>()) // degrees, minutes
    .def(init<double, double, double>()) // degrees, minutes, seconds

    .def(init<Coords::angle>()) // copy

    // accessors

    .def("getValue", &Coords::angle::getValue)
    .def("setValue", setAngleValue)
    .add_property("value", &Coords::angle::getValue, setAngleValue)

    .def("getRadians", &Coords::angle::getRadians)
    .def("setRadians", setAngleRadians)
    .add_property("radians", &Coords::angle::getRadians, setAngleRadians)

    // TODO rich compare

    // operators
    .def(self + Coords::angle())
    .def(Coords::angle() + self)

    .def(self - Coords::angle())
    .def(Coords::angle() - self)

    .def(self * Coords::angle())
    .def(Coords::angle() * self)

    .def(self / Coords::angle())
    .def(Coords::angle() / self)


    // operator<<(), str not repr
    .def(self_ns::str(self_ns::self))

    ; // end of angle class_


  class_<Coords::Latitude, bases<Coords::angle> >("latitude")

        // constructors
    .def(init<>()) // default
    .def(init<double>()) // degrees
    .def(init<double, double>()) // degrees, minutes
    .def(init<double, double, double>()) // degrees, minutes, seconds

    ; // end of Latitude class_



  class_<Coords::Cartesian>("Cartesian")

    // static members
    .def_readonly("Uo", &Coords::Cartesian::Uo)
    .def_readonly("Ux", &Coords::Cartesian::Ux)
    .def_readonly("Uy", &Coords::Cartesian::Uy)
    .def_readonly("Uz", &Coords::Cartesian::Uz)

    // constructors
    .def(init<>()) // default
    .def(init<Coords::Cartesian>()) // copy
    .def(init<Coords::spherical>()) // conversion

    .def(init<double>()) // x
    .def(init<double, double>()) // x, y
    .def(init<double, double, double>()) // x, y, z

    // accessors
    .def("getX", &Coords::Cartesian::getX)
    .def("setX", setX)
    .add_property("x", &Coords::Cartesian::getX, setX)

    .def("getY", &Coords::Cartesian::getY)
    .def("setY", setY)
    .add_property("y", &Coords::Cartesian::getY, setY)

    .def("getZ", &Coords::Cartesian::getZ)
    .def("setZ", setZ)
    .add_property("z", &Coords::Cartesian::getZ, setZ)

    // operator<<(), str not repr
    .def(self_ns::str(self_ns::self))

    // operators
    .def(self + Coords::Cartesian())
    .def(Coords::Cartesian() + self)

    .def(self - Coords::Cartesian())
    .def(Coords::Cartesian() - self)

    .def(self * Coords::Cartesian())
    .def(Coords::Cartesian() * self)

    // scale
    .def(self * double())
    .def(double() * self)

    .def(self / double())

    // other methods
    .def("cross", &Coords::cross)
    .def("dot", &Coords::dot)
    .def("magnitude", &Coords::Cartesian::magnitude)
    .def("normalized", &Coords::Cartesian::normalized)

    ; // end of class_


  class_<Coords::spherical>("spherical")

    // constructors
    .def(init<>()) // default
    .def(init<Coords::spherical>()) // copy
    .def(init<Coords::Cartesian>()) // conversion

    .def(init<double>()) // r
    .def(init<double, Coords::angle>()) // r, theta
    .def(init<double, Coords::angle, Coords::angle>()) // r, theta, phi
    .def(init<double, Coords::Latitude, Coords::angle>()) // r, latitude, phi

    // accessors
    .def("getR", &Coords::spherical::getR)
    .def("setR", setR)
    .add_property("r", &Coords::spherical::getR, setR)

    .def("getTheta", &Coords::spherical::getTheta)
    .def("setTheta", setTheta)
    .add_property("theta", &Coords::spherical::getTheta, setTheta)

    .def("getPhi", &Coords::spherical::getPhi)
    .def("setPhi", setPhi)
    .add_property("phi", &Coords::spherical::getPhi, setPhi)

    // operator<<(), str not repr
    .def(self_ns::str(self_ns::self))

    // operators
    .def(self + Coords::spherical())
    .def(Coords::spherical() + self)

    .def(self - Coords::spherical())
    .def(Coords::spherical() - self)

    // scale
    .def(self * double())
    .def(double() * self)

    .def(self / double())

    // other methods
    .def("zero", &Coords::spherical::zero)

    ; // end of class_


  class_<Coords::DateTime>("datetime")

    // static members
    .def_readonly("LilianDate", &Coords::DateTime::s_LilianDate)
    .def_readonly("ModifiedJulianDate", &Coords::DateTime::s_ModifiedJulianDate)
    .def_readonly("TruncatedJulianDate", &Coords::DateTime::s_TruncatedJulianDate)

    // constructors
    .def(init<std::string>()) // default

    .def(init<>()) // default
    .def(init<int>()) // year
    .def(init<int, int>()) // year, month
    .def(init<int, int, int>()) // year, month, day
    .def(init<int, int, int, int>()) // year, month, day, hour
    .def(init<int, int, int, int, int>()) // year, month, day, hour, minute
    .def(init<int, int, int, int, int, double>()) // year, month, day, hour, minute, second
    .def(init<int, int, int, int, int, double, double>()) // year, month, day, hour, minute, second, time zone

    .def(init<Coords::DateTime>()) // copy

    // operator<<(), str not repr
    .def(self_ns::str(self_ns::self))

    // operators
    .def(self + double())
    .def(double() + self)

    .def(self - double())
    .def(self - Coords::DateTime())


    // other methods
    .def("toJulianDate", &Coords::DateTime::toJulianDate)
    .def("fromJulianDate", &Coords::DateTime::fromJulianDate)

    // TODO other Julian Date methods, Wiki, NRC, APC

    ; // end of class_



};
