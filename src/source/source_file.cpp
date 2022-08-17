//
// Created by chronium on 16.08.2022.
//

#include <source/source_file.hpp>
#include <iostream>
#include <sstream>
#include <fstream>

namespace aatbe::source {

SrcFile::SrcFile(std::string *content)
    : content(content) {
}

std::unique_ptr<SrcFile> SrcFile::FromString(std::string &content) {
  return std::make_unique<SrcFile>(&content);
}

std::unique_ptr<SrcFile> SrcFile::FromString(const char *content) {
  return std::make_unique<SrcFile>(new std::string(content));
}

std::unique_ptr<SrcFile> SrcFile::FromFile(std::string &path) {
  std::ifstream input(path);

  if (!input.is_open()) {
    std::cerr << "Could not open the file - '"
              << path << "'" << std::endl;
    exit(EXIT_FAILURE);
  }

  auto content = new std::string((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

  return std::make_unique<SrcFile>(content);
}

char SrcFile::Char(size_t at) {
  if (at >= this->content->length())
    return 0;

  return this->content->at(at);
}

bool SrcFile::Contains(size_t at, const char *str) {
  return SrcFile::Contains(at, std::string(str));
}

bool SrcFile::Contains(size_t at, std::string str) {
  return this->content->substr(at, str.length()) == str;
}

}
