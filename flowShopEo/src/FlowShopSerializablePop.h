#ifndef FLOWSHOPSERIALIZABLEPOP_H
#define FLOWSHOPSERIALIZABLEPOP_H

// EO includes
#include <eoPop.h>
#include <serial/Serializable.h>

// FlowShop includes
#include <FlowShop.h>

/**
Serializable population

  @ingroup Core
 */
class FlowShopSerializablePop : public eoPop<FlowShop>, public eoserial::Persistent
{
    public:
        FlowShopSerializablePop();
        FlowShopSerializablePop(const eoPop<FlowShop> & _pop);

        eoserial::Object* pack() const;
        void unpack( const eoserial::Object* obj );
};

#endif