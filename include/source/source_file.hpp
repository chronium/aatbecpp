//
// Created by chronium on 16.08.2022.
//

#pragma once

#include <string>

namespace aatbe::source {

class SrcFile {
public:
  explicit SrcFile(std::string *content);

  static std::unique_ptr<SrcFile> FromString(std::string &content);
  static std::unique_ptr<SrcFile> FromString(const char *content);
  static std::unique_ptr<SrcFile> FromFile(std::string &path);

  char Char(size_t at);

  bool Contains(size_t at, const char *str);
  bool Contains(size_t at, std::string str);
private:
  std::unique_ptr<std::string> content;
};

}
