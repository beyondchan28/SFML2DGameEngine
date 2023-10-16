#pragma once
#include <iostream>

class Action
{
    std::string m_name = "NONE";
    std::string m_type = "NONE"; //type would be only START or END

public:
    Action();
    Action(const std::string & name, const std::string & type);

    const std::string & name() const;
    const std::string & type() const;
    std::string toString() const;

};
