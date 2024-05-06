#ifndef FIMS_COMMON_FIMS_LOG
#define FIMS_COMMON_FIMS_LOG 


#include <cstdlib>
#include <vector>
#include <chrono>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <stdlib.h>
#include <fstream>
#include <signal.h>
#include <csignal>
#include <cstring>
#include <memory>
#include <stdexcept>


#if defined(linux) || defined(__linux) || defined(__linux__)
#define FIMS_LINUX
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#define FIMS_BSD
#elif defined(sun) || defined(__sun)
#define FIMS_SOLARIS
#elif defined(__sgi)
#define FIMS_IRIX
#elif defined(__hpux)
#define FIMS_HPUX
#elif defined(__CYGWIN__)
#define FIMS_CYGWIN
#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#define FIMS_WIN32
#elif defined(_WIN64) || defined(__WIN64__) || defined(WIN64)
#define FIMS_WIN64
#elif defined(__BEOS__)
#define FIMS_BEOS
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#define FIMS_MACOS
#elif defined(__IBMCPP__) || defined(_AIX)
#define FIMS_AIX
#elif defined(__amigaos__)
#define FIMS_AMIGAOS
#elif defined(__QNXNTO__)
#define FIMS_QNXNTO
#endif

#if defined(FIMS_WIN32) || defined(FIMS_WIN64)
#define FIMS_WINDOWS
#endif

#ifdef FIMS_WINDOWS
#include <Windows.h>
//#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
  #ifdef FIMS_WINDOWS
    #define __PRETTY_FUNCTION__ __FUNCTION__
  #endif
#endif


struct LogEntry {
  std::string timestamp;
  std::string message;
  std::string level;
  size_t rank;
  std::string user;
  std::string wd;
  std::string file;
  std::string func;
  size_t line;

  std::string to_string() {
    std::stringstream ss;
    ss << "\"timestamp\" : " << "\"" << this->timestamp << "\"" << ",\n";
    ss << "\"level\" : " << "\"" << this->level << "\",\n";
    ss << "\"message\" : " << "\"" << this->message << "\",\n";
    ss << "\"id\" : " << "\"" << this->rank << "\",\n";
    ss << "\"user\" : " << "\"" << this->user << "\",\n";
    ss << "\"wd\" : " << "\"" << this->wd << "\",\n";
    ss << "\"file\" : " << "\"" << this->file << "\",\n";
    ss << "\"routine\" : " << "\"" << this->func << "\",\n";
    ss << "\"line\" : " << "\"" << this->line << "\"\n";
    return ss.str();
  }

};

class FIMSLog {
  std::vector<std::string> entries;
  std::vector<LogEntry> log_entries;
  size_t entry_number = 0;
  std::string path = "fims.log";

  std::string get_user() {
    char * user;
#ifdef FIMS_WINDOWS
    user = getenv("username");
#endif
#ifdef FIMS_LINUX
    user = getenv("USERNAME");
#endif

#ifdef FIMS_MACOS
    user = getenv("USER");
#endif

    return std::string(user);
  }
public:
  bool write_on_exit = true;
    bool throw_on_error = false;
  static std::shared_ptr<FIMSLog> fims_log;

  FIMSLog() {

  }

  ~FIMSLog() {
    if (this->write_on_exit) {
      std::ofstream log(this->path);
      log << this->get_log();
      log.close();
    }
  }

  void set_path(std::string path) {
    this->path = path;
  }

  std::string get_path() {
    return this->path;
  }

  void message(std::string str, int line, const char* file, const char* func ) {

    std::filesystem::path cwd = std::filesystem::current_path();
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::string ctime_no_newline = strtok(ctime(&now_time), "\n");

    LogEntry l;
    l.timestamp = ctime_no_newline;
    l.message = str;
    l.level = "info";
    l.rank = this->log_entries.size();
    l.user = this->get_user();
    l.wd = cwd.string();
    l.file = file;
    l.line = line;
    l.func = func;
    this->log_entries.push_back(l);

  }

  void error_message(std::string str, int line, const char* file, const char* func ) {
      std::filesystem::path cwd = std::filesystem::current_path();

    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::string ctime_no_newline = strtok(ctime(&now_time), "\n");

    LogEntry l;
    l.timestamp = ctime_no_newline;
    l.message = str;
    l.level = "error";
    l.rank = this->log_entries.size();
    l.user = this->get_user();
    l.wd = cwd.string();
    l.file = file;
    l.line = line;
    l.func = func;
    this->log_entries.push_back(l);
      
      if (this->throw_on_error) {
          throw std::runtime_error(l.to_string().c_str());
      }

  }

  void warning_message(std::string str, int line, const char* file, const char* func ) {
      std::filesystem::path cwd = std::filesystem::current_path();

    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::string ctime_no_newline = strtok(ctime(&now_time), "\n");

    LogEntry l;
    l.timestamp = ctime_no_newline;
    l.message = str;
    l.level = "warning";
    l.rank = this->log_entries.size();
    l.user = this->get_user();
    l.wd = cwd.string();
    l.file = file;
    l.line = line;
    l.func = func;
    this->log_entries.push_back(l);

  }

  std::string get_log() {
    std::stringstream ss;
    if (log_entries.size() == 0) {
      ss << "[\n]";
    } else {
      ss << "[\n";
      for (size_t i = 0; i < log_entries.size() - 1; i++) {
        ss << "{\n" << this->log_entries[i].to_string() << "},\n";

      }
      ss << "{\n" << this->log_entries[log_entries.size() - 1].to_string() << "}\n]";
    }
    return ss.str();
  }

  std::string get_errors() {
    std::stringstream ss;
    std::vector<LogEntry> errors;
    for (size_t i = 0; i < log_entries.size(); i++) {
      if (log_entries[i].level == "error") {
        errors.push_back(this->log_entries[i]);
      }
    }

    if (errors.size() == 0) {
      ss << "[\n]";
    } else {
      ss << "[\n";
      for (size_t i = 0; i < errors.size() - 1; i++) {

        ss << "{\n" << errors[i].to_string() << "},\n";

      }

      ss << "{\n" << errors[errors.size() - 1].to_string() << "}\n]";

    }
    return ss.str();
  }

  std::string get_warnings() {
    std::stringstream ss;
    std::vector<LogEntry> warnings;
    for (size_t i = 0; i < log_entries.size(); i++) {
      if (log_entries[i].level == "warning") {
        warnings.push_back(this->log_entries[i]);
      }
    }

    if (warnings.size() == 0) {
      ss << "[\n]";
    } else {
      ss << "[\n";
      for (size_t i = 0; i < warnings.size() - 1; i++) {

        ss << "{\n" << warnings[i].to_string() << "},\n";

      }

      ss << "{\n" << warnings[warnings.size() - 1].to_string() << "}\n]";

    }
    return ss.str();
  }

  std::string get_info() {
    std::stringstream ss;
    std::vector<LogEntry> info;
    for (size_t i = 0; i < log_entries.size(); i++) {
      if (log_entries[i].level == "info") {
        info.push_back(this->log_entries[i]);
      }
    }

    if (info.size() == 0) {
      ss << "[\n]";
    } else {
      ss << "[\n";
      for (size_t i = 0; i < info.size() - 1; i++) {

        ss << "{\n" << info[i].to_string() << "},\n";

      }

      ss << "{\n" << info[info.size() - 1].to_string() << "}\n]";

    }
    return ss.str();
  }

  std::string get_module(const std::string& module) {
    std::stringstream ss;
    std::vector<LogEntry> info;
    for (size_t i = 0; i < log_entries.size(); i++) {
      if (log_entries[i].file.find(module) != std::string::npos) {
        info.push_back(this->log_entries[i]);
      }
    }

    if (info.size() == 0) {
      ss << "[\n]";
    } else {
      ss << "[\n";
      for (size_t i = 0; i < info.size() - 1; i++) {

        ss << "{\n" << info[i].to_string() << "},\n";

      }

      ss << "{\n" << info[info.size() - 1].to_string() << "}\n]";

    }
    return ss.str();
  }




};

std::shared_ptr<FIMSLog> FIMSLog::fims_log = std::make_shared<FIMSLog>();




#define FIMS_LOG(MESSAGE) FIMSLog::fims_log->message(MESSAGE, __LINE__, __FILE__, __PRETTY_FUNCTION__);
#define FIMS_WARNING(MESSAGE) FIMSLog::fims_log->warning_message(MESSAGE, __LINE__, __FILE__, __PRETTY_FUNCTION__);
#define FIMS_ERROR(MESSAGE) FIMSLog::fims_log->error_message(MESSAGE, __LINE__, __FILE__, __PRETTY_FUNCTION__);

void WriteAtExit(int sig) {


  if (FIMSLog::fims_log->write_on_exit) {
    std::ofstream log(FIMSLog::fims_log->get_path());
      log << FIMSLog::fims_log->get_log();
    log.close();
  }
  std::signal(sig, SIG_DFL);
  raise(sig);
}



namespace fims {

template<typename T>
std::string to_string(T v) {
  std::stringstream ss;
  ss << v;
  return ss.str();
}

}



#endif
