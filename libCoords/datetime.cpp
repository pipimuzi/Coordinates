// ================================================================
// Filename:    datetime.cpp
//
// Description: Yet another DateTime implementation. This is not
//              intended for anything other than a light weight
//              DateTime object for my astronomy applications.
//
// Author:      L.R. McFarland
// Created:     2014 Nov 06
// Language:    C++
//
//  DateTime is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  DateTime is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with Coordinates. If not, see <http://www.gnu.org/licenses/>.
// ================================================================

#include <cmath>
#include <iomanip> // for std::setw() and std::setfill()
#include <sstream>

#define DEBUG_REGEX 0
#if DEBUG_REGEX
#include <iostream>
#endif

#include<datetime.h>
#include <utils.h>

// ----- static data members -----

const std::string Coords::DateTime::s_ISO8601_format(
	   "(-){0,1}(\\d*)-" // year
	   "(0[1-9]|1[012])-" // month
	   "(0[1-9]|1\\d|2\\d|3[01])" // day
	   "T"
	   "([01]\\d|2[0-3])" // hour
	   ":"
	   "([0-5]\\d)" // minute
	   "(:"
	   "([0-5]\\d(\\.\\d*){0,1})" // second
	   "(Z|(\\+|-)(0[0-9]|1[012])(\\:){0,1}([0-5]\\d){0,1}){0,1}" // time zone
	   "){0,1}"
						     );
#if BOOST_REGEX
const boost::regex Coords::DateTime::s_ISO8601_rx(Coords::DateTime::s_ISO8601_format);
#else
const std::regex Coords::DateTime::s_ISO8601_rx(Coords::DateTime::s_ISO8601_format);
#endif

const long int Coords::DateTime::s_gDateNRC(15+31L*(10+12L*1582));
const double Coords::DateTime::s_LilianDate(2299160.5);
const double Coords::DateTime::s_ModifiedJulianDate(2400000.5);
const double Coords::DateTime::s_TruncatedJulianDate(2440000.5);
const double Coords::DateTime::s_J2000(2451545.0);
const double Coords::DateTime::s_resolution(0.0001);

Coords::DateTime::DateTime(const std::string& an_iso8601_time)
  : m_year(1970), m_month(1), m_day(1),
    m_hour(0), m_minute(0), m_second(0),
    m_is_zulu(false), m_has_timezone_colon(false), m_timezone(0), m_is_leap_year(false)
{


#if BOOST_REGEX
  boost::smatch m;
  if (!boost::regex_match(an_iso8601_time, m, s_ISO8601_rx)) {
#else
  std::smatch m;
  if (!std::regex_match(an_iso8601_time, m, s_ISO8601_rx)) {
#endif
    std::stringstream emsg;
    emsg << an_iso8601_time
	 << " not in limited ISO-8601 format: year-mm-ddThh:mm:ss[.s*][Z|(+|-)hh[:][mm]]";
    throw Coords::Error(emsg.str());
  }

  m_year = Coords::stoi(m[2]);

  if (m[1] == "-")
    m_year *= -1;

  m_month = Coords::stoi(m[3]);
  m_day = Coords::stoi(m[4]);

  if ((m_year % 4 == 0 && m_year % 100 != 0) || m_year % 400 == 0)
    m_is_leap_year = true;
  else
    m_is_leap_year = false;

  m_hour = Coords::stoi(m[5]);
  m_minute = Coords::stoi(m[6]);
  m_second = Coords::stod(m[8]);

  if (m[10] == "Z") {
    m_is_zulu = true;
    m_timezone = 0;

  } else {

    m_timezone_hh = m[12];
    m_timezone_mm = m[14];

    m_timezone = Coords::stod(m[12]);

    if (m[13] == ":")
      m_has_timezone_colon = true;

    if (m[14] != "")
      m_timezone += Coords::stod(m[14])/60.0;

    if (m[11] == "-")
      m_timezone *= -1;

  }

#if DEBUG_REGEX
  for (int i = 0; i < 15; ++i)
    std::cout << "m[" << i << "]" << m[i] << std::endl;
#endif

  isValid(an_iso8601_time);
}

void Coords::DateTime::throwError(const std::string& a_datetime, const std::string msg) throw (Error) {
  std::stringstream emsg;
  std::stringstream current_time;

  if (a_datetime == "")
    current_time << *this;
  else
    current_time << a_datetime;

  emsg << current_time.str() << ": " << msg;
  throw Coords::Error(emsg.str());
}

void Coords::DateTime::isValid(const std::string& an_iso8601_time) throw (Error) {

  if (m_month < 1 || m_month > 12)
    throwError(an_iso8601_time, "month out of range.");

  if (m_day < 1 || m_day > 31)
    throwError(an_iso8601_time, "day out of range.");

  if ((m_month == 9 || m_month == 4 || m_month == 6 || m_month == 11) && m_day > 30)
    throwError(an_iso8601_time, "Thirty days hath September, April, June and November");

  if (m_is_leap_year) {

    if (m_month == 2 && m_day > 29)
      throwError(an_iso8601_time, "Except for February all alone. It has 28, but 29 each _leap_ year.");

  } else {

    if (m_month == 2 && m_day > 28)
      throwError(an_iso8601_time, "Except for February all alone. It has _28_, but 29 each leap year.");

  }

  if (m_hour < 0 || m_hour > 24)
    throwError(an_iso8601_time, "hour out of range.");

  if (m_minute < 0 || m_minute > 60)
    throwError(an_iso8601_time, "minute out of range.");

  if (m_second < 0 || m_second > 60)
    throwError(an_iso8601_time, "second out of range.");

  if (m_timezone < -12 || m_timezone > 12)
    throwError(an_iso8601_time, "time zone out of range.");

}

// ----- copy constructor -----

Coords::DateTime::DateTime(const Coords::DateTime& a) {
  m_year = a.m_year;
  m_month = a.m_month;
  m_day = a.m_day;
  m_hour = a.m_hour;
  m_minute = a.m_minute;
  m_second = a.m_second;
  m_is_zulu = a.m_is_zulu;
  m_timezone_hh = a.m_timezone_hh;
  m_timezone_mm = a.m_timezone_mm;
  m_has_timezone_colon = a.m_has_timezone_colon;
  m_timezone = a.m_timezone;
  m_is_leap_year = a.m_is_leap_year;
}

// ----- copy assignment -----

Coords::DateTime& Coords::DateTime::operator=(const Coords::DateTime& rhs) {
  if (this == &rhs) return *this;
  m_year = rhs.m_year;
  m_month = rhs.m_month;
  m_day = rhs.m_day;
  m_hour = rhs.m_hour;
  m_minute = rhs.m_minute;
  m_second = rhs.m_second;
  m_is_zulu = rhs.m_is_zulu;
  m_timezone_hh = rhs.m_timezone_hh;
  m_timezone_mm = rhs.m_timezone_mm;
  m_has_timezone_colon = rhs.m_has_timezone_colon;
  m_timezone = rhs.m_timezone;
  m_is_leap_year = rhs.m_is_leap_year;
  return *this;
}

// ----- operators -----

Coords::DateTime& Coords::DateTime::operator+=(const double& rhs) {
  this->fromJulianDate(this->toJulianDate() + rhs, this->timezone());
  return *this;
}

Coords::DateTime& Coords::DateTime::operator-=(const double& rhs) {
  this->fromJulianDate(this->toJulianDate() - rhs, this->timezone());
  return *this;
}

Coords::DateTime Coords::operator+(const Coords::DateTime& lhs, const double& rhs) {
  Coords::DateTime temp(lhs);
  return temp += rhs;
}

Coords::DateTime Coords::operator+(const double& lhs, const Coords::DateTime& rhs) {
  return rhs + lhs; // commute
}

Coords::DateTime Coords::operator-(const Coords::DateTime& lhs, const double& rhs) {
  Coords::DateTime temp(lhs);
  return temp -= rhs;
}

double Coords::operator-(const Coords::DateTime& lhs, const Coords::DateTime& rhs) {
  return lhs.toJulianDate() - rhs.toJulianDate();
}

// ----- timezone -----


void Coords::DateTime::adjustForTimezone(int& a_year, int& a_month, int& a_day,
					 int& a_hour, int& a_minute, double& a_second,
					 const double& a_timezone) {
  // for use with date arithmetic to adjust for changes in timezone
  // e.g. hour + timezone = 26 hrs

  // ASSUMES timezone set in member method

  if (a_timezone < -12 || a_timezone > 12)
    throw Coords::Error("timezone out of range");

  bool is_leap_year(false);

  if ((a_year % 4 == 0 && a_year % 100 != 0) || a_year % 400 == 0)
    is_leap_year = true;
  else
    is_leap_year = false;


  while (a_second >= 60 - s_resolution) {
    a_second -= 60;
    a_minute += 1;
  }

  if (fabs(a_second) < s_resolution)
    a_second = 0.0;


  while (a_minute >= 60) {
    a_minute -= 60;
    a_hour += 1;
  }


  a_hour -= a_timezone;


  if (a_hour >= 24) {

    // pushed to next day
    a_hour -= 24; // ASSUMES timezone is in -12 to 12 range, i.e. only one day at most

    if (a_month == 2) { // February

      if (is_leap_year) {
	if (a_day == 29) {
	  a_month += 1; // March
	  a_day = 1;
	}
      } else {
	if (a_day == 28) {
	  a_month += 1; // March
	  a_day = 1;
	}

      }

    } else if (a_month == 12) { // December

      // pushed to next year
      if (a_day == 31) {

	a_year += 1;
	a_month = 1;
	a_day = 1;

      } else {

	a_day += 1;

      }

    } else if (a_month == 9 || a_month == 4 || a_month == 6 || a_month == 11) {

      // 30 day months

      if (a_day == 30) {
	a_month += 1;
	a_day = 1;
      } else {
	a_day += 1;
      }

    } else { // 31 day months. December is a special case handled above.

      if (a_day == 31) {
	a_month += 1;
	a_day = 1;
      } else {
	a_day += 1;
      }

    }

  }

  // pulled back across date
  if (a_hour < 0) {

    a_hour += 24; // ASSUMES timezone is in -12 to 12 range, i.e. only one day at most

    if (a_month == 1) { // January

      if (a_day == 1) {

	a_year -= 1;
	a_month = 12;
	a_day = 31;

      } else {

	a_day -= 1;

      }

    } else { // Not Jan

      if (a_day == 1) {

	a_month -= 1;

	if (a_month == 2) { // Feburary
	  if (is_leap_year)
	    a_day = 29;
	  else
	    a_day = 28;

	} else if (a_month == 9 || a_month == 4 || a_month == 6 || a_month == 11) {
	  a_day = 30;	// 30 day months

	} else {
	  a_day = 31; // 31 day months
	}

      } else { // Not first of month

	a_day -= 1;

      }

    }

  }

}


void Coords::DateTime::timezone(const double& a_timezone) {

  if (a_timezone < -12 || a_timezone > 12)
    throw Coords::Error("timezone out of range");

  if (m_timezone != 0) {
    this->fromJulianDate(this->toJulianDate(), 0); // in the 0-th timezone
  }

  m_timezone = a_timezone;
  this->adjustForTimezone(m_year, m_month, m_day, m_hour, m_minute, m_second, m_timezone);

  m_timezone_hh.clear(); // for operator<<()
  m_has_timezone_colon = false;
  m_timezone_mm.clear();

  std::stringstream current_time;
  current_time << *this;

  isValid(current_time.str());

}


// ----- as Julian Date -----

double Coords::DateTime::toJulianDateWiki() const {

  // from http://en.wikipedia.org/wiki/Julian_day

  const long int a(floor(14 - m_month)/12);
  const long int y(m_year + 4800 - a);
  const long int m(m_month + 12*a - 3);

  long int jdays(0);

  if (m_day + 31L*(m_month + 12L*m_year) >= s_gDateNRC) {

    jdays = m_day + floor((153*m + 2)/5) + 365*y + floor(y/4) - floor(y/100) + floor(y/400) - 32045;

  } else {

    jdays = m_day + floor((153*m + 2)/5) + 365*y + floor(y/4) - 32083;

  }

  double partial_day(Coords::degrees2seconds(m_hour, m_minute, m_second)/86400.0);
  partial_day += timezone()/24.0;

  return static_cast<double>(jdays) + partial_day;

}


void Coords::DateTime::fromJulianDateWiki(const double& jdays) {

  // from http://en.wikipedia.org/wiki/Julian_day

  // TODO: this does not correct for the Lilian date change. See unit tests.

  const long int y(4716);
  const long int j(1401);
  const long int m(2);
  const long int n(12);
  const long int r(4);
  const long int p(1461);
  const long int v(3);
  const long int u(5);
  const long int s(153);
  const long int w(2);
  const long int B(274277);
  const long int C(-38);

  long int f(jdays + j + (((4*jdays + B) / 146097)*3)/4 + C);
  long int e(r*f+v);
  long int g((e%p)/r);
  long int h(u*g+w);

  m_day = (h%s)/u + 1;

  m_month = (h/s + m) % n + 1;

  m_year = e/p - y + (n + m - m_month)/n;

  double d_hour = 24.0 * (jdays - floor(jdays));
  m_hour = d_hour; // implicit cast to int
  m_hour -= timezone();

  double d_minute = 60.0 * (d_hour - floor(d_hour));
  m_minute = d_minute; // implicit cast to int

  m_second = 60.0 * (d_minute - floor(d_minute));

  m_timezone = 0;
  m_timezone_hh.clear(); // for operator<<()
  m_has_timezone_colon = false;
  m_timezone_mm.clear();

}


double Coords::DateTime::toJulianDateNRC() const {

  // Calculates Julian day number from Gregorian calendar date.
  // From Numerical Recipes in C, pp. 11-12

  // WARNING: this rounds to the nearest day. See datetime_unittest.cpp for details.

  long int l_year(static_cast<long int>(m_year));   // long and local
  long int l_month(static_cast<long int>(m_month));
  long int l_day(static_cast<long int>(m_day));

  long int jdays(0);

  if (m_year == 0)
    throw Coords::Error("There is no year zero in this algorithm, but there should be.");

  if (l_year < 0)
    ++l_year;

  if (l_month > 2) {
    l_month = m_month + 1;
  } else {
    --l_year;
    l_month = m_month + 13;
  }

  jdays = static_cast<long int>(floor(365.25*l_year) + floor(30.6001*l_month) + l_day + 1720995);

  if (m_day + 31L*(m_month + 12L*m_year) >= s_gDateNRC) {
    int ja = static_cast<int>(0.01*l_year);
    jdays += 2 - ja + static_cast<int>(0.25*ja);
  }

  double partial_day(Coords::degrees2seconds(m_hour + timezone(), m_minute, m_second)/86400.0);

  return static_cast<double>(jdays) + partial_day;

}

void Coords::DateTime::fromJulianDateNRC(const double& jdays) {

  // Calculates Gregorian calendar date from Julian day number.
  // From Numerical Recipes in C, pp. 14-15

  long int ja(0);
  long int jalpha(0);
  long int jb(0);
  long int jc(0);
  long int jd(0);
  long int je(0);


  if (jdays >= s_gDateNRC) {
    jalpha = static_cast<long int>((static_cast<float>(jdays - 1867216) - 0.25)/36524.25);
    ja = jdays + 1 + jalpha - static_cast<long int>(0.25*jalpha);
  } else
    ja = jdays;

  jb = ja + 1524;
  jc = static_cast<long int>(6680.0 + (static_cast<float>(jb - 2439870) - 122.1)/365.25);
  jd = static_cast<long int>(365 * jc + (0.25*jc));
  je = static_cast<long int>((jb - jd)/30.6001);

  m_day = jb - jd - static_cast<long int>(30.6001*je);
  m_month = je - 1;

  if (m_month > 12)
    m_month -= 12;

  m_year = jc - 4715;

  if (m_month > 2)
    --m_year;

  if (m_year <= 0)
    --m_year;

  m_timezone = 0;

  m_timezone_hh.clear(); // for operator<<()
  m_has_timezone_colon = false;
  m_timezone_mm.clear();

}


double Coords::DateTime::toModifiedJulianDateAPC() const {

  // Calculates Julian day number from Gregorian calendar date.
  // from Astronomy on the Personal Computer, Montenbruck and Pfleger, p. 15

  long int l_year(static_cast<long int>(m_year));   // long and local. does not alter m_year.
  long int l_month(static_cast<long int>(m_month));
  long int l_day(static_cast<long int>(m_day));

  long int jdays(0);
  long int b(0);

  if (m_month <= 2) {
    l_month += 12;
    --l_year;
  }

  if ((10000L*l_year + 100L*l_month + l_day) <= 15821004L)
    b = -2 + ((l_year + 4716)/4) - 1179; // Julian calendar
  else
    b = (l_year/400) - (l_year/100) + (l_year/4); // Gregorian calendar

  jdays = 365L*l_year - 679004L + b + static_cast<int>(30.6001*(l_month+1)) + l_day; // at midnight

  double partial_day(Coords::degrees2seconds(m_hour, m_minute, m_second)/86400.0);
  partial_day += timezone()/24.0;

  return static_cast<double>(jdays) + partial_day;

}


void Coords::DateTime::fromModifiedJulianDateAPC(const double& jdays, const double& a_timezone) {

  // Calculates Gregorian calendar date from Julian day number.
  // from Astronomy on the Personal Computer, Montenbruck and Pfleger, p. 15-16

  // ASSUMES: jdays are Modified Julian Days

  long int a(0);
  long int b(0);
  long int c(0);
  long int d(0);
  long int e(0);
  long int f(0);

  a = static_cast<long int>(jdays + 2400001.0);

  if (a < 2299161) {
    b = 0;
    c = a + 1524; // Julian calendar
  } else {
    b = static_cast<long int>((a - 1867216.25)/36524.25);
    c = a + b - (b/4) + 1525; // Gregorian calendar
  }

  d = static_cast<long int>((c - 122.1)/365.25);
  e = 365*d + d/4;
  f = static_cast<long int>((c - e)/30.6001);

  m_day = c - e - static_cast<int>(30.6001 * f);
  m_month = f - 1 - 12*(f/14);
  m_year = d - 4715 - ((7+m_month)/10);

  double d_hour = 24.0 * (jdays - floor(jdays));
  m_hour = d_hour; // implicit cast to int

  double d_minute = 60.0 * (d_hour - floor(d_hour));
  m_minute = d_minute; // implicit cast to int

  m_second = 60.0 * (d_minute - floor(d_minute));

  m_timezone = a_timezone;
  this->adjustForTimezone(m_year, m_month, m_day, m_hour, m_minute, m_second, m_timezone);

}


// ----- string utility -----

void Coords::DateTime2String(const Coords::DateTime& a_datetime, std::stringstream& a_string) {

  int a_year(a_datetime.year());
  int a_month(a_datetime.month());
  int a_day(a_datetime.day());
  int a_hour(a_datetime.hour());
  int a_minute(a_datetime.minute());
  double a_second(a_datetime.second());

  // Round seconds (operator<<() output only) to cover rounding issues in calculation.

  if (fabs(a_second) < DateTime::s_resolution)
    a_second = 0.0;

  if (60 - a_second < DateTime::s_resolution && a_second > 0.0) {
    a_second = 0.0;
    a_minute += 1;
  }

  if (a_minute == 60) {
    a_minute = 0;
    a_hour += 1;
  }

  if (a_hour == 24) {
    a_hour = 0.0;
    a_day += 1;
  }

  // TODO months, years? throw is valid? This is info only. Bad dates
  // fed back into constructor will throw exceptions. Do here too?

  a_string << a_year << "-"
	   << std::setw(2) << std::setfill('0') << a_month << "-"
	   << std::setw(2) << std::setfill('0') << a_day
	   << "T"
	   << std::setw(2) << std::setfill('0') << a_hour << ":"
	   << std::setw(2) << std::setfill('0') << a_minute << ":"
	   << std::setw(2) << std::setfill('0') << a_second;

  // TODO seconds needs to set "1.5" to "01.5" and are not quite
  // idempotent. Times with out seconds get :00 added by operator<<()

  if (a_datetime.isZulu())
    a_string << "Z";

  if (a_datetime.timezone() != 0) {

    if (a_datetime.timezoneHH() != "") {

      // ASSUMES: having timezoneHH means it was constructed from an ISO-8601 string

      if (a_datetime.timezone() > 0)
	a_string << "+";
      else
	a_string << "-";

      a_string << std::setw(2) << std::setfill('0') << a_datetime.timezoneHH();

      if (a_datetime.hasTimezoneColon())
	a_string << ":";

      if (a_datetime.timezoneMM() != "")
	a_string << std::setw(2) << std::setfill('0') << a_datetime.timezoneMM();

    } else {

      if (a_datetime.timezone() < 0) {
	a_string << "-";
	a_string << std::setw(2) << std::setfill('0') << -a_datetime.timezone();

      } else {
	a_string << "+";
	a_string << std::setw(2) << std::setfill('0') << a_datetime.timezone();
      }

    }

  }

  // TODO fall through exception?

}
