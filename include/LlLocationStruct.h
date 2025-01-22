#include "Ll.h"

class LlLocationTypeAlias : public LlLocation {
private:
    const std::string* aliasName;

public:
    LlLocationTypeAlias(const std::string* aliasName)
        : LlLocation(aliasName), aliasName(aliasName) {}

    ~LlLocationTypeAlias() override {
        delete aliasName;
    }

    // Override getVarName to return aliasName directly
    const std::string* getAliasTypeName() const {
        return aliasName;
    }

    // Override toString for clarity
    std::string toString() override {
        return "TypeAlias: " + *aliasName;
    }
};

class LlLocationStruct : public LlLocation {
private:
    LlLocation* baseLocation;   
    std::string fieldName;      // Field name within the struct
    int offset;                 

public:
    LlLocationStruct(LlLocation* baseLocation, const std::string& fieldName, int offset)
        : LlLocation(baseLocation->getVarName()), baseLocation(baseLocation), fieldName(fieldName), offset(offset) {}

    ~LlLocationStruct() override {
        delete baseLocation;
    }

    LlLocation* getBaseLocation() const {
        return baseLocation;
    }

    std::string getFieldName() const {
        return fieldName;
    }

    int getOffset() const {
        return offset;
    }

    std::string toString() override {
        return baseLocation->toString() + "->" + fieldName;
    }

    bool operator==(const Ll& other) const override {
        if (auto* otherField = dynamic_cast<const LlLocationStruct*>(&other)) {
            return *baseLocation == *(otherField->baseLocation) && fieldName == otherField->fieldName;
        }
        return false;
    }

    std::size_t hashCode() const override {
        return baseLocation->hashCode() ^ std::hash<std::string>()(fieldName);
    }
};
