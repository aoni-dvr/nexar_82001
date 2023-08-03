// Read an INI file into easy-to-access name/value pairs.

// inih and INIReader are released under the New BSD license (see LICENSE.txt).
// Go to the project home page for more info:
//
// https://github.com/benhoyt/inih

#include "public.h"
#include "inicxx.h"
#include "ini.h"
#include "misc.h"

INIReader::INIReader(std::string const& filename)
{
  if (ini_parse(filename.c_str(), ValueHandler, this)) {
    throw std::exception();
  }
}

std::vector<std::string> INIReader::GetKeys(std::string const& section) {
  std::vector<std::string> ret;
  ret.reserve(10);
  std::string key = section + "=";
  for (auto const& i : _values) {
    if (start_with_nocase(i.first.c_str(), key.c_str())) {
      ret.emplace_back(i.first.substr(key.size()));
    }
  }
  return ret;
}

std::string INIReader::Get(std::string const& section, std::string const& name,
  std::string const& default_value)
{
  std::string key = MakeKey(section, name);
  return _values.count(key) ? _values[key] : default_value;
}

long INIReader::GetInteger(std::string const& section, std::string const& name,
  long default_value)
{
  std::string valstr = Get(section, name, "");
  const char* value = valstr.c_str();
  char* end;
  // This parses "1234" (decimal) and also "0x4D2" (hex)
  long n = strtol(value, &end, 0);
  return end > value ? n : default_value;
}

double INIReader::GetReal(std::string const& section, std::string const& name,
  double default_value)
{
  std::string valstr = Get(section, name, "");
  const char* value = valstr.c_str();
  char* end;
  double n = strtod(value, &end);
  return end > value ? n : default_value;
}

bool INIReader::GetBoolean(std::string const& section, std::string const& name,
  bool default_value)
{
  std::string valstr = Get(section, name, "");
  // Convert to lower case to make string comparisons case-insensitive
  std::transform(valstr.begin(), valstr.end(), valstr.begin(), ::tolower);
  if (valstr == "true" || valstr == "yes" || valstr == "on" || valstr == "1")
    return true;
  else if (valstr == "false" || valstr == "no" || valstr == "off" || valstr == "0")
    return false;
  else
    return default_value;
}

std::string INIReader::MakeKey(std::string const& section,
  std::string const& name)
{
  std::string key = section + "=" + name;
  // Convert to lower case to make section/name lookups case-insensitive
  std::transform(key.begin(), key.end(), key.begin(), ::tolower);
  return key;
}

int INIReader::ValueHandler(void* user, const char* section, const char* name,
  const char* value)
{
  INIReader* reader = (INIReader*)user;
  std::string key = MakeKey(section, name);
  if (reader->_values[key].size() > 0)
    reader->_values[key] += "\n";
  reader->_values[key] += value;
  return 1;
}