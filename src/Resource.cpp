#include "jgl.h"

namespace jgl {

    typedef jutil::Pair<ResourceID, size_t> IDCounter;
    typedef jutil::Queue<IDCounter> IDList;
    typedef jutil::Queue<IDList> ResourceManager;

    ResourceManager loadedResources;

    ResourceType &operator++(ResourceType &t) {
        if (t == __RESOURCE_TYPE_SIZE__) return t;
        return t = static_cast<ResourceType>(static_cast<uint8_t>(t) + 1);
    }

    Resource::Resource(ResourceType t, ResourceID id) : _id(id), _type(t) {
        if (loadedResources.empty()) for (ResourceType i = TEXTURE; i < __RESOURCE_TYPE_SIZE__; ++i) loadedResources.insert(IDList());
    }

    const ResourceType &Resource::type() const {
        return _type;
    }

    bool Resource::acquire() {

        size_t *loaded = nullptr;

        for (auto &i: loadedResources[_type]) {
            if (i.first() == _id) {
                loaded = &i.second();
                break;
            }
        }

        if (_type == __RESOURCE_TYPE_SIZE__ || _type == UNINITIALIZED) {
            getCore()->errorHandler(0xaf2, "Attempted to acquire a jgl resource of invalid type!");
            return false;
        }
        if (_id == 0) {
            getCore()->errorHandler(0xaf3, "Attempted to acquire a jgl resource without first generating it!");
            return false;
        }
        if (!loaded) {
            //jutil::out << "(" << static_cast<uint32_t>(static_cast<uint8_t>(_type)) << ") " << _id << " : created." << jutil::endl;

            loadedResources[_type].insert(IDCounter(_id, 1));

        } else {
            //jutil::out << "(" << static_cast<uint32_t>(static_cast<uint8_t>(_type)) << ") " << _id << " : " << loadedResources[_type][_id] << " -> ";
            //++(loadedResources[_type][_id]);
            *loaded += 1;
            //jutil::out << loadedResources[_type][_id] << jutil::endl;
        }
        return true;
    }

    bool Resource::release() {

        size_t *loaded = nullptr;

        for (auto &i: loadedResources[_type]) {
            if (i.first() == _id) {
                loaded = &i.second();
                break;
            }
        }

        if (!loaded) {
            jutil::String errcode = jutil::String("Attempted to release jgl resource which jgl never acquired! (") + jutil::String(_type) + jutil::String(", ") + jutil::String(_id) + jutil::String(")");
            char errArr[errcode.size() + 1];
            errcode.array(errArr);
            getCore()->errorHandler(0xa0f, errArr);
            return false;
        }

        if (loaded && *loaded > 0) {
            //jutil::out << "(" << static_cast<uint32_t>(static_cast<uint8_t>(_type)) << ") " << _id << " : " << loadedResources[_type][_id] << " -> ";
            *loaded -= 1;
            //jutil::out << loadedResources[_type][_id];
        } else {
            getCore()->errorHandler(0xa0f1, "Attempted to release jgl resource which was already released by other means!");
            loadedResources[_type].erase(_id);
            destroy();
            return false;
        }

        if (loaded && *loaded == 0) {
            destroy();
            //jutil::out << "(destroyed)";
        }
        //jutil::out << jutil::endl;
        return true;
    }

    const ResourceID &Resource::id() const {
        return _id;
    }

    Resource::~Resource() {}
}
