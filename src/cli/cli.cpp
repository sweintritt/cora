#include "cli/cli.hpp"

#include <plog/Log.h>

#include <sstream>

void Cli::addOption(const char opt, const bool hasValue, const std::string& description) {
   addOption(opt, "", hasValue, description);
}

void Cli::addOption(const std::string& longOpt, const bool hasValue, const std::string& description) {
   addOption(' ', longOpt, hasValue, description);
}

void Cli::addOption(const char opt, const std::string& longOpt, const bool hasValue,
         const std::string& description) {
   Option option;
   option.opt = opt;
   option.longOpt = longOpt;
   option.hasValue = hasValue;
   option.description = description;
   option.found = false;
   m_options.push_back(option);
}

void Cli::parse(int argc, char* argv[]) {
   LOG(plog::debug) << "parsing arguments";
   for (int i = 1; i < argc; ++i)    {
      std::string current = argv[i];
      LOG(plog::debug) << "checking: " << current;
      if (current[0] == '-')       {
         current.erase(0, 1);
         // remove possible second hyphen
         if (current[0] == '-') {
            current.erase(0, 1);
         }

         LOG(plog::debug) << "found option: " << current;
         for (std::list<Option>::iterator it = m_options.begin(); it != m_options.end(); ++it) {
            std::string sopt;
            sopt.push_back(it->opt);
            if (current.compare(sopt) == 0 || current.compare(it->longOpt) == 0) {
               it->found = true;
               if (it->hasValue && (i + 1) < argc) {
                  if (argv[i + 1][0] != '-') {
                     LOG(plog::debug) << "found value: " << argv[i + 1] << " for option: " << current;
                     it->value = argv[i + 1];
                     ++i;
                  }
               }
            }
         }
      }
   }
}

bool Cli::hasOption(const char opt) {
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      if (it->opt == opt && it->found) {
         return true;
      }
   }
   return false;
}

bool Cli::hasOption(const std::string& longOpt) {
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      if (it->longOpt.compare(longOpt) == 0 && it->found) {
         return true;
      }
   }
   return false;
}

bool Cli::hasValue(const char opt) {
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      if (it->opt == opt && it->found && it->hasValue) {
         return !it->value.empty();
      }
   }
   return false;
}

bool Cli::hasValue(const std::string& longOpt) {
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      if (it->longOpt.compare(longOpt) == 0 && it->found && it->hasValue) {
         return !it->value.empty();
      }
   }
   return false;
}

std::string Cli::getValue(const char opt) {
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      if (it->opt == opt && it->found && it->hasValue) {
         return it->value;
      }
   }
   return "";
}

std::string Cli::getValue(const std::string& longOpt) {
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      if (it->longOpt.compare(longOpt) == 0 && it->found && it->hasValue) {
         return it->value;
      }
   }
   return "";
}

std::string Cli::usage(const std::string& application) {
   std::stringstream stream;
   stream << "Usage: " << application << " [OPTIONS]" << std::endl << std::endl;
   stream << "Options:" << std::endl;
   for (std::list<Option>::const_iterator it = m_options.begin(); it != m_options.end(); ++it) {
      stream << "  ";

      if (it->opt != ' ') {
         stream << "-" << it->opt;
      }

      if (it->opt != ' ' && !it->longOpt.empty()) {
         stream << ", ";
      }

      if (!it->longOpt.empty()) {
         stream << "--" << it->longOpt;
      }

      if (it->hasValue) {
         stream << " <VALUE> ";
      }

      // Wrap long descriptions
      const std::string indent = "              ";
      const std::string newLineIndent = "\n              ";
      std::string wrappedDescription = it->description;
      unsigned int line = 0;

      for (unsigned int i = 0; i < wrappedDescription.size(); ++i) {
         if (line >= 60 && wrappedDescription[i] == ' ') {
            wrappedDescription.replace(i, 1, newLineIndent);
            line = 0;
         }
         ++line;
      }

      stream << std::endl << indent << wrappedDescription << std::endl << std::endl;
   }
   return stream.str();
}
