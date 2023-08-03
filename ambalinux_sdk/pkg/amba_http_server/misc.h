#pragma once

#include "gsl.h"

using gsl::char_view;

typedef std::pair<uint32_t, uint32_t> http_range_t;

typedef std::vector<http_range_t> http_ranges_t;

char const* has_str(char_view const& view, char const* key, size_t keylen);
bool same_str(char_view const& view, char const* key, size_t keylen);
bool same_str_nocase(char_view const& view, char const* key, size_t keylen);

bool is_dir(char const* path);

bool parse_ranges_header(char_view const& header, http_ranges_t& ranges);

bool uniform_ranges(http_ranges_t& ranges, size_t length);

size_t get_range_length(http_range_t const& range);

bool start_with_nocase(char const* p, char const* q);

bool end_with_nocase(char const* p, char const* q);

void uniform_filename(std::string& name);

void change_limit();