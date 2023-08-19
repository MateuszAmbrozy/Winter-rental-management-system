#pragma once
struct Date
{
    sqlite3* db = nullptr;
    int rc;

    Date() // Konstruktor
    {
        this->rc = sqlite3_open("DB", &this->db);
        if (this->rc != SQLITE_OK)
        {
            std::cerr << "Cannot open database: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(this->db);
            this->db = nullptr;
        }
    }

    ~Date() // Destruktor
    {
        if (this->db != nullptr)
        {
            sqlite3_close(this->db);
        }
    }
};

template<typename T>
static T io_read(const char* msg, const char* errmsg, std::istream& is = std::cin, std::ostream& os = std::cout)
{
    T result{};
    std::string input;
    while ((os << msg) && std::getline(is, input))
    {
        std::istringstream iss(input);
        if ((iss >> std::noskipws >> result) && (result >= 0))
        {
            break;
        }
        else
        {
            os << errmsg;
            is.clear();
            is.ignore(LLONG_MAX, '\n');
        }
    }
    return result;
}
