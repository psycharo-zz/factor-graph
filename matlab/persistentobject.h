#ifndef PERSISTENTOBJECT_H
#define PERSISTENTOBJECT_H

/**
 * base class for object persistent between mexFunction calls
 */
class PersistentObject
{
public:
    virtual ~PersistentObject() {}
};

#endif // PERSISTENTOBJECT_H
