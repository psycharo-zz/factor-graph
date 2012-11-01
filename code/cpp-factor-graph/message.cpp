#include "message.h"

const int Message::UNDEFINED_ID;

ostream& operator <<(ostream &os, const GaussianMessage &msg)
{
    os << "mean[ ";
    for (size_t i = 0; i < msg.size(); i++)
        os << msg.mean()[i] << " ";
    os << "]" << endl;

    os << "variance[ ";
    for (size_t i = 0; i < msg.size2(); i++)
        os << msg.variance()[i] << " ";
    os << "]" << endl;
    return os;
}

