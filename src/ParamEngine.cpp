/************************************************************************************
 * WrightEagle (Soccer Simulation League 2D) * BASE SOURCE CODE RELEASE 2016 *
 * Copyright (c) 1998-2016 WrightEagle 2D Soccer Simulation Team, * Multi-Agent
 *Systems Lab.,                                * School of Computer Science and
 *Technology,               * University of Science and Technology of China *
 * All rights reserved. *
 *                                                                                  *
 * Redistribution and use in source and binary forms, with or without *
 * modification, are permitted provided that the following conditions are met: *
 *     * Redistributions of source code must retain the above copyright *
 *       notice, this list of conditions and the following disclaimer. *
 *     * Redistributions in binary form must reproduce the above copyright *
 *       notice, this list of conditions and the following disclaimer in the *
 *       documentation and/or other materials provided with the distribution. *
 *     * Neither the name of the WrightEagle 2D Soccer Simulation Team nor the *
 *       names of its contributors may be used to endorse or promote products *
 *       derived from this software without specific prior written permission. *
 *                                                                                  *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *AND  * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *IMPLIED    * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *PURPOSE ARE           * DISCLAIMED. IN NO EVENT SHALL WrightEagle 2D Soccer
 *Simulation Team BE LIABLE    * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *EXEMPLARY, OR CONSEQUENTIAL       * DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *PROCUREMENT OF SUBSTITUTE GOODS OR       * SERVICES; LOSS OF USE, DATA, OR
 *PROFITS; OR BUSINESS INTERRUPTION) HOWEVER       * CAUSED AND ON ANY THEORY OF
 *LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,    * OR TORT (INCLUDING
 *NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF * THIS SOFTWARE,
 *EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                *
 ************************************************************************************/

#include "ParamEngine.h"
#include "PlayerParam.h"
#include "ServerParam.h"
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>

bool ParamEngine::GetParam(const char *name, ParamEngine::ParamPtr &it) {
  if (name != 0) {
    int id = hash(name);
    ParamList &param_list = mParamLists[id];
    it = param_list.begin();
    for (; it != param_list.end(); ++it) {
      if (str_equal(name, it->name.c_str())) {
        return true;
      }
    }
  }
  return false;
}

bool ParamEngine::SetParamFromString(const char *name, const char *buffer) {
  if (name != 0 && buffer != 0) {
    ParamPtr it;
    bool found = GetParam(name, it);
    if (found == true) {
      if (buffer[0] == '\0') {
        std::cerr << __FILE__ << ":" << __LINE__
                  << ": miss argument value of \'" << name << "\'" << std::endl;
        return false;
      }

      switch (it->type) {
      case V_INT:
        SetParam(name, atoi(buffer));
        break;
      case V_DOUBLE:
        SetParam(name, strtod(buffer, 0));
        break;
      case V_STRING: {
        int n = strlen(buffer);
        char *tmp = new char[n + 1];
        strcpy(tmp, buffer);
        if (tmp[0] == '\"') {
          tmp[n - 1] = '\0';
          SetParam(name, std::string(tmp + 1)); // strip qutoes
        } else {
          SetParam(name, std::string(tmp));
        }
        delete[] tmp;
      } break;
      case V_ONOFF:
        if (str_equal(buffer, "on") || str_equal(buffer, "true") ||
            str_equal(buffer, "1")) {
          SetParam(name, true);
        } else if (str_equal(buffer, "off") || str_equal(buffer, "false") ||
                   str_equal(buffer, "0")) {
          SetParam(name, false);
        } else {
          std::cerr << __FILE__ << ":" << __LINE__
                    << ": boolean value must be on/true or off/false of \'"
                    << name << "\'" << std::endl;
        }
        break;
      default:
        return false;
      }
      return true;
    } else {
      //			ParamPtr it;
      //			if (ServerParam::instance().GetParam(name, it)
      //== false && PlayerParam::instance().GetParam(name, it) == false){
      //				std::cerr << __FILE__ << ":" << __LINE__
      //<< ": unknown argument \'" << name << "\'" << std::endl;
      //			}
    }
  }
  return false;
}

bool ParamEngine::PrintParam(const char *name, std::ostream &os) {
  if (name != 0) {
    ParamPtr it;
    bool found = GetParam(name, it);
    if (found == true) {
      os << it->name << " = ";
      switch (it->type) {
      case V_INT:
        os << *static_cast<int *>(it->ptr);
        break;
      case V_DOUBLE:
        os << *static_cast<double *>(it->ptr);
        break;
      case V_STRING:
        os << '\"' << *static_cast<std::string *>(it->ptr) << '\"';
        break;
      case V_ONOFF:
        if (*static_cast<bool *>(it->ptr) == true) {
          os << "on";
        } else {
          os << "off";
        }
        break;
      default:
        return false;
      }
      os << std::endl;
      return true;
    } else {
      std::cerr << __FILE__ << ":" << __LINE__ << ": unknown argument \'"
                << name << "\'" << std::endl;
    }
  }
  return false;
}

bool ParamEngine::PrintParam(const char *name) {
  return PrintParam(name, std::cout);
}

void ParamEngine::DumpParam(std::ostream &os) {
  int sum = 0;
  int count = 0;
  for (int i = 0; i < HASH_SIZE; ++i) {
    ParamList &param_list = mParamLists[i];
    sum += param_list.size();
    count += param_list.size() > 0 ? 1 : 0;
    ParamPtr it = param_list.begin();
    for (; it != param_list.end(); ++it) {
      // os << "# " << i << std::endl;
      PrintParam(it->name.c_str(), os);
    }
  }
  os << "#hash hit rate " << float(count) / sum << std::endl;
}

void ParamEngine::DumpParam() { DumpParam(std::cout); }

bool ParamEngine::SaveToConfigFile(const char *file_name) {
  std::ofstream config_file(file_name);
  if (config_file.good()) {
    DumpParam(config_file);
    config_file.close();
    return true;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ": can not open file \'"
              << file_name << "\'" << std::endl;
  }
  return false;
}

void ParamEngine::ParseFromCmdLine(int argc, char **argv) {
  char *name;
  char *buffer;
  char dummy = '\0';

  while (argc > 0) {
    if (**argv == '-') {
      name = *argv + 1;
      if (argc > 1 && **(argv + 1) != '-') {
        buffer = *(argv + 1);
      } else {
        buffer = &dummy;
      }
      SetParamFromString(name, buffer);
    }
    --argc;
    ++argv;
  }
}

void ParamEngine::ParseFromServerMsg(const char *line) {
  const char *p = line;
  const char *q;
  char name[256];
  char buffer[256];
  int n;

  while (*p != '\0') {
    while (*p == '(' || *p == ' ')
      p++;
    q = p;
    while (*q != ' ')
      q++;
    n = q - p;
    strncpy(name, p, n * sizeof(char));
    name[n] = '\0';
    p = q + 1;
    while (*q != ')')
      q++;
    n = q - p;
    strncpy(buffer, p, n * sizeof(char));
    buffer[n] = '\0';
    SetParamFromString(name, buffer);
    while (*q == ')')
      q++;
    p = q;
  }
}

bool ParamEngine::ParseFromConfigFile(const char *file_name) {
  static const int LINE_SIZE = 256;
  char line[LINE_SIZE];
  char name[256];
  char buffer[256];
  std::ifstream config_file(file_name);
  char *p, *q;
  int n;

  if (config_file.good()) {
    while (!config_file.eof()) {
      config_file.getline(line, LINE_SIZE);
      if (line[0] == '#' || line[0] == '\0' || line[0] == '\n' ||
          line[0] == '\r') {
        continue;
      }
      p = line;
      while (isspace(*p))
        p++;
      q = p;
      while (*q != '=' && !isspace(*q))
        q++;
      n = q - p;
      strncpy(name, p, n * sizeof(char));
      name[n] = 0;
      p = q + 1;
      while (*p == '=' || isspace(*p))
        p++;
      q = p;
      while (!isspace(*q) && *q != '\0')
        q++;
      n = q - p;
      strncpy(buffer, p, n * sizeof(char));
      buffer[n] = 0;
      SetParamFromString(name, buffer);
    }
    config_file.close();
    return true;
  } else {
    std::cerr << __FILE__ << ":" << __LINE__ << ": can not open file \'"
              << file_name << "\'" << std::endl;
  }
  return false;
}
