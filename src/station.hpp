#pragma once

#include "author.hpp"
#include <string>
#include <vector>

class Station {
    public:
        Station();
        ~Station();

        void setIdHash(const long idHash);
        long getIdHash();

        void setAuthor(const Author author);
        Author getAuthor();

        void setName(const std::string& name);
        const std::string& getName();

        void setGenre(const std::string& genre);
        const std::string& getGenre();

        void setCountry(const std::string& country);
        const std::string& getCountry();

        void setLanguage(const std::string& language);
        const std::string& getLanguage();

        void setDescription(const std::string& description);
        const std::string& getDescription();

        void addUrl(const std::string& url);
        const std::vector<std::string>& getUrls();

    private:
        long idHash; // Build from name, genre, country and language
        Author author;
        std::string name;
        std::string genre;
        std::string country;
        std::string language;
        std::string description;
        std::vector<std::string> urls;
};
