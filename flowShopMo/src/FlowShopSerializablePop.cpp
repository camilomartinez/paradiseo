#include <FlowShopSerializablePop.h>

FlowShopSerializablePop::FlowShopSerializablePop() : eoPop<FlowShop>()
{
}

FlowShopSerializablePop::FlowShopSerializablePop(const eoPop<FlowShop> & _pop) : eoPop<FlowShop>()
{
    for (unsigned int i=0; i<_pop.size(); i++)
    {
        this->push_back(_pop[i]);
    }
}

eoserial::Object* FlowShopSerializablePop::pack() const
{
    eoserial::Object* obj = new eoserial::Object;
    obj->add( "individuals",
            eoserial::makeArray< std::vector<FlowShop>, eoserial::MakeAlgorithm >
            ( *this )
            );
    return obj;
}

void FlowShopSerializablePop::unpack( const eoserial::Object* obj )
{
    this->clear();
    eoserial::unpackArray< std::vector<FlowShop>, eoserial::Array::UnpackAlgorithm >
        ( *obj, "individuals", *this );
}
