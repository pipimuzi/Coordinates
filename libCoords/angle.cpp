// ==================================================================
// Filename:    angle.cpp
//
// Description: This implements the angle class.
//
// Author:      L.R. McFarland
// Created:     2014 June 19
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
// ==================================================================

#include <iomanip> // for std::setw() and std::setfill()

#include <angle.h>
#include <utils.h>

// =================
// ===== Angle =====
// =================

// ----- constructors -----

Coords::Angle::Angle(const double& a_deg_or_hr,
		     const double& a_min,
		     const double& a_sec) {
  value(degrees2seconds(a_deg_or_hr, a_min, a_sec)/3600.0);
}

Coords::Angle::Angle(const std::string& a_deg_or_hr,
		     const std::string& a_min,
		     const std::string& a_sec) {
  value(Angle(Coords::stod(a_deg_or_hr),
	      Coords::stod(a_min),
	      Coords::stod(a_sec)).value());
  // TODO bad string exception with C++11 stod
  // TODO delegating constructors in C++11
}


// copy constructor
Coords::Angle::Angle(const Coords::Angle& a) {
  m_value = a.value();
}

// copy assignment
Coords::Angle& Coords::Angle::operator=(const Coords::Angle& rhs) {
  if (this == &rhs) return *this;
  m_value = rhs.value();
  return *this;
}

// ----- bool operators -----

bool Coords::Angle::operator== (const Coords::Angle& rhs) const {
  return m_value == rhs.value();
}

bool Coords::Angle::operator!= (const Coords::Angle& rhs) const {
  return !operator==(rhs);
}

bool Coords::Angle::operator< (const Coords::Angle& rhs) const {
  return m_value < rhs.value();
}

bool Coords::Angle::operator<= (const Coords::Angle& rhs) const {
  return m_value <= rhs.value();
}

bool Coords::Angle::operator> (const Coords::Angle& rhs) const {
  return m_value > rhs.value();
}

bool Coords::Angle::operator>= (const Coords::Angle& rhs) const {
  return m_value >= rhs.value();
}

// ----- in-place operators -----

Coords::Angle& Coords::Angle::operator+=(const Coords::Angle& rhs) {
  m_value += rhs.value();
  return *this;
}

Coords::Angle& Coords::Angle::operator-=(const Coords::Angle& rhs) {
  m_value -= rhs.value();
  return *this;
}

Coords::Angle& Coords::Angle::operator*=(const Coords::Angle& rhs) {
  m_value *= rhs.value();
  return *this;
}

Coords::Angle& Coords::Angle::operator/=(const Coords::Angle& rhs) throw (DivideByZeroError) {
  if (rhs.value() == 0)
    throw DivideByZeroError();
  m_value /= rhs.value();
  return *this;
}

// ---------------------
// ----- operators -----
// ---------------------

Coords::Angle Coords::operator+(const Coords::Angle& lhs, const Coords::Angle& rhs) {
  return Coords::Angle(lhs.value() + rhs.value());
}

Coords::Angle Coords::operator-(const Coords::Angle& lhs, const Coords::Angle& rhs) {
  return Coords::Angle(lhs.value() - rhs.value());
}

Coords::Angle Coords::operator-(const Coords::Angle& rhs) {
  return Coords::Angle(-rhs.value());
}

Coords::Angle Coords::operator*(const Coords::Angle& lhs, const Coords::Angle& rhs) {
  return Coords::Angle(lhs.value() * rhs.value());
}

Coords::Angle Coords::operator/(const Coords::Angle& lhs, const Coords::Angle& rhs)
  throw (DivideByZeroError) {
  if (rhs.value() == 0)
    throw DivideByZeroError();
  return Coords::Angle(lhs.value() / rhs.value());
}

// -------------------------
// ----- other methods -----
// -------------------------

// TODO RA?
// TODO this is not right
// >>> rad2deg(math.asin(math.sin(deg2rad(45))))
// 45.0
// >>> rad2deg(math.asin(math.sin(deg2rad(45+360))))
// -44.999999999999936
// >>> rad2deg(math.asin(math.sin(deg2rad(45-360))))
// -45.00000000000001
// >>> rad2deg(math.asin(math.sin(deg2rad(45+360*2))))
// 44.99999999999992
// >>> rad2deg(math.asin(math.sin(deg2rad(45+360*3))))
// -45.00000000000001
// >>> rad2deg(math.asin(math.sin(deg2rad(45+360*4))))
// 44.99999999999999
// TODO fix this Angle(45 * 10).normalize() != 180
// >>> rad2deg(math.acos(math.cos(deg2rad(45+360*4))))
// 45.00000000000001
// >>> rad2deg(math.acos(math.cos(deg2rad(45+360*5))))
// 314.99999999999983

void Coords::Angle::normalize() {
  // bring back into 0-360 range
  while (m_value > 360)
    m_value -= 360;
}


// -----------------------------
// ----- output operator<< -----
// -----------------------------

void Coords::value2DMSString(const double& a_value, std::stringstream& a_string) {

  // output as degrees minutes seconds
  
  bool isNegative(false);
  
  if (a_value < 0)
    isNegative = true;
  
  double degrees = fabs(a_value);
  double minutes = 60 * (degrees - floor(degrees));
  double seconds = 60 * (minutes - floor(minutes));
  
  if (isNegative)
    degrees = -1 * floor(degrees);
  else
    degrees = floor(degrees);
  
  a_string << degrees << "* " << floor(minutes) << "\' " << seconds << "\"";
  
}

void Coords::value2HMSString(const double& a_value, std::stringstream& a_string) {

  // output as time 00:00:00
  
  bool isNegative(false);
  
  if (a_value < 0)
    isNegative = true;
  
  double degrees = fabs(a_value);
  double minutes = 60 * (degrees - floor(degrees));
  double seconds = 60 * (minutes - floor(minutes));
  
  if (isNegative)
    degrees = -1 * floor(degrees);
  else
    degrees = floor(degrees);

  a_string << std::setw(2) << std::setfill('0') << degrees 
	   << ":" << std::setw(2) << std::setfill('0') << floor(minutes) 
	   << ":" << std::setw(2) << std::setfill('0') << seconds;
  
}

