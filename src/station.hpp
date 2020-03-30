#pragma once

#include <string>
#include <vector>

class Station {
    public:
        Station();

        long getId() const;

        void setId(long id);

        const std::string& getName() const;

        void setName(const std::string& name);

        const std::string& getDescription() const;

        void setDescription(const std::string& description);

        const std::string& getGenre() const;

        void setGenre(const std::string& genre);

        const std::string& getCountry() const;

        void setCountry(const std::string& country);

        const std::string& getLanguage() const;

        void setLanguage(const std::string& language);

        const std::vector<std::string>& getUrls() const;

        std::vector<std::string> getMutableUrls();
    private:
        long id;
        std::string name;
        std::string description;
        std::string genre;
        std::string country;
        std::string language;
        std::vector<std::string> urls;
};
