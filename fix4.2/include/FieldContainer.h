#ifndef EXCHANGESIMULATOR_FIX42_FIELDCONTAINER_H
#define EXCHANGESIMULATOR_FIX42_FIELDCONTAINER_H

#include "Common.h"
#include "Exceptions.h"
#include "FieldValue.h"
#include <map>
#include <memory>

__FIX42_BEGIN

class FieldValue;

template<class T>
class FieldContainer {
public:
    const FieldValuePtr get(Tag tag) const noexcept {
        auto it = fields_.find(tag);
        return (it != fields_.cend()) ?
               it->second : FieldValuePtr();
    }

    void set(Tag tag, FieldValuePtr value, bool override = true) throw(DuplicateTag) {
        if (override || fields_.find(tag) == fields_.end()) {
            //override or key does not exist
            fields_[tag] = value;
            return;
        }
        throw DuplicateTag();
    }

    bool contains(Tag tag) const noexcept {
        return fields_.find(tag) != fields_.cend();
    }

    std::string toString() const noexcept {
        std::string result;
        for (auto field: fields_) {
            result += std::to_string(field.first) + '=' + field.second->toString() + DELIMITER;
        }
        return result;
    }

private:
    std::map<Tag, std::shared_ptr<T>> fields_;
};

class FieldValue;
class RepeatGroup;

typedef FieldContainer<FieldValue> FieldValueContainer;
typedef FieldContainer<RepeatGroup> RepeatGroupContainer;

__FIX42_END

#endif //EXCHANGESIMULATOR_FIX42_FIELDGROUP_H