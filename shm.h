#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <nan.h>
#define BOOST_ALL_NO_LIB
#define BOOST_LIB_DIAGNOSTIC
#include <boost/interprocess/shared_memory_object.hpp>

class SharedMemory : public Nan::ObjectWrap {
public:
	static NAN_MODULE_INIT(Init);
private:
	explicit SharedMemory(const char * name, unsigned opentype, unsigned mode);
	~SharedMemory();
	boost::interprocess::shared_memory_object* _data;

	static NAN_METHOD(New);
	static NAN_METHOD(Truncate);
	static NAN_METHOD(Size);
	static NAN_METHOD(Remove);
	static NAN_METHOD(Map);

	static Nan::Persistent<v8::Function> constructor;
};

#endif
