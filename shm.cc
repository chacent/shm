#include "shm.h"

#include <boost/interprocess/mapped_region.hpp>


enum OpenType {
	create_only,
	open_only,
	open_or_create
};
Nan::Persistent<v8::Function> SharedMemory::constructor;

SharedMemory::SharedMemory(const char * name, unsigned opentype, unsigned _mode) {
	boost::interprocess::mode_t mode = (boost::interprocess::mode_t)_mode;
	switch (opentype) {
	case create_only:
		_data = new boost::interprocess::shared_memory_object(boost::interprocess::create_only, name, mode);
		break;
	case open_only:
		_data = new boost::interprocess::shared_memory_object(boost::interprocess::open_only, name, mode);
		break;
	case open_or_create:
		_data = new boost::interprocess::shared_memory_object(boost::interprocess::open_or_create, name, mode);
		break;
	default:
		 throw std::runtime_error("invalid open type");
	}
}

SharedMemory::~SharedMemory() {
	delete _data;
}

NAN_PROPERTY_GETTER(GetCode_Read) {
	info.GetReturnValue().Set(Nan::New(boost::interprocess::read_only));
}
NAN_PROPERTY_GETTER(GetCode_Write) {
	info.GetReturnValue().Set(Nan::New(boost::interprocess::read_write));
}
NAN_PROPERTY_GETTER(GetCode_COW) {
	info.GetReturnValue().Set(Nan::New(boost::interprocess::copy_on_write));
}


NAN_PROPERTY_GETTER(GetCode_Open) {
	info.GetReturnValue().Set(Nan::New(open_only));
}
NAN_PROPERTY_GETTER(GetCode_Create) {
	info.GetReturnValue().Set(Nan::New(create_only));
}
NAN_PROPERTY_GETTER(GetCode_OpenCreate) {
	info.GetReturnValue().Set(Nan::New(open_or_create));
}

NAN_MODULE_INIT(SharedMemory::Init) {
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New("SharedMemory").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);

	Nan::SetPrototypeMethod(tpl, "truncate", Truncate);
	Nan::SetPrototypeMethod(tpl, "size", Size);
	Nan::SetPrototypeMethod(tpl, "remove", Remove);
	Nan::SetPrototypeMethod(tpl, "map", Map);
	v8::Local<v8::Function> func = Nan::GetFunction(tpl).ToLocalChecked();
	Nan::SetAccessor(Nan::To<v8::Object>(func).ToLocalChecked(), Nan::New("read").ToLocalChecked(), GetCode_Read);
	Nan::SetAccessor(Nan::To<v8::Object>(func).ToLocalChecked(), Nan::New("write").ToLocalChecked(), GetCode_Write);
	Nan::SetAccessor(Nan::To<v8::Object>(func).ToLocalChecked(), Nan::New("cow").ToLocalChecked(), GetCode_COW);
	Nan::SetAccessor(Nan::To<v8::Object>(func).ToLocalChecked(), Nan::New("create").ToLocalChecked(), GetCode_Create);
	Nan::SetAccessor(Nan::To<v8::Object>(func).ToLocalChecked(), Nan::New("open").ToLocalChecked(), GetCode_Open);
	Nan::SetAccessor(Nan::To<v8::Object>(func).ToLocalChecked(), Nan::New("open_create").ToLocalChecked(), GetCode_OpenCreate);
	constructor.Reset(func);
	Nan::Set(target, Nan::New("exports").ToLocalChecked(), func);
}

NAN_METHOD(SharedMemory::New) {
	if (info.IsConstructCall()) {
		if (info.Length() < 3) {
			Nan::ThrowTypeError("wrong number of arguments");
			return;
		}

		if (!info[0]->IsString() || !info[1]->IsUint32() || !info[2]->IsInt32()) {
			Nan::ThrowTypeError("wrong arguments");
			return;
		}
		const char* name = *(Nan::Utf8String(info[0]));
		uint32_t type = Nan::To<uint32_t>(info[1]).FromJust();
		uint32_t mode = Nan::To<uint32_t>(info[2]).FromJust();
		try {
			SharedMemory *obj = new SharedMemory(name, type, mode);
			obj->Wrap(info.This());
			info.GetReturnValue().Set(info.This());
		}
		catch (std::exception& e) {
			Nan::ThrowError(e.what());
		}
	}
	else {
		const int argc = 3;
		v8::Local<v8::Value> argv[argc] = { info[0],info[1],info[2] };
		v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
		info.GetReturnValue().Set(cons->NewInstance(argc, argv));
	}
}

NAN_METHOD(SharedMemory::Truncate) {
	try {
		if (info.Length() != 1 && !info[0]->IsUint32()) {
			Nan::ThrowTypeError("wrong arguments");
			return;
		}
		uint32_t size = Nan::To<uint32_t>(info[0]).FromJust();
		SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(info.Holder());
		obj->_data->truncate(size);
	}
	catch (std::exception& e) {
		Nan::ThrowError(e.what());
	}
}

NAN_METHOD(SharedMemory::Size) {
	try {
		SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(info.Holder());
		boost::interprocess::offset_t size;
		obj->_data->get_size(size);
		info.GetReturnValue().Set(Nan::New((unsigned)size));
	}
	catch (std::exception& e) {
		Nan::ThrowError(e.what());
	}
}


NAN_METHOD(SharedMemory::Remove) {
	try {
		SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(info.Holder());
		boost::interprocess::shared_memory_object::remove(obj->_data->get_name());
	}
	catch (std::exception& e) {
		Nan::ThrowError(e.what());
	}
}

static void free_callback(char*, void* _) {
	boost::interprocess::mapped_region* region = (boost::interprocess::mapped_region*)_;
	delete region;
}

NAN_METHOD(SharedMemory::Map) {
	try {
		int argc = info.Length();
		if (argc < 1) {
			Nan::ThrowTypeError("wrong number of arguments");
			return;
		}
		if (!info[0]->IsInt32()) {
			Nan::ThrowTypeError("wrong arguments");
			return;
		}
		unsigned mode = Nan::To<uint32_t>(info[0]).FromJust();
		unsigned offset = 0;
		boost::interprocess::offset_t size;
		if (argc > 1) {
			if (!info[1]->IsUint32()) {
				Nan::ThrowTypeError("wrong arguments");
				return;
			}
			offset = Nan::To<uint32_t>(info[1]).FromJust();
		}
		SharedMemory* obj = ObjectWrap::Unwrap<SharedMemory>(info.Holder());
		if (argc> 2) {
			if (!info[2]->IsUint32()) {
				Nan::ThrowTypeError("wrong arguments");
				return;
			}
			size = Nan::To<uint32_t>(info[2]).FromJust();
		}
		else {
			obj->_data->get_size(size);
			size -= offset;
		}
		boost::interprocess::mapped_region* region = new boost::interprocess::mapped_region(*obj->_data, (boost::interprocess::mode_t)mode,offset,size);
		info.GetReturnValue().Set(Nan::NewBuffer((char*)region->get_address(),size, free_callback, region).ToLocalChecked());
	}
	catch (std::exception& e) {
		Nan::ThrowError(e.what());
	}
}