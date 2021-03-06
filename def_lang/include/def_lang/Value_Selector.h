#pragma once

#include <string>
#include <vector>

namespace ts
{

class Value_Selector final
{
public:

    Value_Selector() = default;
    Value_Selector(std::string const& str);
    Value_Selector(char const* str);

    //---------------------------------------------------------

    struct Element
    {
        enum class Type
        {
            NAME,
            INDEX,
        };

        //----------------------

        Element();
        ~Element();

        Element(Type type);
        Element(size_t index);
        Element(std::string const& name);

        Element(Element const& other);
        Element(Element&& other);
        Element& operator=(Element const& other);
        Element& operator=(Element&& other);


        //----------------------

        Type type = Type::NAME;
        union
        {
            size_t index;
            std::string name;
        };
    };

    //---------------------------------------------------------

    size_t get_element_count() const;
    Element const& get_element(size_t idx) const;

    bool empty() const;

    Element const& front() const;
    Element pop_front();
    void push_front(Element const& element);

    Element const& back() const;
    Element pop_back();
    void push_back(Element const& element);

    std::string to_string() const;

private:
    void parse(std::string const& str);

    std::vector<Element> m_elements;
};


}
