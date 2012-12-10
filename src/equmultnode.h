#ifndef EQUMULTNODE_H
#define EQUMULTNODE_H

#include "factornode.h"
#include "matrix.h"

/**
 * @brief The EquMultNode class implements composite block (Table 4) [Loeliger2007]
 * TODO:
 */

class EquMultNode : public FactorNode
{
public:

    bool isSupported(Message::Type type);

    inline void setMatrix(double *_matrix, size_t _rows, size_t _cols)
    {
        m_matrix = Matrix(_matrix, _rows, _cols);
    }

    inline void setMatrix(const Matrix &mx)
    {
        m_matrix = mx;
    }

protected:
    //! @overload
    GaussianMessage function(int to, const MessageBox &msgs);

    //! forward == multiplication
    GaussianMessage functionForward(int to, const MessageBox &msgs);

    //! backward == equality
    GaussianMessage functionBackward(int to, const MessageBox &msgs);

    Matrix m_matrix;

};

#endif // EQUMULTNODE_H
