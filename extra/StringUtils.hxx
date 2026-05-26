/* ------------------------------------------------------------------   */
/*      item            : StringUtils.hxx
        made by         : Joost Ellerbroek
        date            : 080314
        category        : header file
        description     : String manipulation utilities
        changes         : 080314 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 Joost Ellerbroek/René van Paassen
        license         : EUPL-1.2
        documentation   : DUECA_API
*/

#ifndef StringUtils_hxx
#define StringUtils_hxx

#ifndef DOUBLE_PRECISION
#define DOUBLE_PRECISION 16
#endif

#include <dueca_ns.h>

#include <string>
#include <iomanip>
#include <sstream>
#include <vector>

namespace dueca {

/**
 * \defgroup StringUtils String manipulation functions
 */
/*@{*/
/** \brief std::string stream in operator
  *
  * Templated function that converts the right-hand-side object to a string,
  * and appends it to a string.
  * \param lhs String to append to
  * \param rhs Object to append to string lhs
  * \return Reference to string lhs
  */
template <typename T>
    std::string& operator<<(std::string& lhs, const T& rhs)
{
  std::ostringstream os;
  os <<  rhs;
  lhs+=os.str();
  return lhs;
};

/** \brief Convert a string to a numeric
  *
  * \param s String containing a textual representation of a number
  * \param num The variable to output the converted numeric to
  * \return Reference to parameter num
  */
template <class T> T& s2num(const std::string& s, T& num)
{
  std::istringstream stm;
  stm.str(s);
  stm >> std::skipws; // skip whitespace!
  stm >> num;
  return num;
};

/** \brief Convert any numeric to a string
  *
  * \param num Numeric to convert to a string
  * \param width (optional) When provided, the returned string is padded to the right with zeros to the specified width
  * \return Newly created string containing textual representation of parameter num
  */
template <typename T>
    std::string num2s(const T& num, int width = -1)
{
  std::ostringstream os;
  if (width > 0) {
    os.setf(std::ios::right);
    os.fill('0');
    os.width(width);
  }
  os <<  num;
  return os.str();
}

/** \brief Convert a floating point value to a string
 *
 * The converted string uses scientific format, and higher precision than num2s
 * \param num Value to convert to a string
 * \return Newly created string containing textual representation of parameter num
 */
template <class T> std::string real2s(const T &num)
{
  std::ostringstream os;
  os << std::scientific;
  os << std::setprecision(DOUBLE_PRECISION) << num;
  return os.str();
};

/** \brief Replace all occurences of a certain search string in a string
  *
  * \param s Source string
  * \param search Substring to be replaced
  * \param repl Replacement string
  * \return The new string
  */
std::string replaceAll(std::string s, const std::string & search, const std::string & repl);

/** \brief Split strings into pieces
  *
  * \param input Input string to cut into pieces
  * \param output Vector to append the pieces to. If the vector is not empty, pieces are appended to the end of the vector
  * \param separators Character(s) to recognise as separator between pieces
  * \return The amount of generated pieces
  */
unsigned int split(const std::string & input, std::vector<std::string> & output, const std::string & separators=" \t\n\015");

/** \brief Strip definable characters from both ends of a string
  *
  * \param s String to parse
  * \param pattern Characters that should be trimmed from the string
  * \return The trimmed string
  */
std::string trim(const std::string& s, const std::string & pattern=", \t\n\015");
/*@}*/

} // namespace dueca
#endif
