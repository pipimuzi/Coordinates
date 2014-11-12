// ================================================================
// Filename:    angle.h
//
// Description: This defines an angle class for use in physics
//              applications.
//
// Author:      L.R. McFarland
// Created:     2014 Nov 06
// Language:    C++
//
//  Angle is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  Angle is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Orbits.  If not, see <http://www.gnu.org/licenses/>.
// ================================================================


#pragma once

#include <cmath>
#include <sstream>

#include <utils.h>


namespace Coords {

  // =================
  // ===== Angle =====
  // =================

  class Angle {
    // base class for latitude, longitude, declination and right ascension

  public:
    // angle unit convertors
    static double deg2rad(const double& deg) {return deg*M_PI/180.0;}
    static double rad2deg(const double& rad) {return rad*180.0/M_PI;}

    // ----- ctor and dtor -----

    explicit Angle(const double& a_deg = 0.0,
		   const double& a_min = 0.0,
		   const double& a_sec = 0.0);

    explicit Angle(const std::string& a_deg, // The ambiguity is in the box.
		   const std::string& a_min = "0",
		   const std::string& a_sec = "0");

    virtual ~Angle() {};

     Angle(const Angle& a);
     Angle& operator=(const Angle& rhs);

    // ----- accessors -----
    void          value(const double& a_value) {m_value = a_value;}
    const double& value() const                {return m_value;}

    void          setValue(const double& a_value) {m_value = a_value;}
    double        getValue() const                {return m_value;} // for boost

    void          radians(const double& a_value) {value(rad2deg(a_value));}
    double        radians() const                {return deg2rad(value());}

    void          setRadians(const double& a_value) {value(rad2deg(a_value));}
    double        getRadians() const                {return deg2rad(value());} // for boost

    // ----- boolean operators -----

     bool operator== (const Angle& rhs) const;
     bool operator!= (const Angle& rhs) const;

     bool operator< (const Angle& rhs) const;
     bool operator<= (const Angle& rhs) const;

     bool operator> (const Angle& rhs) const;
     bool operator>= (const Angle& rhs) const;

    // ----- in-place operators -----

    virtual Angle& operator+=(const Angle& rhs);
    virtual Angle& operator-=(const Angle& rhs);

    virtual Angle& operator*=(const Angle& rhs);
    virtual Angle& operator/=(const Angle& rhs) throw (DivideByZeroError);


    // ----- other methods -----
    virtual void normalize();  // TODO normalized -> return a new copy?

  private:

    double m_value; // degrees for declination, latitude, longitude, seconds for right ascension

  };


  // ---------------------
  // ----- operators -----
  // ---------------------

  Angle operator+ (const Angle& lhs, const Angle& rhs);
  Angle operator- (const Angle& lhs, const Angle& rhs);
  Angle operator- (const Angle& rhs); // unitary minus

  Angle operator* (const Angle& lhs, const Angle& rhs);
  Angle operator/ (const Angle& lhs, const Angle& rhs) throw (DivideByZeroError);


  // -------------------------------
  // ----- output operator<<() -----
  // -------------------------------

  // output operator<<
  void value2DMSString(const double& a_value, std::stringstream& a_string);
  void value2HMSString(const double& a_value, std::stringstream& a_string);

  // inline for boost. Use hpp instead?
  inline std::ostream& operator<< (std::ostream& os, const Coords::Angle& a) {
    std::stringstream out;
    Coords::value2DMSString(a.value(), out);
    return os << out.str();
  }


} // end namespace Coords


