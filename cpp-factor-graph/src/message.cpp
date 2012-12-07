#include "message.h"

const int Message::UNDEFINED_ID = -1;

using namespace std;

ostream& operator <<(ostream &os, const GaussianMessage &msg)
{
    os << "mean[ ";
    for (size_t i = 0; i < msg.size(); i++)
        os << msg.mean()[i] << " ";
    os << "]" << endl;

    const double *data = msg.variance().data();
    os << ((msg.type() == Message::GAUSSIAN_VARIANCE) ? "variance[ " : "precision[ ");
    for (size_t i = 0; i < msg.size2(); i++)
        os << data[i] << " ";
    os << "]" << endl;
    return os;
}

