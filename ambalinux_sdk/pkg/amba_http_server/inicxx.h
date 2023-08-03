// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#ifndef __INIREADER_H__
#define __INIREADER_H__

#include <map>
#include <string>

// Read an INI file into easy-to-access name/value pairs. (Note that I've gone
// for simplicity here rather than speed, but it should be pretty decent.)
class INIReader
{
public:
  // Construct INIReader and parse given filename. See ini.h for more info
  // about the parsing.
  INIReader(std::string const& filename); // throw std::exception

  std::vector<std::string> GetKeys(std::string const& section);

  // Get a string value from INI file, returning default_value if not found.
  std::string Get(std::string const& section, std::string const& name,
    std::string const& default_value);

  // Get an integer (long) value from INI file, returning default_value if
  // not found or not a valid integer (decimal "1234", "-1234", or hex "0x4d2").
  long GetInteger(std::string const& section, std::string const& name,
    long default_value);

  // Get a real (floating point double) value from INI file, returning
  // default_value if not found or not a valid floating point value
  // according to strtod().
  double GetReal(std::string const& section, std::string const& name,
    double default_value);

  // Get a boolean value from INI file, returning default_value if not found or if
  // not a valid true/false value. Valid true values are "true", "yes", "on", "1",
  // and valid false values are "false", "no", "off", "0" (not case sensitive).
  bool GetBoolean(std::string const& section, std::string const& name,
    bool default_value);

private:
  std::map<std::string, std::string> _values;
  static std::string MakeKey(std::string const& section,
    std::string const& name);
  static int ValueHandler(void* user, const char* section, const char* name,
    const char* value);
};

#endif  // __INIREADER_H__