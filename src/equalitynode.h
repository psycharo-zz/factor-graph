#ifndef EQUALITYNODE_H
#define EQUALITYNODE_H


#include "factornode.h"
#include "matrixutil.h"

class EqualityNode : public FactorNode
{
public:
    /**
     * @brief EqualityNode
     * @param _type the default format of messages (TODO: use templates instead of parameters?)
     */
    EqualityNode(GaussianMessage::Type _type = GaussianMessage::GAUSSIAN_VARIANCE)
        :m_type(_type)
    {}


    //! set the type of accepted messages
    void setType(GaussianMessage::Type _type = GaussianMessage::GAUSSIAN_VARIANCE)
    {
        m_type = _type;
    }

    //! get the type of the node
    inline Message::Type type() const
    {
        return m_type;
    }

    //! @overload
    bool isSupported(Message::Type type);

protected:
    /**
     * @brief function
     * @param to
     * @param msgs
     * @return gaussian message (m_j, V_j)
     */
    GaussianMessage function(int to, const MessageBox &msgs);

    GaussianMessage functionVariance(int to, const MessageBox &msgs);
    GaussianMessage functionPrecision(int to, const MessageBox &msgs);

private:
    Message::Type m_type;

};



#endif // EQUALITYNODE_H
