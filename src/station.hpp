#pragma once

#include "author.hpp"
#include <string>
#include <vector>

class Station {
    public:
        Station();
        ~Station();

        void setId(const long id);
        long getId() const;

        void setIdHash(const long idHash);
        long getIdHash() const;

        void setAuthor(const Author author);
        Author getAuthor() const;

        void setName(const std::string& name);
        const std::string& getName() const;

        void setGenre(const std::string& genre);
        const std::string& getGenre() const;

        void setCountry(const std::string& country);
        const std::string& getCountry() const;

        void setLanguage(const std::string& language);
        const std::string& getLanguage() const;

        void setDescription(const std::string& description);
        const std::string& getDescription() const;

        void addUrl(const std::string& url);
        const std::vector<std::string>& getUrls() const;

    private:
        // TODO int64_t
        long id;
        long idHash; // Build from name, genre, country and language
        Author author;
        std::string name;
        std::string genre;
        std::string country;
        std::string language;
        std::string description;
        std::vector<std::string> urls;
};
