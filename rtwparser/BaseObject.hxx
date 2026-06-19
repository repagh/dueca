/* ------------------------------------------------------------------   */
/*      item            : BaseObject.hxx
        made by         : Joost Ellerbroek
        date            : 080129
        category        : header file
        description     :
        changes         : 080129 first version
        language        : C++
        copyright       : (c) 2016 TUDelft-AE-C&S
        copyright       : (c) 2022 Joost Ellerbroek
        license         : EUPL-1.2
*/

#ifndef BASE_OBJECT_HXX
#define BASE_OBJECT_HXX
#include <string>
#include <list>
#include <map>

class BaseObject
{
  public:
    /** Constructor.
    \param type     character string of the type
    \param name     name of the member variable
    \param size     dimension, a positive size means that a C-style array
    of fixed length is generated. */
    BaseObject(const char* type, const char* name);

    /// Destructor.
    virtual ~BaseObject();

    /// Return the name of the object
    const std::string& getName() const               {return name;}
    /// Return the unparsed comment string
    const std::string& getComment() const            {return comment;}
    /// Return the variable's type
    const std::string& getType() const               {return type;}
    /// Return the size: implemented in derived objects
    virtual int   getSize() const =0;

    /** Add a comment to this type */
    void addComment(const std::string& c)            {comment =c;}

    /** Set the name of the project model */
    static void setModelName(const char* name)  {modelname = name;}

    /// Return the name of the model we are parsing
    static const std::string& getModelName()         {return modelname;}

  private:
    /// Stores for comments, variable type, and name
    std::string          comment, type, name;
    /// Store for the name of the model we are parsing
    static std::string   modelname;
};

typedef std::list<BaseObject*> ObjectList;
typedef std::map<std::string, BaseObject*> ObjectMap;
#endif
