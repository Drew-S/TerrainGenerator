```c++
// comment as needed
int var_name;

const int VAR_NAME;

enum MyEnum
{
    OPTION,
    OTHER_OPTION
};

/******************************************************************************
 *                           SECTION AS NEEDED                                *
 ******************************************************************************/

/**
 * myFunction @slot // (if it is a slot)
 * myFunction       // (if it is not a slot)
 * 
 * My function does stuff ...
 * and it does more stuff ...
 * 
 * @param int  param : This is used for ...
 * @param type other : This is used for ...
 *                       longer ...
 * 
 * @returns type : The return value is this ...
 * 
 * @signals name // Any signals that are/can be emitted within the function
 * 
 * @example if any apply
 * 
 * TODO: Todos go last, fixmes, note, etc.
 */
void MyClass::myFunction(...)
{
    ...
}

/**
 * MyClass ...
 * 
 * @param int  param : This is used for ...
 * @param type other : This is used for ...
 *                       longer ...
 */
MyClass::MyClass(...)
{
    ...
}

/**
 * MyClass ...
 * 
 * My class does stuff ...
 * and it does more stuff ...
 */
class MyClass
{
    ...
public:
    // Short description as needed
    MyClass();
    ~MyClass();

    int myPublicFunction(...);

public slots:
...

signals:
...

protected:
...

private slots:
    void _myPrivateFunction(...);
...

private:
    int _myPrivateFunctionOther(...);
    int _my_private_variable;
    Type *_pointers_last;
    ...
};
```

Import ordering (Alphabetical)

```c++
#include "sourceheader.h" // for cpp source only

#include <system/lib>
...

#include <QLibraryItems>
...

#include <3rdPartyLib>
...

#include "./.../mysourcefiles.h"
...

#include "ui_MyUiFiles.h"
...
```