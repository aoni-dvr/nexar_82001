#include "public.h"
#include "http/cpp_http_parser.h"
//
//namespace http
//{
//  // "text/*,image/gif;q=1,image/webp;q=0.2"
//  std::vector<std::pair<std::string, std::string>>
//    parse_accept_header(std::string const& value) {
//    std::vector<std::pair<std::string, std::string>> media_types;
//    std::string compact_value(value);
//    compact_value.erase(std::remove(compact_value.begin(),
//      compact_value.end(), ' '), compact_value.end());
//
//    size_t offset = 0;
//    size_t pos = 0;
//    for (; pos != std::string::npos;) {
//      std::string item;
//      pos = compact_value.find_first_of(',', offset);
//      item = compact_value.substr(offset, pos - offset);
//      size_t pos1 = item.find_first_of(';');
//      if (pos1 == std::string::npos) {
//        media_types.push_back(std::make_pair(item, ""));
//      } else {
//        std::string type = item.substr(0, pos1);
//        ++pos1;
//        std::string quality = item.substr(pos1);
//        media_types.push_back(std::make_pair(type, quality));
//      }
//      offset = pos + 1;
//    }
//    return media_types;
//  }
//
//  // "w=300, h = 200"
//  std::pair<size_t, size_t> parse_original_image_header(
//    std::string const& value) {
//    std::pair<size_t, size_t> ret;
//    std::vector<std::string> segments;
//    std::string compact_value(value);
//    compact_value.erase(std::remove(compact_value.begin(),
//      compact_value.end(), ' '), compact_value.end());
//
//    size_t offset = 0;
//    size_t pos = 0;
//    for (; pos != std::string::npos;) {
//      std::string item;
//      pos = compact_value.find_first_of(',', offset);
//      item = compact_value.substr(offset, pos - offset);
//      size_t pos1 = item.find_first_of(';');
//      if (pos1 == std::string::npos) {
//        segments.push_back(item);
//      } else {
//        segments.push_back(item.substr(0, pos1));
//      }
//      offset = pos + 1;
//    }
//
//    auto it = std::find_if(segments.begin(), segments.end(),
//      [](std::string const& segment) {
//      return start_with_nocase(segment.c_str(), "w=");
//    });
//    if (it != segments.end()) {
//      ret.first = atoi(it->c_str() + 2);
//      if (std::to_string(ret.first) != (it->c_str() + 2)) {
//        LOG_NOTICE("invalid header: " << value);
//        return std::make_pair(0, 0);
//      }
//    }
//
//    it = std::find_if(segments.begin(), segments.end(),
//      [](std::string const& segment) {
//      return start_with_nocase(segment.c_str(), "h=");
//    });
//    if (it != segments.end()) {
//      ret.second = atoi(it->c_str() + 2);
//      if (std::to_string(ret.second) != (it->c_str() + 2)) {
//        LOG_NOTICE("invalid header: " << value);
//        return std::make_pair(0, 0);
//      }
//    }
//
//    return ret;
//  }
//} // namespace http
//
//namespace test
//{
//  void test_http_parse_accept_header() {
//    std::string value = "text/*, image/gif;q=1,image/webp;q=0.2 ";
//    auto types = http::parse_accept_header(value);
//    my_assert(types.size() == 3);
//    my_assert(types[0].first == "text/*");
//    my_assert(types[0].second == "");
//    my_assert(types[1].first == "image/gif");
//    my_assert(types[1].second == "q=1");
//    my_assert(types[2].first == "image/webp");
//    my_assert(types[2].second == "q=0.2");
//  }
//
//  void test_http_parse_original_image_header() {
//    std::string value = "x = 100, y=200";
//    auto ret = http::parse_original_image_header(value);
//    my_assert(ret.first == 100);
//    my_assert(ret.second == 200);
//  }
//}