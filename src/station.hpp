#pragma once

#include <string>
#include <vector>
#include <cstdint>

class Station {
    public:
        Station();
        ~Station();

        void setId(const int64_t id);
        int64_t getId() const;

        void setAddedBy(const std::string& addedBy);
        std::string getAddedBy() const;

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
        int64_t m_id;
        std::string m_addedBy;
        std::string m_name;
        std::string m_genre;
        std::string m_country;
        std::string m_language;
        std::string m_description;
        std::vector<std::string> m_urls;
};
