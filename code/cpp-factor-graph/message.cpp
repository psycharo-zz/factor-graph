#include "message.h"

ostream& operator <<(ostream &os, const GaussianMessage &msg)
{
    os << msg.from() << " " << msg.to() << endl;

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

