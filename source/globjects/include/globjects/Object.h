#pragma once

#include <string>

#include <glbinding/gl/types.h>

#include <globjects-base/Referenced.h>

#include <globjects/globjects_api.h>

namespace glo {

class ObjectVisitor;
class IDResource;

/** \brief Superclass of all wrapped OpenGL objects.
    
    The superclass is Referenced so that each wrapped OpenGL object supports reference counting.
    The OpenGL name (id) of the OpenGL resource that was provided in the constructor can be queried using id().
    Additionally, an Object can have meaningful name wich can be get and set using name() and setName().
 */
class GLOBJECTS_API Object : public Referenced
{
    friend class AbstractObjectNameImplementation;
public:
    virtual void accept(ObjectVisitor & visitor) = 0;

    gl::GLuint id() const;

    std::string name() const;
    void setName(const std::string & name);
    bool hasName() const;

    bool isDefault() const;

    virtual gl::GLenum objectType() const = 0;
protected:
    Object(IDResource * resource);
    virtual ~Object();

protected:
    IDResource * m_resource;

    mutable void * m_objectLabelState;
};

} // namespace glo
